#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sqlite3.h>

#include "rosbag.hpp"
#include "resolution.hpp"


RosBagReader::RosBagReader()
{
}

RosBagReader::~RosBagReader()
{
}

inline void save_file(std::string fnm, std::vector<unsigned char> &d)
{
    std::ofstream outfile;
    outfile.open(fnm, std::ios::out | std::ios::binary); 
    outfile.write((const char *)&d[0], d.size());
}


std::string get_temp_file(const char *pat)
{
    return "/tmp/" + std::string(pat);
}


bool RosBagReader::getVideoParams(uint32_t *w, uint32_t *h, uint64_t *fr_n, uint64_t *fr_d)
{
    uint64_t delta = 0;
    for(int i=0; i < 10; i++) {
        delta += m_Frames[i].duration;
    }
    delta /= 10;

    *fr_n = 1000000000 / delta;
    *fr_d = 1000;

//    printf("delta = %ld fr %ld/%ld\n", delta, *fr_n, *fr_d);

    std::string fnm = get_temp_file("1.jpg");
    printf("tmp file name = %s\n", fnm.c_str());
    save_file(fnm, m_Frames[0].data);
    int w1 = 0;
    int h1 = 0;
    int fr_n1 = 0;
    int fr_d1 = 0;
    if(get_resolution(fnm.c_str(), &w1, &h1, &fr_n1, &fr_d1))
        return false;
    *w = w1;
    *h = h1;
    remove(fnm.c_str());

//    printf("%dx%d\n", *w, *h);
    return true;
}


bool RosBagReader::get_topic_id(int ix)
{
    const char *name = "/image";
    const char *type = "msgs/msg/compressed";
    sqlite3 *db = nullptr;
    sqlite3_stmt *statement = nullptr;
    int result = 0;

    std::string fnm = get_file_name(ix);
    if(sqlite3_open(fnm.c_str(), &db) != SQLITE_OK) {
        printf("Open database %s failed\n", fnm.c_str());
        return false;
    }

    char sql[256] = {0};
    snprintf(sql, sizeof(sql), "SELECT id, name, type FROM topics WHERE name = \'%s\' AND type = \'%s\'", name, type);
    if(sqlite3_prepare_v2(db, sql, strlen(sql), &statement, 0) != SQLITE_OK) {
        printf("Prepare statement failed %s\n", sql);
        sqlite3_close(db);
        return false;
    }

    result = sqlite3_step(statement);
    if(result == SQLITE_ROW) {
        m_TopicId = sqlite3_column_int(statement, 0);
//        printf("topic id = %d\n", m_TopicId);
    } else {
        printf("topic not found\n");
    }

    sqlite3_finalize(statement);
    sqlite3_close(db);
    return result == SQLITE_ROW;
}


std::string RosBagReader::get_file_name(int ix)
{
    std::size_t found = m_FolderPath.find_last_of("/\\");
    std::string folderName = m_FolderPath;
    if(found) {
        folderName = m_FolderPath.substr(found+1);
    }
    char ixs[20] = {0};
    snprintf(ixs, sizeof(ixs), "_%d.db3", ix);
    return m_FolderPath + "/" + folderName + ixs;
}


inline bool file_exists(const char *fnm)
{
    FILE *f = fopen(fnm, "r");
    if(f) {
        fclose(f);
        return true;
    }
    return false;
}

void RosBagReader::get_max_ix()
{
    m_MaxIndex = 1;
    for(;;) {
        std::string name = get_file_name(m_MaxIndex);
        if(!file_exists(name.c_str())) break;
        m_MaxIndex++;
    }
//    printf("max index = %d\n", m_MaxIndex);
}


bool RosBagReader::setFolder(const char *path)
{
    m_FolderPath = path;
    if(m_FolderPath.back() == '/') m_FolderPath.pop_back();
    if(m_FolderPath.back() == '\\') m_FolderPath.pop_back();

    get_max_ix();
    if(!read_frames()) return false;
    prepare();
    return true;
}


#define IMAGE_OFFSET    0x24

bool RosBagReader::save_frame(uint8_t *d, uint32_t size, uint64_t timestamp)
{
    size_t start = 0;
    size_t len = size;

    if(size < IMAGE_OFFSET+2) {
        printf("size too small %d\n", size);
        return false;
    }

    if(d[IMAGE_OFFSET] == 0xff && d[IMAGE_OFFSET+1] == 0xd8) { // JPEG header
        start = IMAGE_OFFSET;
        len = (d[IMAGE_OFFSET-1] << 24) + (d[IMAGE_OFFSET-2] << 16) + (d[IMAGE_OFFSET-3] << 8) + d[IMAGE_OFFSET-4];
        if(start + len > size) {
            printf("invalid length %ld\n", len);
            return false;
        }
    }

//    printf("image %ld\n", m_Frames.size());

    m_Frames.push_back(Frame());
    Frame *f = &m_Frames[m_Frames.size()-1];
    f->data.resize(len);
    memcpy(f->data.data(), &d[start], len);
    f->rel_timestamp = timestamp;
    return true;
}


bool RosBagReader::read_frames()
{
    for(int ix=0; ix < m_MaxIndex; ix++) {
        if(!get_topic_id(ix)) return false;
        if(!read_frames_from(ix)) return false;
    }
    return true;
}


bool RosBagReader::read_frames_from(int ix)
{
sqlite3 *db = nullptr;
sqlite3_stmt *statement = nullptr;
int result = 0;

    std::string fnm = get_file_name(ix);
    if(sqlite3_open(fnm.c_str(), &db) != SQLITE_OK) {
        printf("Open database %s failed\n", fnm.c_str());
        return false;
    }

    char sql[256] = {0};
    snprintf(sql, sizeof(sql), "SELECT timestamp, data FROM messages WHERE topic_id = %d order by timestamp", m_TopicId);
    if(sqlite3_prepare_v2(db, sql, strlen(sql), &statement, 0) != SQLITE_OK) {
        printf("Prepare statement failed %s\n", sql);
        sqlite3_close(db);
        return false;
    }

    for(int i=0; ; i++) {
        result = sqlite3_step(statement);
        if(result == SQLITE_ROW) {
            uint64_t timestamp = sqlite3_column_int64(statement, 0);
            int size = sqlite3_column_bytes(statement, 1);
            save_frame((uint8_t *)sqlite3_column_blob(statement, 1), size, timestamp);
        } else {
            break;
        }
    }
    sqlite3_finalize(statement);
    sqlite3_close(db);
    return true;
}


void RosBagReader::prepare()
{
    // adjust timestamps to start with 0
    uint64_t b = m_Frames[0].rel_timestamp;
    for(int i=0; i < m_Frames.size(); i++) {
        ssize_t t = m_Frames[i].rel_timestamp - b;
        m_Frames[i].rel_timestamp = ((t > 0) ? t : 0) / 1000;
    }

    // calculate durations
    for(int i=0; i < m_Frames.size()-1; i++) {
        ssize_t d = m_Frames[i+1].rel_timestamp - m_Frames[i].rel_timestamp;
        m_Frames[i].duration = (d > 0) ? d : 0;
    }
    m_Frames[m_Frames.size()-1].duration = m_Frames[m_Frames.size()-2].duration;
}


Frame *RosBagReader::get_next_frame(void)
{
    Frame *frame = &m_Frames[m_FrameNum];
    frame->timestamp = frame->rel_timestamp + m_TimeBase;
    frame->frame_num = m_FrameNum + m_FrameNumBase;

    if(++m_FrameNum >= m_Frames.size()) { // loop video
        m_FrameNum = 0;
        m_FrameNumBase += m_Frames.size();
        m_TimeBase += m_Frames[m_Frames.size()-1].rel_timestamp;
        m_TimeBase += m_Frames[m_Frames.size()-1].duration;
    }

//  printf("frame %ld ts %ld duration %ld\n", frame->frame_num, frame->timestamp, frame->duration);

    return frame;
}

