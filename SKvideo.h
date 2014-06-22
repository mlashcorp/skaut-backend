/* 
 * File:   video-tools.h
 * Author: cortereal
 *
 * Created on July 28, 2013, 11:48 AM
 */

#ifndef VIDEO_TOOLS_H
#define	VIDEO_TOOLS_H
#include "opencv2/highgui/highgui.hpp"
#include <queue>
//These define the pre and after alarm event buffer sizes (4.5 seconds each at 30FPS)
#define VIDEO_BUFFER_SIZE 125
#define VIDEO_FORWARD_BUFFER_SIZE 125

class SKvideo {
public:
    SKvideo();
    SKvideo(const SKvideo& orig);
    virtual ~SKvideo();
    int start_video_feed(std::string arg);
private:
    long process_frame(cv::Mat &frame);
    void detect_faces(cv::Mat &frame);
    int video_loop();
    int process_alarm_event(long alarm_metric,std::queue<cv::Mat> buffer);
    void alarm_state_update(long alarm_metric);
    int encode_video(std::queue<cv::Mat> buffer);
};


#endif	/* VIDEO_TOOLS_H */

