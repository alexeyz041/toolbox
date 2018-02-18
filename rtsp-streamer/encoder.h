
#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <stdio.h>
#include <x264.h>


class Encoder {
public:
    Encoder(int w,int h,int fps);
    virtual ~Encoder();

    int encode(uint8_t *buf,int size,uint64_t ts);
    virtual void write(uint8_t *buf,int len,uint64_t ts) = 0;

private:
    int init(void);
    void release(void);

private:
    int width;
    int height;
    int fps;
    x264_param_t param;
    x264_picture_t pic;
    x264_picture_t pic_out;
    x264_t *h;
    x264_nal_t *nal;
    int i_frame;
    int i_nal;
};



#endif //ENCODER_H
