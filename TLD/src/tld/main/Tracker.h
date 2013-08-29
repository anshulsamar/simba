//Copyright 2013 Anshul Samar. All Rights Reserved.
//Using Libccv code from LiuLiu

#ifndef TRACKER_H
#define TRACKER_H

#include "Main.h"
#include <QTextEdit>

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

using namespace cv;

#include <QThread>
#include <QFile>
#include <QSemaphore>
#include <QMutex>

class Tracker : public QThread
{
public:
    Tracker(long frameCount, int trackerId, std::string videoPath, QSemaphore* trackerSemaphore, QSemaphore *mainSemaphore, CvRect *rect, long startFrame, long endFrame, const ccv_tld_param_t ccv_tld_params, ccv_dense_matrix_t** x, ccv_dense_matrix_t** y);
    ~Tracker();

    //finds the patch in the next images
    void track();

    //returns true if the patch is found
    bool found();

    //Called by main thread when a tracker is deleted
    void trackingComplete();

    //Reinitializes the tracker on a new patch (coordinates of which are accessed through CvRect *rect), patch is in img pointed to by ptr
    void reinitialize(ccv_tld_param_t ccv_tld_params,  ccv_dense_matrix_t* ptr);

protected:
    void run();

private:
    //the tld object to determine tracking coordinates
    ccv_tld_t* ccvtld;

    //holds images
    ccv_dense_matrix_t** xPtr;
    ccv_dense_matrix_t** yPtr;

    //semaphores for communication with main
    QSemaphore *trackerSemaphore;
    QSemaphore *mainSemaphore;

    //Holds tracking coordinates that are updated by tracker object and accessed by main
    CvRect *rect;

    //Other book keeping
    long frameCount;
    long startFrame;
    long endFrame;
    int trackerId;

    //Informs the tracker whether it is to continue tracking or terminate. When a tracker is deleted, this becomes true through the trackingComplete() function
    bool done;
};

#endif // TRACKER_H
