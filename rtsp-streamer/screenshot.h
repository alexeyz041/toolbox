
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <stdint.h>

#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>


#define BPP   4

struct shmimage {
    XShmSegmentInfo shminfo ;
    XImage * ximage ;
    unsigned int * data ; // will point to the image's BGRA packed pixels
};


class ScreenShot {
public:
	ScreenShot();
	~ScreenShot();

	void get(int *width,int *height,uint32_t **buf);

private:
	int init(void);
	void release(void);

private:
	Display * dsp;
	struct shmimage image;
};

#endif //SCREENSHOT_H


