
#ifndef H264SRC_H_INCLUDED
#define H264SRC_H_INCLUDED

#include <vector>

struct Frame {
    std::vector<uint8_t> data;
    int64_t duration;
    uint64_t rel_timestamp;
    uint64_t timestamp;
    uint64_t frame_num;
};

void load_samples(const char *folder);

Frame *get_next_frame(void);

# endif // H264SRC_H_INCLUDED
