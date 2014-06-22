/* 
 * File:   SKio.h
 * Author: cortereal
 *
 * Created on December 15, 2011, 11:59 AM
 */

#ifndef HSIO_H
#define	HSIO_H

#include "opencv2/opencv.hpp"


using namespace std;

class SKio {
public:
    
    static void debug(const char *fmt, ...);
    static void handle_critical_error();
    static int save_frame(const char* fname,cv::Mat frame);
    static void send_email_alarm(string destination_email,string title,string msg);
    static void reencode_alarm_video(int quality,int framerate);
private:
    SKio();
    SKio(const SKio& orig);
    virtual ~SKio();
};


#endif	/* HSIO_H */

