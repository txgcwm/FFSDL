#ifndef __MEDIAFRAME__H__
#define __MEDIAFRAME__H__

class MediaFrame {
    private:
        AVFrame *frame;
        int64_t pts;
        int64_t dts;
    public:
        MediaFrame(AVFrame *frame);
        void setPts(int64_t pts);
        void setDts(int64_t dts);
        AVFrame* getFrame();
};


#endif
