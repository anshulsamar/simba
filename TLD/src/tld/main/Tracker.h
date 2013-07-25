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

using namespace tld;
using namespace cv;

#include <QThread>
#include <QFile>
#include <QSemaphore>
#include <QMutex>

class Tracker : public QThread
{
public:
    Tracker(int frameCount, int trackerId, std::string videoPath, QSemaphore* trackerSemaphore, QSemaphore *mainSemaphore, CvRect *rect, int startFrame, int endFrame, const ccv_tld_param_t ccv_tld_params, bool saveResults, std::string resultsDirectory, QTextEdit* textEdit, QMutex* textMutex);
    ~Tracker();
    void track();
    void reinitialize(const ccv_tld_param_t ccv_tld_params);
    bool found();
    void trackingComplete();

protected:
    void run();

private:
    ccv_tld_t* ccvtld;
    ccv_dense_matrix_t* x;
    ccv_dense_matrix_t* y;
    AVStream* video_st;
    AVFrame rgb_picture;
    AVFormatContext* ic;
    AVFrame* picture;
    AVPacket packet;
    struct SwsContext* picture_ctx;
    QSemaphore *trackerSemaphore;
    QSemaphore *mainSemaphore;
    CvRect *rect;

    int frameCount;
    int startFrame;
    int endFrame;
    int trackerId;

    FILE* file;
    bool saveResults;
    std::string resultsDirectory;
    QTextEdit* analytics;
    QMutex* textMutex;

    bool done;
};

#endif // TRACKER_H
