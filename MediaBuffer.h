#ifndef __MEDIABUFFER__H__
#define __MEDIABUFFER__H__

#include "Queue.h"
#include "MediaDecoder.h"

class MediaBuffer {
    private:
        Queue<AVFrame*> videoBuffer;
        Queue<AVFrame*> audioBuffer;
    public:
        MediaBuffer();
        bool enQueueVideoFrame(AVFrame* frame);
        bool enQueueAudioFrame(AVFrame* frame);
        bool deQueueVideoFrame(AVFrame* &frame);
        bool deQueueAudioFrame(AVFrame* &frame);
        int getVideoFrameCount();
        int getAudioFrameCount();
};

#endif
