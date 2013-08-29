//Copyright 2013 Anshul Samar. All Rights Reserved.
//Using Libccv code from LiuLiu

#include <QThread>
#include <vector>
#include <QString>
#include <QFile>
#include <QTextStream>
#include "Config.h"
#include "ImAcq.h"
#include "Gui.h"
#include "Settings.h"
#include "tracker.h"

extern "C" {
#include <ccv.h>
}
#ifdef HAVE_AVCODEC
extern "C" {
#include <libavcodec/avcodec.h>
}
#endif
#ifdef HAVE_AVFORMAT
extern "C" {
#include <libavformat/avformat.h>
}
#endif
#ifdef HAVE_SWSCALE
extern "C" {
#include <libswscale/swscale.h>
}
#endif

using namespace tld;
using namespace cv;

/* Function: Tracker()
 * --------------------
 * Constructs a tracker object based on frame and given coordinates.
 */

Tracker::Tracker(long frameCount, int trackerId, std::string videoPath, QSemaphore* trackerSemaphore, QSemaphore *mainSemaphore, CvRect *rect, long startFrame, long endFrame, const ccv_tld_param_t ccv_tld_params, ccv_dense_matrix_t** x, ccv_dense_matrix_t** y)
{

    this->xPtr = x;
    this->yPtr = y;
    this->done = false;
    this->ccvtld = NULL;
    this->frameCount = frameCount;
    this->trackerId = trackerId;
    this->rect = rect;
    this->trackerSemaphore = trackerSemaphore;
    this->mainSemaphore = mainSemaphore;
    this->startFrame = startFrame;
    this->endFrame = endFrame;

   ccv_rect_t ccvBox = ccv_rect(rect->x, rect->y, rect->width, rect->height);
   ccvtld = ccv_tld_new(*xPtr, ccvBox, ccv_tld_params);

}

/* Function: reinitialize(ccv_tld_param_t ccv_tld_params)
 * --------------------
 * Reinitializes a tracker ("rect" should have been updated by the main.cpp thread)
 */

void Tracker::reinitialize(ccv_tld_param_t ccv_tld_params,  ccv_dense_matrix_t* ptr){

    ccv_tld_free(ccvtld);
    ccv_rect_t ccvBox = ccv_rect(rect->x, rect->y, rect->width, rect->height);
    ccvtld = ccv_tld_new(ptr, ccvBox, ccv_tld_params);
    std::cout << "now here" << std::endl;

}

/* Function: ~Tracker()
 * ---------------------
 * Destructor implementation.
 */

Tracker::~Tracker(){

    ccv_tld_free(ccvtld);
}


/* Function: track()
 * -----------------
 * Loops through the video to track (does tracking frame by frame and does so
 * dependening on signal from main via semaphore). Note: If tracker object is
 * initialized on frame t, the tracking begins on frame t+1. Thus, in a manual
 * initialization on the very first frame, frameCount = 1, startFrame = 1, endFrame =
 * -1 before track() is called. main gives permission to tracker object to process
 * frame number 2 and then displays the results on frame number 2 in the main doWork
 * function.
 */

void Tracker::track(){

    int got_picture;

    while (true){
        frameCount++; //the new value of frameCount is the frameNumber that will be processed
        trackerSemaphore->acquire(); //main is ready for tld object to process frame
        if (done) break;
        ccv_tld_info_t info;
       // std::cout << "about to track obj" << std::endl;
        ccv_comp_t newbox = ccv_tld_track_object(ccvtld, *xPtr, *yPtr, &info);
        //std::cout << "object tracked" << std::endl;
        rect->x = newbox.rect.x;
        rect->y = newbox.rect.y;
        rect->width = newbox.rect.width;
        rect->height = newbox.rect.height;
        mainSemaphore->release();
    }
}

/* Function: run()
 * ---------------
 * This function occurs in different thread (see QThread documentation).
 */

void Tracker::run(){
    track();
}


/* Function: found()
 * -----------------
 * Returns true if the tracker object found the object it was looking for in the current frame
 */

bool Tracker::found(){
    return ccvtld->found;
}

/* Function: trackingComplete()
 * ---------------------------
 * Bool to inform object that tracking is complete.
 */

void Tracker::trackingComplete(){
    done = true;
}
