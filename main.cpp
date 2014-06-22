/* 
 * File:   main.cpp
 * Author: cortereal
 *
 * Created on July 28, 2013, 11:10 AM
 */

#include <cstdlib>
#include <iostream>
#include "SKvideo.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    if (argc != 2) {
        cout << "\nThis program justs gets you started reading images from video\n"
                "Usage:\n./" << argv[0] << " <video device number>\n"
                << "q,Q,esc -- quit\n"
                << "space   -- save frame\n\n"
                << "\tThis is a starter sample, to get you up and going in a copy pasta fashion\n"
                << "\tThe program captures frames from a camera connected to your computer.\n"
                << "\tTo find the video device number, try ls /dev/video* \n"
                << "\tYou may also pass a video file, like my_vide.avi instead of a device number"
                << endl;
        return 1;
    }


    std::string arg = argv[1];
    SKvideo feed;
    feed.start_video_feed(arg);

    return 0;
}

