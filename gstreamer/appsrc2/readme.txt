Preparing samples:

1. Extract H264 from mp4
 ffmpeg -i 1.mp4 -vcodec copy -an -bsf:v h264_mp4toannexb 1.h264

2. Extract timestamps
 ffprobe -f lavfi -i "movie=1.mp4,fps=fps=12.5[out0]" -show_frames -show_streams -show_entries frame=pkt_pts_time -of csv=p=0 2>&1 > timestamps

3. Use this to split h264 into NAL's
 https://github.com/ckevar/h264-splitter.git
