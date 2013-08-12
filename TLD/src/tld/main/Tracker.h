/* Lion: Libccv's TLD ("Predator") with GUI
 * ----------------------------
 * Last modified by: Anshul Samar.
 *
 * Previous Credits:
 * See QOpenTLD.cpp for previous credits/licensing/copyright.
 */


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
    Tracker(long frameCount, int trackerId, std::string videoPath, QSemaphore* trackerSemaphore, QSemaphore *mainSemaphore, CvRect *rect, long startFrame, long endFrame, const ccv_tld_param_t ccv_tld_params, bool saveResults, std::string resultsDirectory, ccv_dense_matrix_t** x, ccv_dense_matrix_t** y);
    ~Tracker();
    void track();
    bool found();
    void trackingComplete();

protected:
    void run();

private:
    ccv_tld_t* ccvtld;
    ccv_dense_matrix_t** xPtr;
    ccv_dense_matrix_t** yPtr;
    QSemaphore *trackerSemaphore;
    QSemaphore *mainSemaphore;
    CvRect *rect;

    long frameCount;
    long startFrame;
    long endFrame;
    int trackerId;

    FILE* file;
    bool saveResults;
    std::string resultsDirectory;

    bool done;
};

#endif // TRACKER_H
