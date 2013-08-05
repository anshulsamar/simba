#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QInputDialog>
#include <QLineEdit>
#include <fstream>
#include "Main.h"
#include "Config.h"
#include "Gui.h"
//#include "global.h"
#include "Settings.h"
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

bool Main::doWork(Settings* settings) {


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

    bool ok;

    std::string imagePath = videoPath;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%07ld", frameCount);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");

    IplImage *img = cvLoadImage(imagePath.c_str());
    if (!img)
        return EXIT_FAILURE;

    Mat grey(img->height, img->width, CV_8UC1);
    cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);

    std::string saveImage;

    /*std::string saveImage = resultsDirectory;
    memset(num, 0, 64);
    sprintf(num, "%07ld", frameCount);
    saveImage += QString(num).toStdString();
    saveImage += std::string(".png");*/

    //cvSaveImage(saveImage.c_str(), img);

    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.35, 0.35, 0, 1, 8);
    gui->setWidthHeight(img->width, img->height);
    gui->showImage(img);

    ccv_enable_default_cache();
    ccv_read(imagePath.c_str(), &x, CCV_IO_GRAY | CCV_IO_ANY_FILE);

    gui->setMouseHandler();
    char videoKey = gui->getKey();

    while (videoKey != 'p'){
        if (videoKey == 'q')
            return true;
        if (videoKey == 'a')
            initializeTracking(img, ccv_tld_params);
        gui->setMouseHandler();
        videoKey = gui->getKey();
    }

    bool reuseFrameOnce = false;
    bool skipProcessingOnce = false;

    while(true)
    {
        cvReleaseImage(&img);

        frameCount++;

        imagePath = videoPath;
        memset(num, 0, 64);
        sprintf(num, "%07ld", frameCount);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");

        img = cvLoadImage(imagePath.c_str());
        if (!img)
            break;
        cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);

        ccv_read(imagePath.c_str(), &y, CCV_IO_GRAY | CCV_IO_ANY_FILE);

        for (int i = 0; i < numTrackers; i++){
          if (trackers[i] != NULL)
                trackerSems[i]->release();
        }

        for (int i = 0; i < numTrackers; i++){
            if (trackers[i] != NULL)
                mainSems[i]->acquire();
        }

        for (int i = 0; i < numTrackers; i++){

            if (trackers[i] != NULL && trackers[i]->found()) {
                struct colors* c = groupColors[trackersToGroupMap[i]];
                CvScalar rectangleColor = CV_RGB(c->r, c->g, c->b);
                CvPoint topLeft = cvPoint(rectangles[i]->x, rectangles[i]->y);
                CvPoint bottomRight = cvPoint(rectangles[i]->x + rectangles[i]->width, rectangles[i]->y + rectangles[i]->height);
                cvRectangle(img, topLeft, bottomRight, rectangleColor, 1, 4, 0);
                std::string name = idToGroupName.at(trackersToGroupMap[i]) + ": " + idToTrackerName.at(i);
                cvPutText(img, name.c_str(), cvPoint(topLeft.x, topLeft.y - 2), &font, rectangleColor);
                QString s = "\n" + QString::number(rectangles[i]->x) + " " + QString::number(rectangles[i]->height) + " " + QString::number(rectangles[i]->width) + " " + QString::number(rectangles[i]->height);
                trackerResults[i].append(s.toStdString());

            }

        }

        ccv_matrix_free(x);
        x = y;
        y = 0;

        gui->showImage(img);

        saveImage = resultsDirectory;
        memset(num, 0, 64);
        sprintf(num, "%07ld", frameCount);
        saveImage += QString(num).toStdString();
        saveImage += std::string(".png");

        cvSaveImage(saveImage.c_str(), img);

        gui->setMouseHandler();
        videoKey = gui->getKey();

        while (videoKey == 'p') {
            char newKey = gui->getKey();
            if (newKey == 'a') {
                videoKey = 'a';
            }
            if (newKey == 'q'){
                videoKey = 'q';
            }
            if (newKey == 'i')
                videoKey = 'i';
            if (newKey == 'd')
                videoKey = 'd';
            if (newKey == 'r')
                videoKey = 'r';
            if (newKey == 'b')
                videoKey = 'b';
            if (newKey == 'p') break;
        }
        if (videoKey == 'i'){
            analysis();
            //gui->setMouseHandler();
            videoKey = gui->getKey();
            while (videoKey != 'a' && videoKey != 'p' && videoKey != 'q') {
                //gui->setMouseHandler();
                videoKey = gui->getKey();
            }
        }
        while(videoKey == 'a')
        {
            initializeTracking(img, ccv_tld_params);
            //gui->setMouseHandler();
            videoKey = gui->getKey();
            while (videoKey != 'a' && videoKey != 'q' && videoKey != 'p')
                 videoKey = gui->getKey();
            if (videoKey == 'p' || videoKey == 'q') break;
        }
        bool ok = true;
        while (videoKey == 'd' && ok){
            QWidget w;
            QString number = QInputDialog::getText(&w, QString("Tracker to delete"),QString("Enter tracker number:"), QLineEdit::Normal,"", &ok);
            int trackerToDelete = number.toInt();
            if (ok) deleteTracker(trackerToDelete);
            videoKey = gui->getKey();
            while (videoKey != 'a' && videoKey != 'q' && videoKey != 'p' && videoKey != 'd')
                 videoKey = gui->getKey();
            //add some ability to also hit a from here
        }
        if (videoKey == 'r'){
            doDpm(img, imagePath, ccv_tld_params);
        }
        if (videoKey == 'b'){
            doBBF(img, imagePath, ccv_tld_params);
        }
        if(videoKey == 'q') {
            break;
        }

    }

    for (int i = 0; i < trackerResults.size(); i++){
        std::cout << trackerResults[i] << std::endl;

    }

    for (int i = 0; i < numGroups; i++){

        std::string filename = resultsDirectory + idToGroupName[i] + ".txt";
        //std::cout << filename << std::endl;
        FILE* file = fopen(filename.c_str(), "w");
        if (file == NULL){
            //std::cout << "Error in opening new file to write. Continuing without saving results." << std::endl;
        }

        fprintf(file, "%s", idToGroupName[i].c_str());

        for (int j = 0; j < trackersPerGroup[i].size(); j++){
            fprintf(file, "%s", "\n#");
            fprintf(file, "\n%s", trackerResults[trackersPerGroup[i][j]].c_str());
        }

        fclose(file);

    }

    std::string settingsFileName = resultsDirectory + "groupSettings.ini";
    FILE* file = fopen(settingsFileName.c_str(), "w");
    //so technically what if it was incorrectly stopped in between/is this frame count right?
    fprintf(file, "$s\nFrameCount=%ld", "[Info]", frameCount);
    fprintf(file, "%s\nNames=", "[GroupNames]");
    for (int i = 0; i < numGroups; i++){
       fprintf(file, "%s ", idToGroupName[i].c_str());
    }

    for (int i = 0; i < numGroups; i++){
        fprintf(file, "\n[%s]", idToGroupName[i].c_str());
        fprintf(file, "\nRed=%d", groupColors[i]->r);
        fprintf(file, "\nGreen=%d", groupColors[i]->g);
        fprintf(file, "\nBlue=%d", groupColors[i]->b);
    }

    fclose(file);

    //do memory cleanup of x/y and release image
    ccv_disable_cache();
    return true;
}

void Main::doBBF(IplImage* img, std::string imagePath, ccv_tld_param_t ccv_tld_params){

    IplImage* bbfInitializationImage = (IplImage* )cvClone(img);
    //ccv_enable_default_cache();
    ccv_dense_matrix_t* bbfImage = 0;
    ccv_bbf_classifier_cascade_t* cascade = ccv_load_bbf_classifier_cascade("./TLD.app/Contents/Resources/samples/face");

    ccv_read(imagePath.c_str(), &bbfImage, CCV_IO_GRAY | CCV_IO_ANY_FILE);

    if (bbfImage != 0)
    {
        ccv_array_t* seq = ccv_bbf_detect_objects(bbfImage, &cascade, 1, ccv_bbf_default_params);
        if (seq->rnum == 0)     //std::cout << "nothing found by bbf" << std::endl;
        for (int i = 0; i < seq->rnum; i++)
        {
            ccv_comp_t* comp = (ccv_comp_t*)ccv_array_get(seq, i);

            int trackerId = numTrackers;
            CvRect *add = new CvRect;
            add->x = comp->rect.x;
            add->y = comp->rect.y;
            add->width = comp->rect.width;
            add->height = comp->rect.height;

            CvScalar black = CV_RGB(0, 0, 0);
            CvPoint topLeftBlack = cvPoint(add->x, add->y);
            CvPoint bottomRightBlack = cvPoint(add->x + add->width, add->y + add->height);
            cvRectangle(bbfInitializationImage, topLeftBlack, bottomRightBlack, black, 3, 8, 0);
            gui->showImage(bbfInitializationImage);

            if (!createTLD(frameCount, -1, add, ccv_tld_params, trackerId))
                delete add;


        }
        ccv_array_free(seq);
    }
    //ccv_disable_cache();

    //free image clone of bbfinitialization image
    //understand caching

}

void Main::doDpm(IplImage* img, std::string imagePath, ccv_tld_param_t ccv_tld_params){

    //ccv_enable_default_cache();

    IplImage* dpmInitializationImage = (IplImage* )cvClone(img);
    int i, j;

    ccv_dense_matrix_t* dpmImage = 0;
    ccv_read(imagePath.c_str(), &dpmImage, CCV_IO_ANY_FILE);
    ccv_dpm_mixture_model_t* model = ccv_load_dpm_mixture_model("./TLD.app/Contents/Resources/samples/pedestrian.m");

    ccv_array_t* seq = ccv_dpm_detect_objects(dpmImage, &model, 1, ccv_dpm_default_params);
    if (seq){
        for (i = 0; i < seq->rnum; i++)
        {
            ccv_root_comp_t* comp = (ccv_root_comp_t*)ccv_array_get(seq, i);

            int trackerId = numTrackers;
            CvRect *add = new CvRect;
            add->x = comp->rect.x;
            add->y = comp->rect.y;
            add->width = comp->rect.width;
            add->height = comp->rect.height;

            CvScalar black = CV_RGB(0, 0, 0);
            CvPoint topLeftBlack = cvPoint(add->x, add->y);
            CvPoint bottomRightBlack = cvPoint(add->x + add->width, add->y + add->height);
            cvRectangle(dpmInitializationImage, topLeftBlack, bottomRightBlack, black, 3, 8, 0);
            gui->showImage(dpmInitializationImage);

            if (!createTLD(frameCount, -1, add, ccv_tld_params, trackerId))
                delete add;

            for (j = 0; j < comp->pnum && j < 2; j++){
                int trackerId = numTrackers;
                CvRect *add = new CvRect;
                add->x = comp->part[j].rect.x;
                add->y = comp->part[j].rect.y;
                add->width = comp->part[j].rect.width;
                add->height = comp->part[j].rect.height;
                if (!createTLD(frameCount, -1, add, ccv_tld_params, trackerId))
                    delete add;
             }

        }


    }
    ccv_array_free(seq);
    //ccv_drain_cache();
    ccv_dpm_mixture_model_free(model);
    ccv_matrix_free(dpmImage);

}

void Main::analysis(){


}


/* Function: createTLD()
 * ---------------------
 * Given that all required TLD information has been appended on necessary data structures (i.e. startFrames,
 * endFrames, rectangles), this creates the TLD object as well as related semaphores for thread features.
 * If required, it also updates the ini file.
 */

bool Main::createTLD(long startFrame, long endFrame, CvRect *rect, const ccv_tld_param_t ccv_tld_params, int trackerId){

    bool ok = false;
    QWidget w;
    QString groupName = QInputDialog::getText(&w, QString("Group Name. No '#' or '-'"),QString("Enter group name:"), QLineEdit::Normal,"", &ok);
    if (!ok) return false;
    int groupNumber;

    QString trackerName = QInputDialog::getText(&w, QString("Tracker Name (no '#' or '-')"),QString("Enter tracker name:"), QLineEdit::Normal,"", &ok);
    if (!ok) return false;

    if (trackerNameToId.find(trackerName.toStdString()) != trackerNameToId.end()){
        trackerName = QInputDialog::getText(&w, QString("Tracker Name exists. (no '#' or '-')"),QString("Enter a new tracker name:"), QLineEdit::Normal,"", &ok);
        if (!ok) return false;
    }

    addTrackerInfo(startFrame, endFrame, rect, trackerName.toStdString(), trackerId);

    trackerSems.push_back(new QSemaphore());
    mainSems.push_back(new QSemaphore());
    Tracker* t = new Tracker(frameCount, trackerId, videoPath, trackerSems[trackerId], mainSems[trackerId], rectangles[trackerId], startFrames[trackerId], endFrames[trackerId], ccv_tld_params, saveResults, resultsDirectory, &x, &y);
    trackers.push_back(t);

    if (groupNameToId.find(groupName.toStdString()) == groupNameToId.end()){
        groupNumber = numGroups;
        groupNameToId.insert(std::pair<std::string,int>(groupName.toStdString(),numGroups));
        idToGroupName.insert(std::pair<int, std::string>(numGroups, groupName.toStdString()));
        std::vector<int> v;
        trackersPerGroup.push_back(v);
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
        //std::cout << QString::number(numGroups).toStdString() << std::endl;
    } else {
        groupNumber = groupNameToId.at(groupName.toStdString());
    }

    trackersToGroupMap[trackerId] = groupNumber;
    trackersPerGroup[groupNumber].push_back(trackerId);

    t->start();
    numTrackers++;
    return true;



}

/* Function: addTrackerInfo()
 * ---------------------
 * Appends required information for tracker at the end of three data structures: startFrames, endFrames,
 * and rectangles. This is only modified and accessed by the main thread (furthermore, while a better
 * approach is required, currently the ini file information is added from config.cpp in the same order
 * that the TLD objects are eventually created).
 */

void Main::addTrackerInfo(long startFrame, long endFrame, CvRect* add, std::string trackerName, int trackerId){

    startFrames.push_back(startFrame);
    endFrames.push_back(endFrame);
    rectangles.push_back(add);
    trackerNameToId.insert(std::pair<std::string,int>(trackerName,trackerId));
    idToTrackerName.insert(std::pair<int, std::string>(trackerId, trackerName));
    trackerResults.push_back(trackerName);
    trackerResults[trackerId].append("\n#");
    for (int i = 0; i < frameCount; i++){
        trackerResults[trackerId].append("\n-");

    }
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

    if (!createTLD(frameCount, -1, add, ccv_tld_params, trackerId))
        delete add;

    gui->showImage(img);
    cvReleaseImage(&img0);
}

void Main::deleteTracker(int i){

    if (trackers[i] != NULL && i < numTrackers){
    trackers[i]->trackingComplete();
    trackerSems[i]->release();
    trackers[i]->wait();
    delete trackers[i];
    delete trackerSems[i];
    delete mainSems[i];
    delete rectangles[i];
    trackers[i] = NULL;
    }

}

/* Function: deleteTrackers()
 * -------------------------
 * Deletes trackers/breaks them out of their loop
 */

void Main::deleteTrackersAndGroups(){

    for (int i = 0; i < numTrackers && trackers[i] != NULL; i++){
        deleteTracker(i);
    }

    for (int i = 0; i < groupColors.size(); i++){
        delete groupColors[i];
    }

}

Main::~Main(){

    deleteTrackersAndGroups();
    delete gui;
    if (settingsOut != NULL && saveIni)
        delete settingsOut;

}

void Main::initGui(int videoX, int videoY){

    gui->initVideoWindow(videoX, videoY);

}

