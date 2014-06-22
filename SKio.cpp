
#include "SKio.h"
#include "definitions.h"
#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

using namespace std;

/**
 * \brief Debug verbose
 *
 * This helper function sends debug vervose to syslog
 *
 */

void SKio::debug(const char *fmt, ...) {
    if (DEBUG_VERBOSE_FLAG) {
        va_list va;
        va_start(va, fmt);
        char buffer[MAX_DEBUG_SIZE];
        vsprintf(buffer, fmt, va);
        va_end(va);
        syslog(LOG_DEBUG, "%s", buffer);
        fflush(stdout);
    }
}

/**
 * \brief Handles protocol errors
 *
 * This function is invoked upon unsucessfull protocol execution
 *
 */
void SKio::handle_critical_error() {
    exit(1);
}

/**
 * @brief Saves a Mat to disk as an image
 *
 * @param fname filename
 * @param frame image to be saved
 * @return SUCESS or ERROR
 */
int SKio::save_frame(const char* fname, cv::Mat frame) {
    if (!frame.empty())
        cv::imwrite(fname, frame);
    return SUCCESS;
}

/**
 * 
 * @param destination_email
 * @param title
 * @param msg
 */
void SKio::send_email_alarm(string destination_email,string title,string msg){
    char cmd[200] = {0};
    sprintf(cmd, "echo | mutt %s -a alarm.m4v -s \"%s\" ", destination_email.c_str(),title.c_str());
    system(cmd);
}

/**
 * 
 * @param quality
 * @param framerate
 */
void SKio::reencode_alarm_video(int quality,int framerate){
    char cmd[200] = {0};
    sprintf(cmd, "HandBrakeCLI -i alarm.avi -o alarm.m4v -e x264 -q %d -r %d", quality,framerate);
    system(cmd);
}
SKio::SKio() {
}

SKio::SKio(const SKio& orig) {
}

SKio::~SKio() {
}





