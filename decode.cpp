extern "C"
{
#include <libavformat/avformat.h>
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <sys/time.h>
}

#include "ffsdl.h"
#include <iostream>
using namespace::std;

#define TAG "MediaDecoder"

#define REFRESH_EVENT  (SDL_USEREVENT + 1)

class MediaDecoder {
    public:
        MediaDecoder();
        void setDataSource(const char *url);
        int prepare();
        int initCodec();
        int getPacket(AVPacket *pkt);
        int getFrame(AVPacket *pkt, AVFrame *frame);

        void setOutVideoWidth(int w);
        void setOutVideoHeight(int h);
        void setOutVideoPixFmt(AVPixelFormat fmt);
        AVFrame* convertVideoFrame(AVFrame *src);
        void initVideoConvert();
        AVRational getVideoTimeBase();
        int getVideoIndex();
        int getDisPlayWidth();
        int getDisPlayHeight();
        int getVideoWidth();
        int getVideoHeight();
        void setDisPlayWidth(int w);
        void setDisPlayHeight(int h);

        int getAudioIndex();
        void initAudioConvert();
        AVFrame* convertAudioFrame(AVFrame *src);
        void setOutAudioFromat(AVSampleFormat fmt);
        void setOutAudioSampleRate(int rate);
        void setOutAudioLayout(uint64_t layout);
        void setOutAudioChannels(int channels);
        uint64_t getAudioLayout();
        int getSampleRate();
        int getChannels();
        AVSampleFormat getAudioFormat();

        static int64_t getMsByPts(AVRational time_base, int64_t pts);
        static int64_t getCurMs();
        static int64_t getCurUs();
    private:
        bool hasVideo;
        bool hasAudio;
        int videoIndex;
        int audioIndex;
        int videoWidth;
        int videoHeight;
        int displayWidth;
        int displayHeight;
        AVRational videoTimeBase;
        AVPixelFormat videoPixFmt;
        AVPixelFormat outPixFmt;
        SwsContext *swsVideoCtx;

        SwrContext *swrAudioCtx;
        int audioSampleRate;
        int audioChannels;
        AVSampleFormat audioSampleFormat;
        int audioNbSamples;
        uint64_t audioLayout;
        int outSampleRate;
        int outChannels;
        AVSampleFormat outSampleFormat;
        int outNbSamples;
        uint64_t outLayout;

        string url;
        AVFormatContext *inputFormatContext;
};

MediaDecoder::MediaDecoder():hasVideo(false),
    hasAudio(false),
    videoIndex(-1),
    audioIndex(-1),
    videoWidth(-1),
    videoHeight(-1),
    displayWidth(-1),
    displayHeight(-1),
    inputFormatContext(NULL),
    swsVideoCtx(NULL),
    swrAudioCtx(NULL),
    url("")
{
    av_register_all();
    avdevice_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_DEBUG);
}

int MediaDecoder::getVideoWidth() {
    return videoWidth;
}

int MediaDecoder::getVideoHeight() {
    return videoHeight;
}

int MediaDecoder::getDisPlayWidth() {
    return displayWidth;
}

int MediaDecoder::getDisPlayHeight() {
    return displayHeight;
}

void MediaDecoder::setDisPlayWidth(int w) {
    displayWidth = w;
}

void MediaDecoder::setDisPlayHeight(int h) {
    displayHeight = h;
}

int MediaDecoder::getVideoIndex() {
    return videoIndex;
}

int MediaDecoder::getAudioIndex() {
    return audioIndex;
}

void MediaDecoder::setOutVideoWidth(int width) {
    displayWidth = width;
}

AVSampleFormat MediaDecoder::getAudioFormat() {
    return audioSampleFormat;
}

int MediaDecoder::getSampleRate() {
    return audioSampleRate;
}

uint64_t MediaDecoder::getAudioLayout() {
    return audioLayout;
}

int MediaDecoder::getChannels() {
    return audioChannels;
}

void MediaDecoder::setOutVideoHeight(int height) {
    displayHeight = height;
}

void MediaDecoder::setOutAudioSampleRate(int rate) {
    outSampleRate;
}

void MediaDecoder::setOutAudioFromat(AVSampleFormat fmt) {
    outSampleFormat = fmt;
}

void MediaDecoder::setOutAudioChannels(int ch) {
    outChannels = ch;
}

void MediaDecoder::setOutAudioLayout(uint64_t layout) {
    outLayout = layout;
}

void MediaDecoder::setOutVideoPixFmt(AVPixelFormat fmt) {
    outPixFmt = fmt;
    if(outPixFmt == videoPixFmt) {
        av_log(NULL, AV_LOG_DEBUG, "outPixFmt == videoPixFmt\n");
    }
}

void MediaDecoder::setDataSource(const char* url) {
    this->url = url;
}

int MediaDecoder::prepare() {
    int ret = avformat_open_input(&inputFormatContext, url.c_str(), NULL, NULL);
    if(ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_open_input error!");
        return ret;
    }
    ret = avformat_find_stream_info(inputFormatContext, NULL);
    if(ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "avformat_find_stream_info error!");
        return -1;
    }
    if(initCodec() >= 0) {
        return ret;
    }
    return -1;
}

AVRational MediaDecoder::getVideoTimeBase() {
    return videoTimeBase;
}

int MediaDecoder::initCodec() {
    int ret = 0;
    for(int i = 0;i < inputFormatContext->nb_streams;i++) {
        AVStream *inStream = inputFormatContext->streams[i];
        AVCodec *dec = NULL;
        AVCodecContext *dec_ctx = NULL;
        dec_ctx = inStream->codec;
        dec = avcodec_find_decoder(dec_ctx->codec_id);
        if(dec == NULL) {
            av_log(NULL, AV_LOG_ERROR, "avcodec_find_decoder find error %d\n", __LINE__);
            return -1;
        }
        ret = avcodec_open2(dec_ctx, dec, NULL);
        if(ret < 0) {
            av_log(NULL, AV_LOG_ERROR, "avcodec_open2 err in line %d\n", __LINE__);
            return -1;
        }
        if(inStream->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            hasVideo = true;
            videoIndex = i;
            videoWidth = dec_ctx->width;
            videoHeight = dec_ctx->height;
            videoPixFmt = dec_ctx->pix_fmt;
            videoTimeBase = inStream->time_base;
        } else if(inStream->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioSampleRate = dec_ctx->sample_rate;
            audioChannels = dec_ctx->channels;
            audioSampleFormat = dec_ctx->sample_fmt;
            audioLayout = dec_ctx->channel_layout;
            hasAudio = true;
            audioIndex = i;
        }
        av_dump_format(inputFormatContext, i, url.c_str(), 0);
    }
    av_log(NULL, AV_LOG_DEBUG, "videoIndex %d audioIndex %d\n", videoIndex, audioIndex);
    return 1;
}

int64_t MediaDecoder::getMsByPts(AVRational time_base, int64_t pts) {
    return av_rescale_q (pts, time_base, AV_TIME_BASE_Q)/1000;
}

int64_t MediaDecoder::getCurMs() {
    struct timeval start;
    gettimeofday(&start, NULL  );
    return start.tv_usec/1000 + start.tv_sec*1000;
}

int64_t MediaDecoder::getCurUs() {
    struct timeval start;
    gettimeofday(&start, NULL  );
    return start.tv_usec + start.tv_sec*1000*1000;
}

int MediaDecoder::getPacket(AVPacket *pkt) {
    int ret = av_read_frame(inputFormatContext, pkt);
    if(ret < 0) {
        char errstr[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errstr, AV_ERROR_MAX_STRING_SIZE, ret);
        av_log(NULL, AV_LOG_ERROR, "av_read_frame error %s", errstr);
    }
    return ret;
}

int MediaDecoder::getFrame(AVPacket *pkt, AVFrame *frame) {
    int ret = 0;
    int got_frame = 0;
    if(pkt->stream_index == videoIndex) {
        ret = avcodec_decode_video2(inputFormatContext->streams[pkt->stream_index]->codec, frame, &got_frame, pkt);
        av_log(NULL, AV_LOG_DEBUG, "get video frame ret %d got_frame %d\n", ret, got_frame);
        if(got_frame <= 0) {
            return -1;
        }
        return ret;
    } else if(pkt->stream_index == audioIndex) {
        ret = avcodec_decode_audio4(inputFormatContext->streams[pkt->stream_index]->codec, frame,&got_frame, pkt);
        if(got_frame <= 0) {
            return -1;
        }
        return ret;
        av_log(NULL, AV_LOG_DEBUG, "get audio frame ret %d got_frame %d\n", ret, got_frame);
    }
    return -1;
}

void MediaDecoder::initVideoConvert() {
    if(swsVideoCtx) {
        sws_freeContext(swsVideoCtx);
    }
    av_log(NULL, AV_LOG_ERROR, "videoWidth %d videoHeight %d displayWidth %d displayHeight %d\n", videoWidth, videoHeight, displayWidth, displayHeight);
    swsVideoCtx = sws_getContext(videoWidth, videoHeight, videoPixFmt, displayWidth, displayHeight, outPixFmt, SWS_BILINEAR, NULL, NULL, NULL);
    return 1;
}

void MediaDecoder::initAudioConvert() {
    if(swrAudioCtx) {
        swr_free(&swrAudioCtx);
    }
    swrAudioCtx = swr_alloc();
    av_opt_set_int(swrAudioCtx, "in_channel_layout", audioLayout, 0);
    av_opt_set_int(swrAudioCtx, "in_sample_rate", audioSampleRate, 0);
    av_opt_set_sample_fmt(swrAudioCtx, "in_sample_fmt", audioSampleFormat, 0);
    av_opt_set_int(swrAudioCtx, "out_channel_layout", outLayout, 0);
    av_opt_set_int(swrAudioCtx, "out_sample_rate", outSampleRate, 0);
    av_opt_set_sample_fmt(swrAudioCtx, "out_sample_fmt", outSampleFormat, 0);
    swr_init(swrAudioCtx);
}

AVFrame* MediaDecoder::convertVideoFrame(AVFrame *src) {
    AVFrame *frame = av_frame_alloc();
    int frameSize = av_image_get_buffer_size(outPixFmt, displayWidth, displayHeight, 1);
    printf("frame size is %d\n", frameSize);
    unsigned char* frameData = (unsigned char*)av_malloc(frameSize);
    av_image_fill_arrays(frame->data, frame->linesize, frameData, outPixFmt, displayWidth, displayHeight, 1);
    sws_scale(swsVideoCtx, src->data, src->linesize, 0, videoHeight, frame->data, frame->linesize);
    return frame;
}

AVFrame* MediaDecoder::convertAudioFrame(AVFrame *src) {
    AVFrame *frame = av_frame_alloc();
    return frame;
}

int main(int argc, char **argv) {
    int audioChannels = 2;
    MediaDecoder decoder;
    decoder.setDataSource(argv[1]);
    decoder.prepare();
    decoder.setOutVideoPixFmt(AV_PIX_FMT_YUV420P);
    decoder.setDisPlayWidth(decoder.getVideoWidth());
    decoder.setDisPlayHeight(decoder.getVideoHeight());
    decoder.setOutAudioChannels(audioChannels);
    decoder.setOutAudioLayout(av_get_default_channel_layout(audioChannels));
    decoder.setOutAudioFromat(AUDIO_S16SYS);
    decoder.setOutAudioSampleRate(decoder.getSampleRate());
    decoder.initVideoConvert();
    decoder.initAudioConvert();

    AVPacket *pkt = (AVPacket *)av_malloc(sizeof(AVPacket));
    AVFrame *frame = av_frame_alloc();

    SDL sdl(0);
    SDL_Event event;
    //SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

    int64_t lastVideoFrameTime = -1;
    int64_t lastVideoFramePts = -1;

    sdl.setVideoWidth(decoder.getVideoWidth());
    sdl.setVideoHeight(decoder.getVideoHeight());
    sdl.setVideoPixFormat(SDL_PIXELFORMAT_IYUV);
    sdl.createWindow();
    sdl.initRect();
    sdl.createTextrue();
    sdl.showWindow();

    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    while(decoder.getPacket(pkt) >= 0) {
        SDL_WaitEvent(&event);
        if(event.type==SDL_QUIT){
            break;
        }
        if(pkt->stream_index == decoder.getVideoIndex()) {
            if(decoder.getFrame(pkt, frame) > 0) {
                AVFrame* outFrame = decoder.convertVideoFrame(frame);
                av_log(NULL, AV_LOG_DEBUG, "pkt pts %lld\n", pkt->pts);
                sdl.setBuffer(outFrame->data[0], outFrame->linesize[0]);
                //sdl.setBuffer(frame->data[0], frame->linesize[0]);
                if(lastVideoFrameTime == -1) {
                    sdl.showFrame(0);
                    lastVideoFrameTime = decoder.getCurMs();
                    lastVideoFramePts = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                } else {
                    int64_t curMsTime = decoder.getCurMs();
                    int64_t curPtsTime = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                    int sleepTime = (curMsTime - lastVideoFrameTime) - (curPtsTime - lastVideoFramePts);
                    if(sleepTime >= 0) {
                        av_log(NULL, AV_LOG_ERROR, "sleepTime %d\n", sleepTime);
                        sdl.showFrame(sleepTime);
                    } else {
                        sdl.showFrame(-sleepTime);
                        av_log(NULL, AV_LOG_ERROR, "sleepTime %d\n", -sleepTime);
                    }
                    lastVideoFrameTime = decoder.getCurMs();
                    lastVideoFramePts = decoder.getMsByPts(decoder.getVideoTimeBase(), pkt->pts);
                }
                printf("%d %d %d\n", outFrame->linesize[0], outFrame->linesize[1], outFrame->linesize[2]);
                av_frame_free(&outFrame);
            }
        } else if(pkt->stream_index == decoder.getAudioIndex()) {
            if(decoder.getFrame(pkt, frame) > 0) {
            }
        }
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        av_free_packet(pkt);
    }
    av_free_packet(pkt);
    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    SDL_Quit();
    return 0;
}
