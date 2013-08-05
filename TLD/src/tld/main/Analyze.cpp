#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QInputDialog>
#include <QLineEdit>
#include <fstream>
#include "Analyze.h"
#include "Config.h"
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

//note frame initialiezd on is startFrame - 1

void Analyze::getImage(long frame, std::string saveImage, int x, int y, int width, int height){

    std::string imagePath = resultsDirectory;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%07ld", frame);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");

    //IplImage *img = cvLoadImage(imagePath.c_str());
    //cv::Mat imgMat(img);

    cv::Mat imgMat = imread(imagePath, 1);
    cv::Mat imgROI;
    imgROI = imgMat(Rect(x, y, width, height));

    imwrite(saveImage, imgROI);


}

long Analyze::startFrame(int trackerId){

    long sframe = 1;

    std::vector< std::string > *v = &trackerResults[trackerId];

    for (int i = 0; i < v->size(); i++){
        std::cout << v->at(i) << std::endl;
        if (v->at(i).compare("-") != 0)
            break;
        sframe++;
    }

    return sframe;

}

bool Analyze::trackerInfo(std::string trackerName){

    int trackerId = trackerNameToId[trackerName];

    double percentTotalFramesTrackerAppeared = 0;
    long countDetected = 0;

    std::vector< std::string > *v = &trackerResults[trackerId];

    //careful with percentages because the first frame is never tracked. be careful with all frame stuff here, go over it

    for (int i = 1; i < v->size(); i++){
        std::cout << v->at(i) << std::endl;
        if (v->at(i).compare("-") == 0)
            continue;
        countDetected++;
    }

    percentTotalFramesTrackerAppeared = (double)countDetected/v->size();
    long framesTracked = v->size() - startFrame(trackerId);
    double percentFramesTracked = (double)countDetected/framesTracked;

    //Output: assuming 24 fps you can do some time based thing
    //video is not proper time based

    std::string groupNameFormatted = "<b>" + trackerName + "</b>";
    std::ostringstream s;
    s << percentTotalFramesTrackerAppeared;
    aWin->insertHtml(QString("<br><br>percent: ") + QString(s.str().c_str()));



    return true;


}

bool Analyze::groupInfo(std::string groupName){

    int groupId = groupNameToId[groupName];
    std::string groupNameFormatted = "<b>" + groupName + "</b>";

    aWin->insertHtml(QString(groupNameFormatted.c_str()));
    std::string listOfTrackers;
    for (int i = 0; i < trackersPerGroup[groupId].size(); i++){
        listOfTrackers = listOfTrackers + std::string("<br>") + idToTrackerName[trackersPerGroup[groupId][i]] + ":" + QString::number(startFrame(trackersPerGroup[groupId][i])).toStdString();
    }
    aWin->insertHtml(QString(listOfTrackers.c_str()));

    std::string imageNewName = resultsDirectory + "helloWorld.png";
    long frame = 2;
    getImage(frame, imageNewName, 0, 0, 100, 100);

    aWin->insertHtml(QString("<img src=\"") + QString(imageNewName.c_str()) + QString("\">"));

    return true;


}

std::string Analyze::getCommand(){

    QString info = aWin->toHtml();
    if (info.size() < 1) return "";
    std::cout << "making string list" << std::endl;
    QStringList commands = info.split(">");
    if (commands.size() < 1) return "";
    std::cout << "getting command" << std::endl;
    QString command = commands[commands.size() - 1];
    std::cout << "returning" << std::endl;
    return command.toStdString();

}

bool Analyze::doWork() {

    std::string iniPath = resultsDirectory + "groupSettings.ini";
    std::ifstream infile(iniPath.c_str());
    QSettings* settingsIn = new QSettings(QString(iniPath.c_str()), QSettings::IniFormat);
    if (!infile || settingsIn->status() != QSettings::NoError){
        std::cerr << "Failed to open initialization file\n" << std::endl;
        return false;
    }

    settingsIn->beginGroup("Info");
    //is it ok that this is to long long instead of just long
    frameCount = settingsIn->value("FrameCount").toLongLong();
    settingsIn->endGroup();

    settingsIn->beginGroup("GroupNames");
    QString groupsString = settingsIn->value("Names").toString();
    settingsIn->endGroup();

    std::cout << groupsString.toStdString() << std::endl;
    QStringList groupsStringList = groupsString.split(' ');

    for (int i = 0; i < groupsStringList.size(); i++){
        std::cout << groupsStringList[i].toStdString() << std::endl;
        groupNameToId.insert(std::pair<string, int>(groupsStringList[i].toStdString(), i));
        idToGroupName.insert(std::pair<int, string>(i, groupsStringList[i].toStdString()));
        settingsIn->beginGroup(groupsStringList[i].toStdString().c_str());
        int red = settingsIn->value("Red").toInt();
        int green = settingsIn->value("Green").toInt();
        int blue = settingsIn->value("Blue").toInt();
        settingsIn->endGroup();
        struct colors *c = new struct colors;
        c->r = red;
        c->g = green;
        c->b = blue;
        groupColors.push_back(c);
        std::string groupFilePath = resultsDirectory + groupsStringList[i].toStdString() + ".txt";
        QFile file(groupFilePath.c_str());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
        QTextStream in(&file);
        QString text;
        text = in.readAll();
        file.close();
        QStringList trackersFromFile = text.split("#");
        std::vector<int> trackerNumbersInGroup;
        for (int i = 1; i < trackersFromFile.size(); i+=2){
            trackerNameToId.insert(std::pair<string, int>(trackersFromFile[i].toStdString(), numTrackers));
            idToTrackerName.insert(std::pair<int, string>(numTrackers, trackersFromFile[i].toStdString()));
            QString trackerResultsString = trackersFromFile[i+1];
            QStringList trackerResultsPerFrame = trackerResultsString.split('\n');
            std::vector< std::string > v;
            for (int j = 1; j < trackerResultsPerFrame.size(); j++){
                v.push_back(trackerResultsPerFrame[j].toStdString());
            }
            trackerResults.push_back(v);
            trackerNumbersInGroup.push_back(numTrackers);
            trackersToGroupMap.insert(std::pair<int, int>(numTrackers, numGroups));
            numTrackers++;
        }
        trackersPerGroup.push_back(trackerNumbersInGroup);
        numGroups++;
    }


    bool play = true;
    long currFrame = 0; //make 0 once you add the first frame to the folder
    std::string imagePath;
    char num[64];

    aWin->append(">");
    std::cout << "appended the >" << std::endl;

    std::string command;

        while (play){
            currFrame++;
            imagePath = resultsDirectory;
            memset(num, 0, 64);
            sprintf(num, "%07ld", currFrame);
            imagePath += QString(num).toStdString();
            imagePath += std::string(".png");

            cv::Mat img = imread(imagePath.c_str(), 1);
            cv::Mat imgResized;
            resize(img, imgResized, Size(), fx, fy, CV_INTER_AREA);
            //do error checking here
            analyzeGui->showImage(imgResized, mainWindowName);
            if (currFrame == 10)currFrame = 1;
        }







   return true;



}

Analyze::~Analyze(){

}

void Analyze::initGui(int mainVideoX, int mainVideoY, int secondaryVideoX, int secondaryVideoY, std::string mainWindowName, std::string secondaryWindowName){

    analyzeGui->initVideoWindow(mainVideoX, mainVideoY, secondaryVideoX, secondaryVideoY, mainWindowName, secondaryWindowName);

}

void Analyze::debugAnalyze(){

    std::cout << "Printing id to tracker name" << std::endl;

    for (int i = 0; i < idToTrackerName.size(); i++){
        std::string s = idToTrackerName.at(i);
        std::cout << QString::number(i).toStdString() + ":" + s << std::endl;
        std::cout << QString::number(trackerNameToId[s]).toStdString() + ":" + s << std::endl;
    }



      std::cout << "Printing  id to groupname" << std::endl;

    for (int i = 0; i < idToGroupName.size(); i++){
        std::string p = idToGroupName.at(i);
        std::cout << QString::number(i).toStdString() + ":" + p << std::endl;
        std::cout << QString::number(groupNameToId[p]).toStdString() + ":" + p << std::endl;
    }


    std::cout << "Printing  trackers to groupmap" << std::endl;

  for (int i = 0; i < trackersToGroupMap.size(); i++){
      int x = trackersToGroupMap.at(i);
      std::cout << QString::number(i).toStdString() + ":" + QString::number(x).toStdString() << std::endl;
  }

std::cout << "Printing  trackers per group" << std::endl;

  for (int i = 0; i < trackersPerGroup.size(); i++){
      std::cout << QString::number(i).toStdString() << std::endl;
      for (int j = 0; j < trackersPerGroup[i].size(); j++){
          std::cout << QString::number(trackersPerGroup[i][j]).toStdString() << std::endl;
      }
  }

  std::cout << "Printing tracker results" << std::endl;

  for (int i = 0; i < trackerResults.size(); i++){

      std::cout << QString::number(i).toStdString() << std::endl;
      for (int j = 0; j < trackerResults[i].size(); j++){
          std::cout << trackerResults[i][j] << std::endl;
      }

  }

}

