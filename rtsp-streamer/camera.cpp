
#include "camera.h"


#define CLEAR(x) memset(&(x), 0, sizeof(x))

static void ffmpeg_encoder_init_frame(AVFrame **framep, int width, int height,int pix_fmt)
{
    int ret = 0;
    AVFrame *frame = nullptr;
    frame = av_frame_alloc();
    if (!frame) {
        fprintf(stderr, "Could not allocate video frame\n");
        exit(1);
    }
    frame->format = pix_fmt;
    frame->width  = width;
    frame->height = height;
    ret = av_image_alloc(frame->data, frame->linesize, frame->width, frame->height, (AVPixelFormat)frame->format, 32);
    if (ret < 0) {
        fprintf(stderr, "Could not allocate raw picture buffer\n");
        exit(1);
    }
    *framep = frame;
}


int Camera::xioctl(int fh, int request, void *arg)
{
int r = 0;
	do {
		r = v4l2_ioctl(fh, request, arg);
    } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if (r == -1) {
        printf("error %d, %s\n", errno, strerror(errno));
        return -1;
    }
    return 0;
}



int Camera::init()
{
struct v4l2_format fmt;
struct v4l2_requestbuffers req;
enum v4l2_buf_type type;
int r = 0;
unsigned int i = 0;

        fd = v4l2_open(dev_name, O_RDWR | O_NONBLOCK, 0);
        if(fd < 0) {
        	perror("Cannot open device");
            return -1;
        }

        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = width;
        fmt.fmt.pix.height      = height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.field       = V4L2_FIELD_NONE;

        xioctl(fd, VIDIOC_S_FMT, &fmt);

        if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
        	printf("Libv4l didn't accept format. Can't proceed.\n");
            return -1;
        }

        if (fmt.fmt.pix.width != width || fmt.fmt.pix.height != height) {
        	printf("Warning: driver is sending image at %dx%d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);
        	return -1;
        }

        CLEAR(req);
        req.count = 2;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;

        xioctl(fd, VIDIOC_REQBUFS, &req);

        buffers = (struct buffer *)calloc(req.count, sizeof(*buffers));
        if(!buffers) {
        	fprintf(stderr,"out of memory\n");
        	return -1;
        }

        struct v4l2_buffer buf;
        for(n_buffers=0; n_buffers < req.count; n_buffers++) {
        	CLEAR(buf);

            buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index  = n_buffers;

            xioctl(fd, VIDIOC_QUERYBUF, &buf);

            buffers[n_buffers].length = buf.length;
            buffers[n_buffers].start = v4l2_mmap(NULL,
            								buf.length,
											PROT_READ | PROT_WRITE,
											MAP_SHARED,
											fd,
											buf.m.offset);

            if (MAP_FAILED == buffers[n_buffers].start) {
            	perror("mmap");
                return -1;
            }
        }

        for(i=0; i < n_buffers; i++) {
        	CLEAR(buf);
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;
            xioctl(fd, VIDIOC_QBUF, &buf);
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl(fd, VIDIOC_STREAMON, &type);
}


int Camera::convert(uint8_t *ibuf,int ilen, uint8_t *obuf,int olen)
{
	int len = av_image_get_buffer_size(AV_PIX_FMT_YUYV422,frame->width,frame->height,32);
	if(len != ilen) {
		fprintf(stderr,"wrong input length = %d vs %d\n",len,ilen);
		return -1;
	}
	int ret = av_image_fill_arrays(frame->data, frame->linesize,
	                             ibuf,
								 AV_PIX_FMT_YUYV422, frame->width, frame->height, 32);
	if(ret < 0) {
	 	fprintf(stderr, "av_image_fill_arrays failed\n");
	 	return -1;
	}

	sws_context = sws_getCachedContext(sws_context,
		            frame->width, frame->height, AV_PIX_FMT_YUYV422,
		            frame2->width, frame2->height, AV_PIX_FMT_YUV420P,
		            0, NULL, NULL, NULL);

	sws_scale(sws_context,frame->data,frame->linesize,0,frame2->height,frame2->data,frame2->linesize);

	len = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,frame2->width,frame2->height,32);
	if(len != olen) {
		fprintf(stderr,"wrong output length = %d vs %d\n",len,olen);
		return -1;
	}
	ret = av_image_copy_to_buffer(obuf,
								olen,
			                    frame2->data,
								frame2->linesize,
								AV_PIX_FMT_YUV420P,
								frame2->width,
								frame2->height,
								32);

	if (ret < 0) {
	  	fprintf(stderr,"av_image_copy_to_buffer failed\n");
	  	return -1;
	}
	return 0;
}


int Camera::get(uint8_t *buffer,int len)
{
	int r = 0;
	struct v4l2_buffer buf;
	do {
		FD_ZERO(&fds);
        FD_SET(fd, &fds);

        tv.tv_sec = 2;
        tv.tv_usec = 0;
        r = select(fd + 1, &fds, NULL, NULL, &tv);
    } while ((r == -1 && (errno = EINTR)));
    if (r == -1) {
    	perror("select");
        return -1;
    }

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    xioctl(fd, VIDIOC_DQBUF, &buf);

   	convert((uint8_t*)buffers[buf.index].start, buf.bytesused, buffer, len);

    xioctl(fd, VIDIOC_QBUF, &buf);
}


void Camera::release()
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    xioctl(fd, VIDIOC_STREAMOFF, &type);
    for(int i=0; i < n_buffers; i++) {
    	v4l2_munmap(buffers[i].start, buffers[i].length);
    }
    v4l2_close(fd);
    av_frame_free(&frame);
    av_frame_free(&frame2);
}



Camera::Camera(int width,int height,const char *dev_name) : width(width), height(height), fd(-1),
															buffers(nullptr), dev_name(dev_name), frame(nullptr), frame2(nullptr)
{
	if(init() != 0) {
		fprintf(stderr,"camera init failed\n");
		exit(1);
	}
	ffmpeg_encoder_init_frame(&frame,width,height,AV_PIX_FMT_YUYV422);
	ffmpeg_encoder_init_frame(&frame2,width,height,AV_PIX_FMT_YUV420P);
}


Camera::~Camera()
{
	release();
}

