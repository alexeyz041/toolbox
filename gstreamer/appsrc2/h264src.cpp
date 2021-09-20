
#include <unistd.h>
#include <vector>
#include <iterator>
#include <iostream>
#include <sstream>
#include <fstream>

#include "h264src.h"

std::vector<Frame> frames;
std::vector<uint64_t> timestamps;

uint64_t frame_num; // 0 .. frames.size()-1
uint64_t time_base;
uint64_t frame_num_base;


ssize_t readFile(const char* filename, std::vector<uint8_t> &data)
{
    std::ifstream file(filename, std::ios::binary);
    file.unsetf(std::ios::skipws);
    std::streampos fileSize;
    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
//    printf("file size = %ld\n", (long)fileSize);
    if(fileSize <= 0)
        return fileSize;
    file.seekg(0, std::ios::beg);

    data.reserve(fileSize);
    data.insert(data.begin(),
               std::istream_iterator<uint8_t>(file),
               std::istream_iterator<uint8_t>());

    return fileSize;
}


bool is_vcl(int tp)
{
    return (tp >= 1 && tp <= 5);
}

bool load_sample(const char *folder)
{
char fnm[1024] = {0};
int last_nal_type = 0;
int index = 0;
    for( ; ; ) {
        snprintf(fnm, sizeof(fnm), "%s/sample.h264.%04d", folder, index++);
        printf("loading %s\r", fnm);
        std::vector<uint8_t> data;
        ssize_t sz = readFile(fnm, data);
        if(sz <= 0) return sz;

        int nal_type = data[4] & 0x1f;
        if(nal_type == 1) {
            size_t i = frames.size();
            frames.push_back(Frame());
            frames[i].data.reserve(data.size());
            frames[i].data.insert(frames[i].data.begin(), data.begin(), data.end());
        } else {
            size_t i = frames.size();
            if(frames.empty() || last_nal_type == 1) {
                frames.push_back(Frame());
            } else {
                i--;
            }
            //printf("%ld append %d  %ld -> %ld\n", i, nal_type, frames[i].data.size(), data.size());
            frames[i].data.reserve(frames[i].data.size() + data.size());
            frames[i].data.insert(frames[i].data.end(), data.begin(), data.end());
        }
        last_nal_type = nal_type;
    }
}


void load_timestamps(const char *folder)
{
char fnm[1024] = {0};
    snprintf(fnm, sizeof(fnm), "%s/timestamps", folder);
    std::fstream in(fnm);
    std::string line;

    while (std::getline(in, line)) {
        float value;
        std::stringstream ss(line);
        if(ss >> value) {
            timestamps.push_back((uint64_t)(value*1000000));
        }
    }
    printf("got %ld timestamps\n", timestamps.size());
}



void load_samples(const char *folder)
{
    load_timestamps(folder);
    load_sample(folder);

    printf("\nloaded %ld frames\n", frames.size());
    if(frames.size() == 0) {
        exit(1);
    }

    int cnt = 0;
    for( ; frames.size() > timestamps.size(); cnt++) {
        frames.pop_back();
    }
    printf("dropped %d frames without timestamps\n", cnt);

    // set timestamps and durations
    for(size_t i=0; i < frames.size(); i++) {
        frames[i].rel_timestamp = (i < timestamps.size()) ? timestamps[i] : *timestamps.end();
    }
    for(size_t i=0; i < frames.size(); i++) {
        if(i != frames.size() - 1) {
            frames[i].duration = frames[i+1].rel_timestamp - frames[i].rel_timestamp;
        } else {
            frames[i].duration = frames[i-1].duration; //hack
        }
        if(frames[i].duration < 0) {
            frames[i].duration = 0;
            int nal_type = frames[i].data[4] & 0x1f;
            printf("nal type %d frame duration = %ld size %ld\n", nal_type, frames[i].duration, frames[i].data.size());
        }
    }
    time_base = 0;
    frame_num = 0;
}


Frame *get_next_frame(void)
{
    Frame *frame = &frames[frame_num];
    frame->timestamp = frame->rel_timestamp + time_base;
    frame->frame_num = frame_num + frame_num_base;

    if(++frame_num >= frames.size()) {
        frame_num = 0;
        frame_num_base += frames.size();
        time_base += frames[frames.size()-1].rel_timestamp;
        time_base += frames[frames.size()-1].duration;
    }
    return frame;
}

