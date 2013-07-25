/* Lion: Libccv's TLD ("Predator") with GUI
 * ----------------------------
 * Last modified by: Anshul Samar.
 *
 *
 */

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
 * Constructs a tracker object as well as initializing av related structs and creating
 * output file. For more details, see libccv's tld.c implementation. Note to developer:
 * main calls the constructor with frameCount, startFrame, and endFrame. frameCount is the
 * frame number the main process is currently on. startFrame is the frame on which the TLD
 * object should be initialized. We thus move the video to the start frame and initialize
 * TLD object before main even reaches that part of the video.
 */

Tracker::Tracker(int frameCount, int trackerId, std::string videoPath, QSemaphore* trackerSemaphore, QSemaphore *mainSemaphore, CvRect *rect, int startFrame, int endFrame, const ccv_tld_param_t ccv_tld_params, bool saveResults, std::string resultsDirectory, QTextEdit* analytics, QMutex* textMutex)
{

    this->x = 0;
    this->y = 0;
    this->done = false;
    this->ccvtld = NULL;
    this->frameCount = frameCount;
    this->trackerId = trackerId;
    this->rect = rect;
    this->trackerSemaphore = trackerSemaphore;
    this->mainSemaphore = mainSemaphore;
    this->startFrame = startFrame;
    this->endFrame = endFrame;
    this->saveResults = saveResults;
    this->resultsDirectory = resultsDirectory;
    this->analytics = analytics;
    this->textMutex = textMutex;
    file = NULL;

    if (saveResults == true){
        string name = "/tld";
        std::ostringstream numberString;
        numberString << trackerId;
        if (resultsDirectory[resultsDirectory.size() - 1] == '/')
            name = string("tld");
        string filename = resultsDirectory + string("/tld") + numberString.str() + string(".txt");
        file = fopen(filename.c_str(), "w");
        if (file == NULL){
            textMutex->lock();
            analytics->append("Error in opening new file to write. Continuing without saving results.");
            saveResults = false;
            textMutex->unlock();
        }
    }

    // init av-related structs
    ic = 0;
    int video_stream = -1;
    video_st = 0;
    picture = 0;
    //rgb_picture;
    memset(&rgb_picture, 0, sizeof(AVPicture));
    //packet;
    memset(&packet, 0, sizeof(AVPacket));
    av_init_packet(&packet);
    av_register_all();
    avformat_network_init();
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
    picture_ctx = sws_getCachedContext(0, video_st->codec->width, video_st->codec->height, video_st->codec->pix_fmt, video_st->codec->width, video_st->codec->height, PIX_FMT_RGB24, SWS_BICUBIC, 0, 0, 0);
    sws_scale(picture_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, video_st->codec->height, rgb_picture.data, rgb_picture.linesize);

    for (int i = 1; i < frameCount; i++){
        got_picture = 0;
        int result = av_read_frame(ic, &packet);
        if (result == AVERROR(EAGAIN))
            continue;
        avcodec_decode_video2(video_st->codec, picture, &got_picture, &packet);
        if (!got_picture)
            break;
        sws_scale(picture_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, video_st->codec->height, rgb_picture.data, rgb_picture.linesize);
    }

    for (int i = frameCount; i < startFrame; i++){
        got_picture = 0;
        int result = av_read_frame(ic, &packet);
        if (result == AVERROR(EAGAIN))
            continue;
        avcodec_decode_video2(video_st->codec, picture, &got_picture, &packet);
        if (!got_picture)
            break;
        sws_scale(picture_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, video_st->codec->height, rgb_picture.data, rgb_picture.linesize);
    }

   ccv_read(rgb_picture.data[0], &x, CCV_IO_RGB_RAW | CCV_IO_GRAY, video_st->codec->height, video_st->codec->width, rgb_picture.linesize[0]);
   ccv_rect_t ccvBox = ccv_rect(rect->x, rect->y, rect->width, rect->height);
   ccvtld = ccv_tld_new(x, ccvBox, ccv_tld_params);

}

/* Function: ~Tracker()
 * ---------------------
 * Destructor implementation.
 */

Tracker::~Tracker(){

    ccv_matrix_free(x);
    ccv_tld_free(ccvtld);
    ccfree(rgb_picture.data[0]);
    ccv_disable_cache();
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
        if (frameCount <= startFrame || (endFrame != -1 && frameCount > endFrame)){
            ccvtld->found = false;
            if (saveResults)
                fprintf(file, "Tracker %02d %05d: --------------\n", trackerId, frameCount);
            //printf("Tracker %02d %05d: --------------\n", trackerId, frameCount);
            //fflush(stdout);
            mainSemaphore->release();
            continue;

        }
        got_picture = 0;
        int result = av_read_frame(ic, &packet);
        if (result == AVERROR(EAGAIN))
            continue;
        avcodec_decode_video2(video_st->codec, picture, &got_picture, &packet);
        if (!got_picture)
            break;
        sws_scale(picture_ctx, (const uint8_t* const*)picture->data, picture->linesize, 0, video_st->codec->height, rgb_picture.data, rgb_picture.linesize);
        ccv_read(rgb_picture.data[0], &y, CCV_IO_RGB_RAW | CCV_IO_GRAY, video_st->codec->height, video_st->codec->width, rgb_picture.linesize[0]);
        ccv_tld_info_t info;

        ccv_comp_t newbox = ccv_tld_track_object(ccvtld, x, y, &info);
        rect->x = newbox.rect.x;
        rect->y = newbox.rect.y;
        rect->width = newbox.rect.width;
        rect->height = newbox.rect.height;

        if (ccvtld->found){
            if (saveResults)
                fprintf(file, "Tracker %02d %05d: %d %d %d %d %f\n", trackerId, frameCount, newbox.rect.x, newbox.rect.y, newbox.rect.width, newbox.rect.height, newbox.confidence);
            //printf("Tracker %02d %05d: %d %d %d %d %f\n", trackerId, frameCount, newbox.rect.x, newbox.rect.y, newbox.rect.width, newbox.rect.height, newbox.confidence);
        }
        else {
            if (saveResults)
                fprintf(file, "Tracker %02d %05d: --------------\n", trackerId, frameCount);
            //printf("Tracker %02d %05d: --------------\n", trackerId, frameCount);
        }
        //fflush(stdout);
        x = y;
        y = 0;
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

/* Function: reinititialize() (currently not used or fully tested)
 * --------------------------
 * Reinitializes the tracker object based on the current frame and the rectangle
 * coordinates (the CvRect pointed to by rect should be updated by the function
 * that calls reinitialize).
 */

void Tracker::reinitialize(const ccv_tld_param_t ccv_tld_params){
    ccv_matrix_free(x);
    ccv_tld_free(ccvtld);
    x = 0;
    y = 0;
    ccv_read(rgb_picture.data[0], &x, CCV_IO_RGB_RAW | CCV_IO_GRAY, video_st->codec->height, video_st->codec->width, rgb_picture.linesize[0]);
    ccv_rect_t ccvBox = ccv_rect(rect->x, rect->y, rect->width, rect->height);
    ccvtld = ccv_tld_new(x, ccvBox, ccv_tld_params);
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
