#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QInputDialog>
#include <QLineEdit>
#include <fstream>
#include <opencv/cv.h>
#include <sys/time.h>
#include <ctype.h>
#include <sstream>
#include "Main.h"
#include "Gui.h"
#include "Settings.h"

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

/* Function: track(Settings* settings)
 * ----------------------------------
 * Loops through each frame in the trackImagesPath directory and allows users to add, reinitialize, and delete trackers.
 * Saves results to csv files.
 * */

bool Main::track(Settings* settings) {

    aWin->append("Use the following keys to direct Simba:\n'p' : play/pause\n'a' : add a tracker\n'q' : quit");

    aWin->append("\nWhen one of the trackers cannot find its patch in a frame, Simba will stop. If you wish to delete the tracker hit 'd' and then the trackerId. If you wish to reinitialize the tracker hit 'r' and then the trackerId. Hit 'p' when finished.\n\n");
    aWin->insertHtml("List of Available Trackers & Tracker IDs");
    bool ok;

    //Loads first image
    std::string imagePath = getImagePath(frameCount, trackImagesPath);
    img = cvLoadImage(imagePath.c_str());
    if (!img)
        return 2;

    //Sets up gui internal struct
    gui->setWidthHeight(img->width, img->height);

    //Saves first image
    std::string saveImagePath = getImagePath(frameCount, resultsDirectory);
    cvSaveImage(saveImagePath.c_str(), img);

    //Displays first image
    gui->showImage(img);

    //libccv cache (see documentation)
    ccv_enable_default_cache();

    //Read in the first image into "x". Both "x" and "y" will refer to frames n and n+1 respectively and will be updated with every loop.
    ccv_read(imagePath.c_str(), &x, CCV_IO_GRAY | CCV_IO_ANY_FILE);

    char key = 0;
    bool quit = false;

    while (key != 'p'){
        key = gui->getKey();
        int ret = userAction(key);
        if (ret == 0) {
            quit = true;
            break;
        }
    }

    while(!quit)
    {
        //free memory from current image
        cvReleaseImage(&img);

         //load next image
        frameCount++;
        imagePath = getImagePath(frameCount, trackImagesPath);
        img = cvLoadImage(imagePath.c_str());
        if (!img)
            break;

        //read next image into "y"
        ccv_read(imagePath.c_str(), &y, CCV_IO_GRAY | CCV_IO_ANY_FILE);

        //release semaphores (the tracker threads now determine where the tracked object is in curr frame)
        for (int i = 0; i < numTrackers; i++){
          if (trackers[i] != NULL)
                trackerSems[i]->release();
        }

        //acquire semaphores effectively waiting for all trackers to complete work on the curr frame
        for (int i = 0; i < numTrackers; i++){
            if (trackers[i] != NULL)
                mainSems[i]->acquire();
        }

        //will be set to true if a tracker could not find its object in the current frame
        bool lost = false;

        //Draws rectangles on the current image around the detected objects
        for (int i = 0; i < numTrackers; i++){

            if (trackers[i] != NULL){
                if (trackers[i]->found()) {

                    struct colors* c = groupColors[trackersToGroupMap[i]];
                    CvScalar rectangleColor = CV_RGB(c->r, c->g, c->b);
                    CvPoint topLeft = cvPoint(rectangles[i]->x, rectangles[i]->y);
                    CvPoint bottomRight = cvPoint(rectangles[i]->x + rectangles[i]->width, rectangles[i]->y + rectangles[i]->height);
                    cvRectangle(img, topLeft, bottomRight, rectangleColor, 1, 4, 0);
                    std::string name = idToGroupName.at(trackersToGroupMap[i]) + ": " + idToTrackerName.at(i) + " (" + QString::number(i).toStdString() + ")";
                    cvPutText(img, name.c_str(), cvPoint(topLeft.x, topLeft.y - 2), &font, rectangleColor);
                    QString s = QString::number(rectangles[i]->x) + "," + QString::number(rectangles[i]->y) + "," + QString::number(rectangles[i]->width) + "," + QString::number(rectangles[i]->height);
                    fprintf(files[i], "\n%s", s.toStdString().c_str());

               } else {
                    lost = true;
                    fprintf(files[i], "\n%s", "0,0,0,0");
                }
            }

        }

        //Free x and update x and y
        ccv_matrix_free(x);
        x = y;
        y = 0;

        //Saves image with rectangles in ResultsDirectory
        saveImagePath = getImagePath(frameCount, resultsDirectory);
        cvSaveImage(saveImagePath.c_str(), img);

        //Show image
        gui->showImage(img);

        key = 0;

        //If a track is lost, we stop and let the user reinitialize trackers with keyboard commands

        if (lost){

            do {
                key = gui->getKey();
                if (userAction(key) == 0) {
                    quit = true;
                    break;
                }
            } while (key != 'p');

        }

        if (quit == true) break;

        //Lets user add a tracker, quit the program, or play/pause with keyboard command

        //Gets key from user
        key = gui->getKey();
        int ret = userAction(key);
        if (ret == 0) {
            quit = true;
            break;
        }

        //Ret is 1 if user hit a, d, r and is 2 if user hit p
        if (ret == 1 || ret == 2){
            while (true){
                key = gui->getKey();
                 ret = userAction(key);
                if (ret == 0) {
                    quit = true;
                    break;
                }
                if (ret == 2) break;
            }
        }

    }

    ccv_matrix_free(x);
    ccv_disable_cache();
    writeSettingsToFile();
    return true;
}

/* Function: userAction(char key)
 * -----------------------------
 * Adds, deletes, reinitializes trackers, pauses/plays, or returns a marker to quit based on user input
 */

int Main::userAction(char key){

    if (key == 'a'){
        initializeTracking(img, ccv_tld_params);
        return 1;
    }

    if (key == 'd'){
        bool ok;
        QWidget w;
        QString number = QInputDialog::getText(&w, QString("Tracker to delete"),QString("Enter tracker number:"), QLineEdit::Normal,"", &ok);
        int trackerToDelete = number.toInt();
        if (ok) deleteTracker(trackerToDelete);
        return 1;
    }

    if (key == 'q'){
        return 0;
    }

    if (key == 'r'){

        char num = key;
        while (!isdigit(num)){
            num = gui->getKey();
        }

        std::stringstream s;
        s << num;
        string numString;
        s >> numString;
        int trackerNum = QString(numString.c_str()).toInt();

        IplImage *img0 = (IplImage *) cvClone(img);
        gui->showImage(img0);

        std::string message = string("Tracker number: ") + num + string(". Frame number: ") + QString::number(frameCount).toStdString() + string(". Draw bounding box and press enter");
        if(getBBFromUser(img, *rectangles[trackerNum], gui, num, message) == 0) return 0;

        if (trackerNum < numTrackers && trackerNum >= 0 && trackers[trackerNum] != NULL)
            trackers[trackerNum]->reinitialize(ccv_tld_params);

        gui->showImage(img);
        cvReleaseImage(&img0);
        return 1;

    }

    if (key == 'p')
        return 2;

    return 3;

}

/* Function: createTLD()
 * ---------------------
 * Given that all required TLD information has been appended on necessary data structures (i.e. startFrames,
 * endFrames, rectangles), this creates the TLD object as well as related semaphores for thread features.
 * If required, it also updates the ini file.
 */

bool Main::createTLD(long startFrame, long endFrame, CvRect *rect, const ccv_tld_param_t ccv_tld_params, int trackerId){

    //Get group and tracker information
    bool ok = false;
    QWidget w;
    QString groupName = QInputDialog::getText(&w, QString("Group Name. No '#' or '-'"),QString("Enter group name:"), QLineEdit::Normal,"", &ok);
    if (!ok) return false;
    int groupNumber;

    QString trackerName = QInputDialog::getText(&w, QString("Tracker Name (no '#' or '-')"),QString("Enter tracker name:"), QLineEdit::Normal,"", &ok);
    if (!ok) return false;

    //Error checking
    if (trackerNameToId.find(trackerName.toStdString()) != trackerNameToId.end()){
        trackerName = QInputDialog::getText(&w, QString("Tracker Name exists. (no '#' or '-')"),QString("Enter a new tracker name:"), QLineEdit::Normal,"", &ok);
        if (!ok) return false;
    }

    //Add tracker information and create semaphores
    addTrackerInfo(startFrame, endFrame, rect, trackerName.toStdString(), trackerId);
    trackerSems.push_back(new QSemaphore());
    mainSems.push_back(new QSemaphore());

    //Create tracker
    Tracker* t = new Tracker(frameCount, trackerId, trackImagesPath, trackerSems[trackerId], mainSems[trackerId], rectangles[trackerId], startFrames[trackerId], endFrames[trackerId], ccv_tld_params, &x, &y);
    trackers.push_back(t);

    if (groupNameToId.find(groupName.toStdString()) == groupNameToId.end()){
        groupNumber = numGroups;
        groupNameToId.insert(std::pair<std::string,int>(groupName.toStdString(),numGroups));
        idToGroupName.insert(std::pair<int, std::string>(numGroups, groupName.toStdString()));
        idToGroupName[numGroups] = groupName.toStdString(); //for an odd reason, this line is needed (the line above fails to record the first group)
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
            c->r = 225 - (groupNumber%10)*15;
            c->g = 224 - (groupNumber%10)*15;
            c->b = 115 - (groupNumber%10)*10;
        }

        groupColors.push_back(c);
        numGroups++;
    } else {
        groupNumber = groupNameToId.at(groupName.toStdString());
    }

    std::string filename = resultsDirectory + groupName.toStdString() + trackerName.toStdString() + ".csv";
    FILE* file = fopen(filename.c_str(), "w");
    files.push_back(file);

    for (int i = 0; i < frameCount - 1; i++){
        if (i == 0)
            fprintf(file, "%s", "0,0,0,0");
        else
            fprintf(file, "\n%s", "0,0,0,0");
    }

    QString s = QString::number(rect->x) + "," + QString::number(rect->y) + "," + QString::number(rect->width) + "," + QString::number(rect->height);
    fprintf(file, "\n%s", s.toStdString().c_str());

    trackersToGroupMap[trackerId] = groupNumber;
    trackersPerGroup[groupNumber].push_back(trackerId);
    numTrackers++;
    aWin->append(QString(idToTrackerName[trackerId].c_str()) + QString(": ") + QString::number(trackerId));

    //Start the tracker thread
    t->start();
    return true;

}

/* Function: addTrackerInfo()
 * ---------------------
 * Appends required information for tracker at the end of three data structures: startFrames, endFrames,
 * and rectangles. This is only modified and accessed by the main thread.
 */

void Main::addTrackerInfo(long startFrame, long endFrame, CvRect* add, std::string trackerName, int trackerId){

    startFrames.push_back(startFrame);
    endFrames.push_back(endFrame);
    rectangles.push_back(add);
    trackerNameToId.insert(std::pair<std::string,int>(trackerName,trackerId));
    idToTrackerName.insert(std::pair<int, std::string>(trackerId, trackerName));

}

/* Function: initializeTracking()
 * ------------------------------
 * Tracking is initialized on the current image. The user is allowed to add
 * a new tracker.
 */

void Main::initializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params){

    //tracker id
    int trackerId = numTrackers;

    //Image on which we are initializing trackers
    IplImage *img0 = (IplImage *) cvClone(img);
    gui->showImage(img0);

    //Get rectangle from the user on what to initialize
    std::string message = string("Tracker number: ") + QString::number(trackerId).toStdString() + string(". Frame number: ") + QString::number(frameCount).toStdString() + string(". Draw a bounding box and press enter");
    CvRect boxUser;
    if(getBBFromUser(img, boxUser, gui, trackerId, message) == 0) return;
    CvRect *add = new CvRect;
    add->x = boxUser.x;
    add->y = boxUser.y;
    add->width = boxUser.width;
    add->height = boxUser.height;

    //Create tld
    if (!createTLD(frameCount, -1, add, ccv_tld_params, trackerId))
        delete add;

    //show original image
    gui->showImage(img);

    cvReleaseImage(&img0);
}

/* Function: deleteTracker(int i)
 * ------------------------------
 * Deletes tracker i by terminating the thread and deleting its semaphores, rectangle, and other associated memory.
 */

void Main::deleteTracker(int i){

    if (trackers[i] != NULL && i < numTrackers){
        trackers[i]->trackingComplete();
        trackerSems[i]->release();
        trackers[i]->wait();
        delete trackers[i];
        delete trackerSems[i];
        delete mainSems[i];
        delete rectangles[i];
        fclose(files[i]);
        trackers[i] = NULL;
    }

}

/* Function: deleteTrackers()
 * -------------------------
 * Deletes trackers and their color structs.
 */

void Main::deleteTrackersAndGroups(){

    for (int i = 0; i < numTrackers && trackers[i] != NULL; i++){
        deleteTracker(i);
    }

    for (int i = 0; i < groupColors.size(); i++){
        delete groupColors[i];
    }

}

/* Function: initGui(int videoX, int videoY)
 * ----------------------------------------
 * Initializes the gui with the x/y coordinates of the video.
 */

void Main::initGui(int videoX, int videoY){

    gui->initVideoWindow(videoX, videoY);

}

/* Function: writeSettingsToFile()
 * -----------------------------------------
 * Writes group settings, colors, and which tracker belongs to which group
 * to a ini file, easily read by Qt
 */

void Main::writeSettingsToFile(){

    std::string settingsFileName = resultsDirectory + "groupSettings.ini";
    FILE* file = fopen(settingsFileName.c_str(), "w");
    fprintf(file, "%s\nFrameCount=%ld", "[Info]", frameCount);
    fprintf(file, "\n%s\nNames=", "[GroupNames]");
    for (int i = 0; i < numGroups; i++){
       fprintf(file, "%s ", idToGroupName[i].c_str());
    }

    for (int i = 0; i < numGroups; i++){
        fprintf(file, "\n[%s]", idToGroupName[i].c_str());
        fprintf(file, "\nRed=%d", groupColors[i]->r);
        fprintf(file, "\nGreen=%d", groupColors[i]->g);
        fprintf(file, "\nBlue=%d", groupColors[i]->b);
        fprintf(file, "\n%s", "Trackers=");
        for (int j = 0; j < trackersPerGroup[i].size(); j++){
            fprintf(file, "%s ", idToTrackerName[(trackersPerGroup[i])[j]].c_str());
        }
    }

    fclose(file);

}

/* Function: getImagePath(long frame, std::string p)
 * -------------------------------------------------
 * Gets the image path of image with frame number frame located in directory p
 */

std::string Main::getImagePath(long frame, std::string p){

    std::string imagePath = p;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%07ld", frame);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");
    return imagePath;

}

