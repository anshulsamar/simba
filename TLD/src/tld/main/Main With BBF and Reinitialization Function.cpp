//credits in other file

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
 * If user loaded ini file, creates tracker objects (the rest of the ini file has been already
 * loaded into data structures startFrames, endFrames, groups, and rectangles). Then, user is prompted for
 * manual ini/reinitialization. doWork() loops: for each frame of video, it allows tracker
 * objects to compute, waits on their completion, displays results, and
 * if prompted, allows for new tracker objects to new or existing groups.
 */

bool Main::doWork() {

        /*USE CODE IN THIS COMMENT IF YOU WANT BBF INITIZILIZATIONs
        // init av-related structs
        AVFormatContext* ic = 0;
        int video_stream = -1;
        AVStream* video_st = 0;
        AVFrame* picture = 0;
        AVFrame rgb_picture;
        memset(&rgb_picture, 0, sizeof(AVPicture));
        AVPacket packet;
        memset(&packet, 0, sizeof(AVPacket));
        av_init_packet(&packet);
        av_register_all();
        avformat_network_init();
        // load video and codec
        avformat_open_input(&ic, videoPath.c_str(), 0, 0);
        avformat_find_stream_info(ic, 0);
        int i;
        for (i = 0; i < ic->nb_streams; i++)
        {
            AVCodecContext* enc = ic->streams[i]->codec;
            enc->thread_count = 2;
            if (AVMEDIA_TYPE_VIDEO == enc->codec_type && video_stream < 0)
            {
                AVCodec* codec = avcodec_find_decoder(enc->codec_id);
                if (!codec || avcodec_open2(enc, codec, 0) < 0)
                    continue;
                video_stream = i;
                video_st = ic->streams[i];
                picture = avcodec_alloc_frame();
                rgb_picture.data[0] = (uint8_t*)ccmalloc(avpicture_get_size(PIX_FMT_RGB24, enc->width, enc->height));
                avpicture_fill((AVPicture*)&rgb_picture, rgb_picture.data[0], PIX_FMT_RGB24, enc->width, enc->height);
                break;
            }
        }
        int got_picture = 0;
        while (!got_picture)
        {
            int result = av_read_frame(ic, &packet);
            if (result == AVERROR(EAGAIN))
                continue;
            avcodec_decode_video2(video_st->codec, picture, &got_picture, &packet);
        }
        ccv_enable_default_cache();
        struct SwsContext* picture_ctx = sws_getCachedContext(0, video_st->codec->width, video_st->codec->height, video_st->codec->pix_fmt, video_st->codec->width, video_st->codec->height, PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
        sws_scale(picture_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, video_st->codec->height, rgb_picture.data, rgb_picture.linesize);

        //BBF initialization

        ccv_enable_default_cache();
        ccv_dense_matrix_t* image = 0;
        ccv_bbf_classifier_cascade_t* cascade = ccv_load_bbf_classifier_cascade("/Users/Anshul/Desktop/FrankLab/ccv/samples/face");

        //BBF initialization


        ccv_dense_matrix_t* x = 0;
        ccv_read(rgb_picture.data[0], &x, CCV_IO_RGB_RAW | CCV_IO_GRAY, video_st->codec->height, video_st->codec->width, rgb_picture.linesize[0]);

        if (x != 0)
        {
            ccv_array_t* seq = ccv_bbf_detect_objects(x, &cascade, 1, ccv_bbf_default_params);
            for (int i = 0; i < seq->rnum; i++)
            {
                ccv_comp_t* comp = (ccv_comp_t*)ccv_array_get(seq, i);
                printf("%d %d %d %d %f\n", comp->rect.x, comp->rect.y, comp->rect.width, comp->rect.height, comp->confidence);
                fflush(stdout);
            }
            ccv_array_free(seq);
            ccv_matrix_free(x);
        }

        ccv_dense_matrix_t* y = 0;
*/


    bool ok;

    if (loadIni){
        for (int trackerId = 0; trackerId < trackersInIniFile; trackerId++){
            createTLD(rectangles[trackerId], ccv_tld_params, trackerId);
        }
        analytics->append(QString("Created Trackers from Initialization File"));
    }

    IplImage *img = imAcqGetImg(imAcq);
    Mat grey(img->height, img->width, CV_8UC1);
    cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);

    analytics->append(QString("Press > to start or + to add a tracker"));

    gui->setWidthHeight(img->width, img->height);
    gui->showImage(img);
    gui->setMouseHandler();

    gui->showGraph(graphImage);

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
    IplImage *graphCopy = (IplImage *) cvClone(graphImage);
    int xCoord = 20;

    bool analyze = true;

    while(imAcqHasMoreFrames(imAcq))
    {
        for (int i = 0; i < numTrackers; i++){
            trackerSems[i]->release();
        }

        for (int i = 0; i < numTrackers; i++){
            mainSems[i]->acquire();
        }

        if(!reuseFrameOnce)
        {

            img = imAcqGetImg(imAcq);

            if(img == NULL)
            {
                analytics->append(QString("Current image is NULL, assuming end of input"));
                break;

            }

           cvtColor(cv::Mat(img), grey, CV_BGR2GRAY);
        }

        frameCount++;

        //CvScalar yellow = CV_RGB(255, 255, 0);

        for (int i = 0; i < numGroups; i++){
            groups[i]->push_back(0);
        }

        int countMissed = 0;

        for (int i = 0; i < numTrackers; i++){

            if (trackers[i]->found()) {
                trackerConsecutiveMissed[i] = 0;
                struct colors* c = groupColors[trackersToGroupMap[i]];
                CvScalar rectangleColor = CV_RGB(c->r, c->g, c->b);
                CvPoint topLeft = cvPoint(rectangles[i]->x, rectangles[i]->y);
                CvPoint bottomRight = cvPoint(rectangles[i]->x + rectangles[i]->width, rectangles[i]->y + rectangles[i]->height);
                cvRectangle(img, topLeft, bottomRight, rectangleColor, 8, 8, 0);
                std::vector<int>* currGroup = groups[trackersToGroupMap[i]];
                (*currGroup)[currGroup->size() - 1]++;
                CvPoint center = cvPoint(rectangles[i]->x + rectangles[i]->width/2, rectangles[i]->y + rectangles[i]->height/2);
                cvLine(img, cvPoint(center.x-2, center.y-2), cvPoint(center.x+2, center.y+2), rectangleColor, 2);
                cvLine(img, cvPoint(center.x-2, center.y+2), cvPoint(center.x+2, center.y-2), rectangleColor, 2);
            } else {
                trackerConsecutiveMissed[i]++;
                countMissed++;
            }

        }


        if (xCoord > graphWidth){
            cvReleaseImage(&graphCopy);
            graphCopy = (IplImage *) cvClone(graphImage);
            xCoord = 20;
        }

        CvFont font;
        cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);

        /*for (int j = 0; j < groupColors.size(); j++){

            struct colors* c = groupColors[j];
            CvScalar boxColor = CV_RGB(c->r, c->g, c->b);
            int start = (j+1)*(graphHeight/4);
            cvRectangle(graphCopy, cvPoint(15, graphHeight - start - sizeBox), cvPoint(15 + sizeBox, graphHeight - start), boxColor, CV_FILLED, 8, 0);
            cvPutText(graphCopy, QString::number(groupNumTrackers[j]).toStdString().c_str(), cvPoint(spacing*(j+1) + sizeBox*.1, img->height - sizeBox - spacing + sizeBox/1.25), &font, cvScalar(0, 0, 0));

        }*/

        for (int i = 0; i < numGroups; i++){
             std::vector<int>* currGroup = groups[i];
             int size = (*currGroup)[currGroup->size() - 1];
             struct colors* c = groupColors[i];
             CvScalar pointColor = CV_RGB(c->r, c->g, c->b);
             int start = (i+1)*(graphHeight/4);
             int yCoord = graphHeight - start - size*3;
             cvLine(graphCopy, cvPoint(xCoord, yCoord), cvPoint(xCoord, yCoord), pointColor, 2);
             //std::vector<int>* currGroup = groups[trackersToGroupMap[i]];
             //cvPutText(graphCopy, QString::number((*currGroup)[currGroup->size() - 1]).toStdString().c_str(), cvPoint(5, graphHeight - start), &font, cvScalar(255, 255, 255));
        }
        if (numGroups > 0)
            xCoord += 3;

        gui->showGraph(graphCopy);
        gui->setMouseHandler();
        gui->showImage(img);

         /*//USE CODE IN THIS COMMENT IF YOU WANT BBF INITIALIZATIONS

        if (BBF){
        got_picture = 0;
        int result = av_read_frame(ic, &packet);
        if (result == AVERROR(EAGAIN))
            continue;
        avcodec_decode_video2(video_st->codec, picture, &got_picture, &packet);
        if (!got_picture)
            break;
        sws_scale(picture_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, video_st->codec->height, rgb_picture.data, rgb_picture.linesize);

        ccv_read(rgb_picture.data[0], &y, CCV_IO_RGB_RAW | CCV_IO_GRAY, video_st->codec->height, video_st->codec->width, rgb_picture.linesize[0]);

        if (countMissed == numTrackers && y != 0)
        {
            std::cout << "No tracker used using BBF to detect" << std::endl;
            ccv_array_t* seq = ccv_bbf_detect_objects(y, &cascade, 1, ccv_bbf_default_params);
            for (int i = 0; i < seq->rnum; i++)
            {
                ccv_comp_t* comp = (ccv_comp_t*)ccv_array_get(seq, i);
                printf("%d %d %d %d %d %f\n", frameCount, comp->rect.x, comp->rect.y, comp->rect.width, comp->rect.height, comp->confidence);
                fflush(stdout);

                int trackerId = numTrackers;
                CvRect *add = new CvRect;
                add->x = comp->rect.x;
                add->y = comp->rect.y;
                add->width = comp->rect.width;
                add->height = comp->rect.height;
                addTrackerInfo(frameCount, -1, add);
                createTLD(rectangles[trackerId], ccv_tld_params, trackerId);
                std::cout << "made new tld" << std::endl;
                //look at initialize to see how ini file is made and other important stuff

            }
            ccv_array_free(seq);
            ccv_matrix_free(y);
        }

        x = y;
        y = 0;
    }*/




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
            std::cout << "done with analysis" << std::endl;
            gui->setMouseHandler();
            videoKey = gui->getVideoKey();
            while (videoKey != 'a' && videoKey != 'p' && videoKey != 'q') {
                gui->setMouseHandler();
                videoKey = gui->getVideoKey();
                if (videoKey == 'q') analyze = false;
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

    std::cout << "here" << std::endl;

    if (true){
    analysis();
    gui->setMouseHandler();
    videoKey = gui->getVideoKey();
    while (videoKey != 'q')
         videoKey = gui->getVideoKey();
    }

    cvReleaseImage(&graphCopy);


/*//     USE CODE IN THIS COMMENT IF YOU WANT BBF INITIALIZATIONS
    if (BBF){
    //tld.c avi struct cleanup

    ccv_matrix_free(x);
    //ccv_tld_free(tld);
    ccfree(rgb_picture.data[0]);
    ccv_disable_cache();

    //BBF cleanup

    ccv_bbf_classifier_cascade_free(cascade);
    ccv_disable_cache();
    }*/

    return true;
}

void Main::analysis(){

    analytics->append("\nanalytics");
    analytics->append(QString("Number of trackers: ") + QString::number(numTrackers));
    analytics->append(QString("Number of groups: ") + QString::number(numGroups));
    analytics->append("");

    for (int i = 0; i < numGroups; i++){
         analytics->append(QString("Group ") + QString::number(i));
         //std::cout << (QString("Group ") + QString::number(i)).toStdString() << std::endl;
         int detection = 0;
         int count = 0;
         for (int j = 0; j < groups[i]->size(); j++){
             int trackers = (*groups[i])[j];
             //std::cout << string("trackers: ") + QString::number(trackers).toStdString() << std::endl;
             if (trackers){
                 count++;
                 detection += trackers;
                 //std::cout << string("count: ") + QString::number(count).toStdString() << std::endl;
                 //std::cout << string("detection: ") + QString::number(detection).toStdString() << std::endl;
             }
         }

         double percentFrames = (double)count/(frameCount - 1);
         double averageTrackersPerDetection = (double)detection/count;
         analytics->append(QString("Percent frames of appearance: ") + QString::number(percentFrames));
         analytics->append(QString("Average trackers per detection: ") + QString::number(averageTrackersPerDetection));
    }
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
    trackerConsecutiveMissed.push_back(0);
    Tracker* t = new Tracker(frameCount, trackerId, videoPath, trackerSems[trackerId], mainSems[trackerId], rectangles[trackerId], startFrames[trackerId], endFrames[trackerId], ccv_tld_params, saveResults, resultsDirectory, analytics, textMutex);
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
    if (saveIni) {
        settingsOut->setValue(QString("info/numTrackers"), numTrackers);
        settingsOut->sync();
    }
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
 * a new tracker. The function for re-initialization is below thus, but is not
 * currently used.
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

//Function: currently not used or fully tested, nor IplImage memory cvReleaseImage done here (playground in progress)

void Main::reinitializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params){


    IplImage* img0 = (IplImage *) cvClone(img);
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
    analytics->append("Would you like to reinitialize trackers? Press y for yes, enter otherwise");
    /*cvRectangle(img0, cvPoint(0, 0), cvPoint(img0->width, 30), black, CV_FILLED, 8, 0);
    cvPutText(img0, "Would you like to reinitialize trackers? Press y for yes, enter otherwise", cvPoint(10, 20),
              &font, cvScalar(255, 255, 0));
    cvPutText(img0, QString::number(frameCount).toStdString().c_str(), cvPoint(650, 20),
              &font, cvScalar(255, 255, 0));*/
    gui->showImage(img0);
    char key = cvWaitKey(0);
    char videoKey = gui->getVideoKey();

    if (key == 'y' || videoKey == 'y'){
        for (int trackerId = 0; trackerId < numTrackers; trackerId++){
            img0 = (IplImage *) cvClone(img);
            std::ostringstream numberString;
            numberString << trackerId;
            CvScalar black = CV_RGB(0, 0, 0);
            cvRectangle(img0, cvPoint(0, 0), cvPoint(img0->width, 30), black, CV_FILLED, 8, 0);
            std::string trackerMessage = "Reinitialize tracker number: "  + numberString.str() + "? Press y for yes, enter otherwise";
            analytics->append(QString(trackerMessage.c_str()));
            /*cvPutText(img0, QString::number(frameCount).toStdString().c_str(), cvPoint(650, 20),
                      &font, cvScalar(255, 255, 0));
            cvPutText(img0, trackerMessage.c_str(), cvPoint(10, 20),
                      &font, cvScalar(255, 255, 0));*/
            gui->showImage(img0);
            key = cvWaitKey(0);
            videoKey = gui->getVideoKey();
            if (key == 'y' || videoKey == 'y'){
                CvRect boxUser;
                std::string message = "Draw a bounding box and press enter";
                analytics->append(QString(message.c_str()));
                //cvPutText(img0, QString::number(frameCount).toStdString().c_str(), cvPoint(650, 20),
                        //  &font, cvScalar(255, 255, 0));
                if(getBBFromUser(img, boxUser, gui, trackerId, message) == PROGRAM_EXIT) return;
                CvRect *add = rectangles[trackerId];
                add->x = boxUser.x;
                add->y = boxUser.y;
                add->width = boxUser.width;
                add->height = boxUser.height;

                trackers[trackerId]->reinitialize(ccv_tld_params);
             }
          }
      }

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

void Main::initGui(QTextEdit* analytics, int videoX, int videoY, int graphWidth, int graphHeight, int graphX, int graphY){

    this->analytics = analytics;

    IplImage* orig = cvLoadImage("./TLD.app/Contents/Resources/graph.jpg");   // Read the file
    //cvSetImageROI(orig, cvRect(0, 0, graphWidth, graphHeight));
    //graphImage = cvCreateImage(cvGetSize(orig), orig->depth, orig->nChannels);
    //cvCopy(orig, graphImage, NULL);
    //cvResetImageROI(orig);
    //cvReleaseImage(&orig);

    graphImage = (IplImage *)cvClone(orig);


    this->graphHeight = graphHeight;
    this->graphWidth = graphWidth;
    imAcqInit(imAcq);
    gui->initVideoWindow(videoX, videoY);
    gui->initGraphWindow(graphWidth, graphHeight, graphX, graphY);

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
