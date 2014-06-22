#include "SKvideo.h"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <iostream>
#include <vector>
#include <stdio.h>
#include "definitions.h"
#include "SKio.h"
#include <sys/time.h>
#include <queue>


using namespace std;

/**----------Global variables----------*/

//Global flag to enable and disable video processing
bool capture_flag = false;
cv::VideoCapture capture;
//Defined as global to reduce computational overhead
cv::Mat dilate_mask = cv::getStructuringElement(cv::MORPH_ELLIPSE,
        cv::Size(7, 7),
        cv::Point(2, 2));
//Useful for performance benchmarking 
struct timeval begin, end, now, last_known_alarm;
int alarm_event_count = 0;
queue<cv::Mat> video_buffer;
queue<cv::Mat> forward_video_buffer;
//Face detection template
cv::CascadeClassifier face_cascade;
/**------End of global variables------*/

/**
 * This function launches an infinite loop that:
 * - captures data from the camera
 * - processes the frame
 * - outputs the frame to the screen
 * 
 * This is an interactive GUI, and as such the user can
 * quit this loop by pressing esc or q(Q). However, a
 * global flag was also used to enable this code to be disabled
 * programmatically by another thread.
 * 
 * This is a blocking function.
 * 
 * @param id camera id (usually between 0 and n)
 * @return status from running the capture loop
 */
int SKvideo::start_video_feed(std::string id) {
    face_cascade.load("haarcascade_frontalface_alt.xml");
    //cv highgui facility to simplify camera access, thank you very much

    capture.open(atoi(id.c_str()));

    //We set the last known alarm to the beggining to inhibit triggering for the
    //first ALARM_INHIBIT_TIMER seconds
    gettimeofday(&last_known_alarm, NULL);

    if (!capture.isOpened()) {
        cerr << "Failed to open a video device or video file!\n" << endl;
        return 1;
    }
    capture_flag = true;
    return this->video_loop();
}

/**
 * 
 * @param capture
 * @return 
 */
int SKvideo::video_loop() {
    int n = 0;
    char filename[200];
    long alarm_metric;

    string window_name = "Skaut testbed | q or esc to quit";
    cout << "press space to save a picture. q or esc to quit" << endl;
    cv::namedWindow(window_name, CV_WINDOW_KEEPRATIO); //resizable window;
    cv::Mat frame;
    while (capture_flag) {
        //Get frame from camera
        capture >> frame;
        //cv::cvtColor(frame, frame, CV_BGR2GRAY);

        //quick workaround to make queue fixed sized
        video_buffer.push(frame.clone());
        if (video_buffer.size() == VIDEO_BUFFER_SIZE) video_buffer.pop();
        //calculate motion and update alarm state
        this->alarm_state_update(this->process_frame(frame));

        if (frame.empty())
            break;

        cv::imshow(window_name, frame);
        char key = (char) cv::waitKey(5); //delay N millis, usually long enough to display and capture input
        switch (key) {
            case 'q':
            case 'Q':
            case 27: //escape key
                capture_flag = false;
                return 0;
            case ' ': //Save an image
                sprintf(filename, "filename%.3d.jpg", n++);
                cv::imwrite(filename, frame);
                cout << "Saved " << filename << endl;
                break;
            default:
                break;
        }
    }
    return 0;
}

/**
 * Method used to test different algorithms on individual video frames
 * @param frame A reference of the video frame, result will be stored inplace
 * @return alarm metric
 */
long SKvideo::process_frame(cv::Mat &frame) {
    gettimeofday(&begin, NULL);
    long status = 0;
    //Mixture of Gaussians background model, needs to be static so it maintains its
    //state between calls.
    static cv::BackgroundSubtractorMOG2 bg_model;
    cv::Mat mask;
    bg_model(frame, mask);
    //Apply morphological erosion to remove noise from the BG model (computationally expensive!)
    cv::erode(mask, mask, dilate_mask);
    //Output a simple candidate parameter for motion detection. ie: the number of white pixels
    status = (long) cv::sum(mask)[0] / 255;
    if (OUTPUT_SENSOR_DATA)
        printf("sum: %ld\n", status);

    //Set the visualization frame to be the processed mask
    //frame = mask;
    gettimeofday(&end, NULL);
    if (OUTPUT_PERFORMANCE_METRICS)
        SKio::debug("time to run process frame: %ld ms\n", (((end.tv_usec + (end.tv_sec * 1000000))-((begin.tv_usec)+(begin.tv_sec * 1000000))) / 1000));

    //this->detect_faces(frame);
    
    return status;
}

/**
 * This test function draws contours around faces, so we can test the cascade
 * classifier
 * @TODO: Will be converted in the final face cropping utility
 * @param frame input image frame
 */
void SKvideo::detect_faces(cv::Mat &frame){
    std::vector<cv::Rect> faces;
    cv::Mat frame_gray;

   cv::cvtColor( frame, frame_gray, cv::COLOR_BGR2GRAY );
   cv::equalizeHist( frame_gray, frame_gray );
   //-- Detect faces
   face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|cv::CASCADE_SCALE_IMAGE, cv::Size(30, 30) );

   for( size_t i = 0; i < faces.size(); i++ ){
        int h_temp =faces[i].height;    // storing original height
        faces[i].y = faces[i].y - h_temp*0.3;// y is reduced by 0.3*h
        faces[i].height = h_temp*1.6;
      cv::rectangle( frame, faces[i], cv::Scalar( 0, 255, 0 ), 2, 8, 0 ); 
    }
}

/**
 * 
 * @param alarm_metric
 */
void SKvideo::alarm_state_update(long alarm_metric) {

    //Check if the alarm isn't inhibited
    gettimeofday(&now, NULL);
    int time_delta = (((now.tv_usec + (now.tv_sec * 1000000))-((last_known_alarm.tv_usec)+(last_known_alarm.tv_sec * 1000000))) / 1000000);
    if (time_delta > ALARM_INHIBIT_TIMER) {
        //A simple hysteresis implementation to reduce sensor response noise
        if (alarm_metric > ALARM_THRESHOLD && alarm_event_count < ALARM_COUNT_THRESHOLD)
            alarm_event_count++;
        else if (alarm_event_count > 0)
            alarm_event_count--;

        if (alarm_event_count == ALARM_COUNT_THRESHOLD) {
            alarm_event_count = 0; //re-arm the alarm
            this->process_alarm_event(alarm_metric, video_buffer);
            //We're issuing an alarm, so we update the last known alarm timer to inhibit the alarm
            last_known_alarm = now;
        }
    }
}

/**
 * This method looks at the video buffer window and determines what to do
 * with the motion alarm
 * 
 * Since this will probably be a long-running method, it should be invoked in a new
 * thread. Also, we pass the buffer by copying it, even considering the copy time cost,
 * because if we pass only a reference, the main thread will continue to mess with
 * the buffer and invalidate our reference to it.
 * 
 * @param alarm_metric value for the motion alarm trigger
 * @param buffer previous seconds to the alarm occurring (determined by buffer size)
 * @return status flag
 */
int SKvideo::process_alarm_event(long alarm_metric, queue<cv::Mat> buffer) {
    int status = -1;
    puts("alarm triggered");
    cv::Mat frame;

    //Record next n seconds
    for(int i=0;i< VIDEO_FORWARD_BUFFER_SIZE;i++){
        capture >> frame;
        video_buffer.push(frame.clone());
    }
    
    
    //First encode - high-quality
    this->encode_video(video_buffer);
    //Encode video for web (constant quality 30, 15 FPS)
    SKio::reencode_alarm_video(30,15);
    //Send email with video
    SKio::send_email_alarm("mlashcorp@gmail.com","Skaut alarm triggered - better see what's going on ...","");
    return status;
}


/**
 * This method encodes a series of frames into a video. Codecs available
 * depend on FFMPEG bindings on compile
 * 
 * @return status flag
 */
int SKvideo::encode_video(queue<cv::Mat> buffer) {
    cout << "Starting video encoding ...";
    cv::VideoWriter outputVideo;
    const string filename = "alarm.avi";

    outputVideo.open(filename, CV_FOURCC('D', 'I', 'V', 'X'), (double) 25, buffer.front().size(), true);

    if (!outputVideo.isOpened()) {
        cout << "Could not open the output video for write: \n";
        return -1;
    }

    cout.flush();

    while (buffer.size()) {
        outputVideo << buffer.front();
        buffer.pop();
    }
    cout << "done writing video file\n";
    return 0;
}

SKvideo::SKvideo() {
}

SKvideo::SKvideo(const SKvideo& orig) {
}

SKvideo::~SKvideo() {
}