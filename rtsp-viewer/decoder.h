
#ifndef DECODER_H
#define DECODER_H

#include <stdint.h>
#include <stdio.h>

extern "C" {
	#include <libavformat/avformat.h>
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}

class Decoder {
public:
	Decoder(int width,int height);
	virtual ~Decoder();

	int decode(uint8_t* buf,int len);
	virtual void write(uint8_t* buf,int len) = 0;

private:
	void write_frame(void);

private:
	AVCodec *codec;
	AVCodecContext *c;
    AVFrame *frame;
    AVFrame *frame2;
    AVPacket avpkt;
    int width;
    int height;
    struct SwsContext *sws_context;
    uint8_t *buffer;
};


#endif //DECODER_H

