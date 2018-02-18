
#ifndef ENCODER2_H
#define ENCODER2_H

#include <stdint.h>
#include <stdio.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}

class Encoder2 {
public:
    Encoder2(int w,int h,int fps);
    virtual ~Encoder2();

    int encode(uint8_t *buf,int size,uint64_t ts);
    virtual void write(uint8_t *buf,int len,uint64_t ts) = 0;

private:
    int init(void);
    void release(void);

private:
    int width;
    int height;
    int fps;
    uint64_t frame_time;

    AVCodecContext *c;
    AVFrame *frame;
    AVPacket pkt;
};



#endif //ENCODER2_H
