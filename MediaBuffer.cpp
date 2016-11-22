#include "MediaBuffer.h"

bool MediaBuffer::enQueueVideoFrame(AVFrame* frame) {
    QueueNode<AVFrame*> *node = new QueueNode<AVFrame*>(frame);
    videoBuffer.push(node);
    return true;
}

bool MediaBuffer::enQueueAudioFrame(AVFrame *frame) {
    QueueNode<AVFrame*> *node = new QueueNode<AVFrame*>(frame);
    audioBuffer.push(node);
    return true;
}

bool MediaBuffer::deQueueVideoFrame(AVFrame* &frame) {
    QueueNode<AVFrame*> *node = NULL;
    videoBuffer.dequeue(node);
    return node->getVal();
}

bool MediaBuffer::deQueueAudioFrame(AVFrame* &frame) {
    QueueNode<AVFrame*> *node = NULL;
    audioBuffer.dequeue(node);
    return node->getVal();
}

int MediaBuffer::getAudioFrameCount() {
    return audioBuffer.getSize();
}

int MediaBuffer::getVideoFrameCount() {
    return videoBuffer.getSize();
}
