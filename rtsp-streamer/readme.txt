
rtsp-streamer uses
    * live555 from http://www.live555.com/liveMedia/
    * ffmpeg libraries from http://ffmpeg.org/
    * libv4l2 from https://linuxtv.org/ for camera access
    * libx264 (separate or thru ffmpeg)


Building ffmpeg from sources:
    git clone https://git.ffmpeg.org/ffmpeg.git ffmpeg
    ./configure --disable-cuda --enable-libx264 --enable-gpl


Hint: list camera's supported modes
    v4l2-ctl --list-formats-ext

    