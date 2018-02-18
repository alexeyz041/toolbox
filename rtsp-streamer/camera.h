#ifndef CAMERA_H
#define CAMERA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>

#include <linux/videodev2.h>
#include <libv4l2.h>

extern "C" {
    #include <libavcodec/avcodec.h>
    #include <libavutil/imgutils.h>
    #include <libavutil/opt.h>
    #include <libswscale/swscale.h>
}

struct buffer {
	void   *start;
    size_t length;
};


class Camera {
public:
	Camera(int width,int height,const char *device);
	~Camera();

	int get(uint8_t *buffer,int len);

private:
	int xioctl(int fh, int request, void *arg);
	int init(void);
	void release(void);
	int convert(uint8_t *ibuf,int ilen, uint8_t *obuf,int olen);

private:
	int width;
	int height;
	struct buffer *buffers;
	int fd;
	const char *dev_name;
	fd_set fds;
	struct timeval tv;
	unsigned int n_buffers;
	AVFrame *frame;
	AVFrame *frame2;
	struct SwsContext *sws_context;
};

#endif //CAMERA_H


