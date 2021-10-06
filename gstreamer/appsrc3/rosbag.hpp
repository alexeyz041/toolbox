#ifndef ROSBAG_HPP_INCLUDED
#define ROSBAG_HPP_INCLUDED

#include <vector>

struct Frame {
    std::vector<uint8_t> data;    // frame data
    int64_t duration;             // frame duration (in microseconds)
    uint64_t rel_timestamp;       // frame timestamp relative to video start (in microseconds)
    uint64_t timestamp;           // frame timestamp from streaming session start (in microseconds)
    uint64_t frame_num;           // frame number in streaming session (stream is looped, video sequence will be repeated again and again)
};

class RosBagReader {
public:
	RosBagReader();
	~RosBagReader();

	bool setFolder(const char *path);
        bool getVideoParams(uint32_t *w, uint32_t *h, uint64_t *fr_n, uint64_t *fr_d);
	Frame *get_next_frame(void);

private:
	std::string get_file_name(int ix);
	void get_max_ix(void);
	bool save_frame(uint8_t *d, uint32_t size, uint64_t timestamp);
	bool get_topic_id(int ix);
	void prepare(void);
	bool read_frames(void);
	bool read_frames_from(int index);

private:
	uint64_t m_TimeBase = 0;
	uint64_t m_FrameNumBase = 0;
	uint64_t m_FrameNum = 0;
	std::string m_FolderPath;
	int m_TopicId;
        int m_MaxIndex;
	std::vector<Frame> m_Frames;
};


#endif //ROSBAG_HPP_INCLUDED

