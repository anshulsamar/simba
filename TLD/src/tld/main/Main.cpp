#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QInputDialog>
#include <QLineEdit>
#include <fstream>
#include "Main.h"
#include "Config.h"
#include "Gui.h"
#include "global.h"
#include <opencv/cv.h>
#include <sys/time.h>
#include <ctype.h>

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

#define BBF 1

using namespace tld;
using namespace cv;

/* Function: doWork()
 * -----------------
 * doWork() loops: for each frame of video, it allows tracker
 * objects to compute, waits on their completion, displays results, and
 * if prompted, allows for new tracker objects to new or existing groups.
 */

bool Main::doWork() {

    bool ok;

    std::string imagePath = resultsDirectory;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%05d", frameCount);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");
    std::cout << imagePath << std::endl;

    IplImage *img = cvLoadImage(imagePath.c_str());
    if (!img)
        return EXIT_FAILURE;

    ccv_enable_default_cache();
    ccv_read(imagePath.c_str(), &x, CCV_IO_GRAY | CCV_IO_ANY_FILE);

    Mat grey(img->height, img->width, CV_8UC1);
    cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);

    gui->setWidthHeight(img->width, img->height);
    gui->showImage(img);
    gui->setMouseHandler();

    char videoKey = gui->getVideoKey();

    while (videoKey != 'p'){
        if (videoKey == 'q')
            return true;
        if (videoKey == 'a')
            initializeTracking(img, ccv_tld_params);
        gui->setMouseHandler();
        videoKey = gui->getVideoKey();
    }

    bool reuseFrameOnce = false;
    bool skipProcessingOnce = false;

    while(true)
    {
        cvReleaseImage(&img);

        imagePath = resultsDirectory;
        memset(num, 0, 64);
        sprintf(num, "%05d", frameCount);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");

        img = cvLoadImage(imagePath.c_str());
        if (!img)
            break;
        cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);

        ccv_read(imagePath.c_str(), &y, CCV_IO_GRAY | CCV_IO_ANY_FILE);

        frameCount++;

        for (int i = 0; i < numTrackers; i++){
            trackerSems[i]->release();
        }

        for (int i = 0; i < numTrackers; i++){
            mainSems[i]->acquire();
        }


        for (int i = 0; i < numGroups; i++){
            groups[i]->push_back(0);
        }

        int countMissed = 0;

        for (int i = 0; i < numTrackers; i++){

            if (trackers[i]->found()) {
                struct colors* c = groupColors[trackersToGroupMap[i]];
                CvScalar rectangleColor = CV_RGB(c->r, c->g, c->b);
                CvPoint topLeft = cvPoint(rectangles[i]->x, rectangles[i]->y);
                CvPoint bottomRight = cvPoint(rectangles[i]->x + rectangles[i]->width, rectangles[i]->y + rectangles[i]->height);
                cvRectangle(img, topLeft, bottomRight, rectangleColor, 8, 8, 0);
                std::vector<int>* currGroup = groups[trackersToGroupMap[i]];
                (*currGroup)[currGroup->size() - 1]++;
            } else {
                countMissed++;
            }

        }

        ccv_matrix_free(x);
        x = y;
        y = 0;

        gui->showImage(img);
        gui->setMouseHandler();

        videoKey = gui->getVideoKey();

        while (videoKey == 'p') {
            char newKey = gui->getVideoKey();
            if (newKey == 'a') {
                videoKey = 'a';
            }
            if (newKey == 'q'){
                videoKey = 'q';
            }
            if (newKey == 'i')
                videoKey = 'i';
            if (newKey == 'p') break;
        }
        if (videoKey == 'i'){
            analysis();
            gui->setMouseHandler();
            videoKey = gui->getVideoKey();
            while (videoKey != 'a' && videoKey != 'p' && videoKey != 'q') {
                gui->setMouseHandler();
                videoKey = gui->getVideoKey();
            }
        }
        while(videoKey == 'a')
        {
            initializeTracking(img, ccv_tld_params);
            gui->setMouseHandler();
            videoKey = gui->getVideoKey();
            while (videoKey != 'a' && videoKey != 'q' && videoKey != 'p')
                 videoKey = gui->getVideoKey();
            if (videoKey == 'p' || videoKey == 'q') break;
        }

        if(videoKey == 'q') {
            break;
        }

    }

    //do memory cleanup of x/y

    ccv_disable_cache();
    return true;
}

void Main::analysis(){


}


/* Function: createTLD()
 * ---------------------
 * Given that all required TLD information has been appended on necessary data structures (i.e. startFrames,
 * endFrames, rectangles), this creates the TLD object as well as related semaphores for thread features.
 * If required, it also updates the ini file.
 */

void Main::createTLD(CvRect *rect, const ccv_tld_param_t ccv_tld_params, int trackerId){

    trackerSems.push_back(new QSemaphore());
    mainSems.push_back(new QSemaphore());
    Tracker* t = new Tracker(frameCount, trackerId, videoPath, trackerSems[trackerId], mainSems[trackerId], rectangles[trackerId], startFrames[trackerId], endFrames[trackerId], ccv_tld_params, saveResults, resultsDirectory, &x, &y);
    trackers.push_back(t);

    bool ok;
    QWidget w;
    QString number = QInputDialog::getText(&w, QString("Group Number"),QString("Enter group number:"), QLineEdit::Normal,"", &ok);
    int groupNumber = number.toInt();

    if (!loadIni){
    while (!ok){
        analytics->append("You must provide a group number.");
        number = QInputDialog::getText(&w, QString("Enter Group Number"),QString("Group Number:"), QLineEdit::Normal,"", &ok);
        groupNumber = number.toInt();
    }

    if (groupNumber >= numGroups || groupNumber < 0){
        groupNumber = numGroups;
        groups.push_back(new std::vector<int>);
        groupNumTrackers.push_back(0);
        struct colors *c = new struct colors;
        if (groupNumber == 0){
            c->r = 225;
            c->g = 224;
            c->b = 115;
        } else if (groupNumber == 1){
            c->r = 247;
            c->g = 111;
            c->b = 135;
        } else if (groupNumber == 2){
            c->r = 146;
            c->g = 237;
            c->b = 107;
        } else if (groupNumber == 3) {
            c->r = 123;
            c->g = 114;
            c->b = 216;
        }  else {
            c->r = 0;
            c->g = 0;
            c->b = 0;
        }

        groupColors.push_back(c);
        numGroups++;
    }
    groupNumTrackers[groupNumber]++;
    trackersToGroupMap[trackerId] = groupNumber;
    }

    t->start();
    numTrackers++;

}

/* Function: addTrackerInfo()
 * ---------------------
 * Appends required information for tracker at the end of three data structures: startFrames, endFrames,
 * and rectangles. This is only modified and accessed by the main thread (furthermore, while a better
 * approach is required, currently the ini file information is added from config.cpp in the same order
 * that the TLD objects are eventually created).
 */

void Main::addTrackerInfo(int startFrame, int endFrame, CvRect* add){

    startFrames.push_back(startFrame);
    endFrames.push_back(endFrame);
    rectangles.push_back(add);

}

/* Function: initializeTracking()
 * ------------------------------
 * Tracking is initialized on the current image. The user is allowed to add
 * a new tracker.
 */

void Main::initializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params){

    int trackerId = numTrackers;
    CvScalar black = CV_RGB(0, 0, 0);

    IplImage *img0 = (IplImage *) cvClone(img);

    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
    gui->showImage(img0);

    std::string message = string("Tracker number: ") + QString::number(trackerId).toStdString() + string(". Frame number: ") + QString::number(frameCount).toStdString() + string(". Draw a bounding box and press enter");
    CvRect boxUser;
    if(getBBFromUser(img, boxUser, gui, trackerId, message) == PROGRAM_EXIT) return;
    CvRect *add = new CvRect;
    add->x = boxUser.x;
    add->y = boxUser.y;
    add->width = boxUser.width;
    add->height = boxUser.height;
    addTrackerInfo(frameCount, -1, add);

    createTLD(rectangles[trackerId], ccv_tld_params, trackerId);

    if (saveIni){
        QString trackerString = QString("tracker") + QString::number(trackerId);
        trackerString += QString("/");
        settingsOut->setValue(trackerString + QString("startFrame"), QVariant(frameCount));
        settingsOut->setValue(trackerString + QString("endFrame"), -1 );
        settingsOut->setValue(trackerString + QString("x"), add->x);
        settingsOut->setValue(trackerString + QString("y"), add->y);
        settingsOut->setValue(trackerString + QString("width"), add->width);
        settingsOut->setValue(trackerString + QString("height"), add->height);
        settingsOut->sync();
    }

    trackerId++;
    gui->showImage(img);
    cvReleaseImage(&img0);
}

/* Function: deleteTrackers()
 * -------------------------
 * Deletes trackers/breaks them out of their loop
 */

void Main::deleteTrackersAndGroups(){

    for (int i = 0; i < numTrackers; i++){
        trackers[i]->trackingComplete();
        trackerSems[i]->release();
        trackers[i]->wait();
        delete trackers[i];
        delete trackerSems[i];
        delete mainSems[i];
        delete rectangles[i];
    }

    for (int i = 0; i < groups.size(); i++){
        delete groups[i];
        delete groupColors[i];
    }

}

Main::~Main(){

    deleteTrackersAndGroups();
    cvReleaseImage(&graphImage);
    imAcqFree(imAcq);
    delete gui;
    delete textMutex;
    if (settingsOut != NULL && saveIni)
        delete settingsOut;

}

void Main::initGui(int videoX, int videoY){

    imAcqInit(imAcq);
    gui->initVideoWindow(videoX, videoY);

}

/*
    const ccv_tld_param_t ccv_tld_params = {
        .win_size = {
            settings->win_size_width,
            settings->win_size_height,
        },
        .level = settings->level,
        .min_forward_backward_error = settings->min_forward_backward_error,
        .min_eigen = settings->min_eigen,
        .min_win = settings->min_win,
        .interval = settings->interval,
        .shift = settings->shift,
        .top_n = settings->top_n,
        .rotation = settings->rotation,
        .include_overlap = settings->include_overlap,
        .exclude_overlap = settings->exclude_overlap,
        .structs = settings->structs,
        .features = settings->features,
        .validate_set = settings->validate_set,
        .nnc_same = settings->nnc_same,
        .nnc_thres = settings->nnc_thres,
        .nnc_verify = settings->nnc_verify,
        .nnc_beyond = settings->nnc_beyond,
        .nnc_collect = settings->nnc_collect,
        .bad_patches = settings->bad_patches,
        .new_deform = settings->new_deform,
        .track_deform = settings->track_deform,
        .new_deform_angle = settings->new_deform_angle,
        .track_deform_angle = settings->track_deform_angle,
        .new_deform_scale = settings->new_deform_scale,
        .track_deform_scale = settings->track_deform_scale,
        .new_deform_shift = settings->new_deform_shift,
        .track_deform_shift = settings->track_deform_shift,
    };
*/
