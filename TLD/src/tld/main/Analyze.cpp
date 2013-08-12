#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QInputDialog>
#include <QLineEdit>
#include <fstream>
#include "Analyze.h"
#include "AnalyzeGui.h"
#include "Config.h"
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

using namespace cv;

//change these to doubles

void Analyze::initGui(int oneX, int oneY, int twoX, int twoY, int intelX, int intelY){

    analyzeGui->initGui(oneX, oneY, twoX, twoY, intelX, intelY, oneName, twoName, intelName);

}

std::string Analyze::getImagePath(long frame){

    std::string imagePath = analysisImagesPath;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%07ld", frame);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");
    return imagePath;

}

void Analyze::saveImage(long frame, std::string saveImagePath, bool crop, int x, int y, int width, int height){

    std::string imagePath = analysisImagesPath;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%07ld", frame);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");

    cv::Mat imgMat = imread(imagePath, 1);
    cv::Mat imgROI = imgMat;
    if (crop)
        imgROI = imgMat(Rect(x, y, width, height));
    imwrite(saveImagePath, imgROI);

}

long Analyze::getStartFrame(int trackerId){

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

bool Analyze::isGroup(std::string group){

    return groupNameToId.find(group) != groupNameToId.end();

}

bool Analyze::isTracker(std::string tracker){

    return trackerNameToId.find(tracker) != trackerNameToId.end();

}

void Analyze::getStats(){

    for (int g = 0; g < numGroups; g++){
       int groupId = g;

       for (int i = 0; i < trackersPerGroup[groupId].size(); i++){
            int trackerId = trackersPerGroup[groupId][i];
            std::vector< std::string >& result = trackerResults[trackerId];
            long starting;
            long ending;
            bool tracking = false;
            for (long j = 0; j < endFrames[trackerId]; j++){
                std::cout << QString::number(j).toStdString() << std::endl;
                QStringList frame = QString(result[j].c_str()).split(QString(" "));
                std::string first = frame[0].toStdString();

                if (first.compare("-") != 0){
                    if (!tracking){
                        starting = j + 1;
                    }
                    tracking = true;
                      if (!groupAppearances[groupId][j]){
                        groupCounts[groupId]++;
                        groupAppearances[groupId][j] = true;
                    }
                    trackerAppearances[trackerId][j] = true;
                    trackerCounts[trackerId]++;

                } else {
                    if (tracking){
                    ending = j + 1;
                    vector<long>& l = startStopFrames[trackerId];
                    l.push_back(starting);
                    l.push_back(ending);
                    tracking = false;
                    }
                }
            }
        }
    }

    for (int t = 0; t < numTrackers; t++){
        startFrames.push_back(getStartFrame(t));
    }



}

QStringList Analyze::getCommand(){

    QString info = aWin->toPlainText();
    QStringList commands = info.split("$ ");
    QString command = commands[commands.size() - 1];
    return command.split(" ");
}

bool Analyze::parse(){

    std::string iniPath = analysisImagesPath + "groupSettings.ini";
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
    QStringList groupsStringList = groupsString.split(' ');

    for (int i = 0; i < groupsStringList.size(); i++){
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
        std::string groupFilePath = analysisImagesPath + groupsStringList[i].toStdString() + ".txt";
        QFile file(groupFilePath.c_str());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
        QTextStream in(&file);
        QString text;
        text = in.readAll();
        file.close();
        QStringList trackersFromFile = text.split("#");
        std::vector<int> trackerNumbersInGroup;
        for (int i = 1; i < trackersFromFile.size(); i+=2){
            std::string trackerName = trackersFromFile[i].toStdString().substr(1, trackersFromFile[1].size() - 2);

            trackerNameToId.insert(std::pair<string, int>(trackerName, numTrackers));
            idToTrackerName.insert(std::pair<int, string>(numTrackers, trackerName));
            QString trackerResultsString = trackersFromFile[i+1];
            QStringList trackerResultsPerFrame = trackerResultsString.split('\n');
            std::vector< std::string > v;
            for (int j = 1; j < trackerResultsPerFrame.size(); j++){
                v.push_back(trackerResultsPerFrame[j].toStdString());
            }
            trackerResults.push_back(v);
            endFrames.push_back(trackerResultsPerFrame.size() - 1);
            trackerNumbersInGroup.push_back(numTrackers);
            trackersToGroupMap.insert(std::pair<int, int>(numTrackers, numGroups));
            numTrackers++;
        }
        trackersPerGroup.push_back(trackerNumbersInGroup);
        numGroups++;
    }

    for (int i = 0; i < numTrackers; i++){

        std::vector<bool> v(frameCount);
        trackerAppearances.push_back(v);
        trackerCounts.push_back(0L);
        std::vector<long> l;
        startStopFrames.push_back(l);
    }

    for (int i = 0; i < numGroups; i++){

        std::vector<bool> v(frameCount);
        groupAppearances.push_back(v);
        groupCounts.push_back(0L);
    }


    getStats();

    return true;

}

void Analyze::fillTotalAppearances(bool group, std::vector <std::string>& list, std::vector<bool> &totalAppearances){

    if (group){
        for (int i = 0; i < list.size(); i++){
            std::vector<bool> &g = groupAppearances[groupNameToId[list[i]]];
            for (long i = 0; i < g.size(); i++){
                if (g[i])
                    totalAppearances[i] = true;
            }
        }
    } else {
        for (int i = 0; i < list.size(); i++){
            std::vector<bool> &t = trackerAppearances[trackerNameToId[list[i]]];
            for (long i = 0; i < t.size(); i++){
                if (t[i])
                    totalAppearances[i] = true;
            }
        }
    }


}

void Analyze::show(QStringList& command){

    int trackerId = trackerNameToId[command[1].toStdString()];
    std::vector<bool>& appearances = trackerAppearances[trackerId];
    std::vector< std::string >& result = trackerResults[trackerId];
    std::string imagePath;
    char num[64];

    for (long i = startFrames[trackerId]; i < endFrames[trackerId]; i++){

        QStringList frame = QString(result[i].c_str()).split(QString(" "));
        cv::Mat copy = twoImgCrop;

        imagePath = analysisImagesPath;
        memset(num, 0, 64);
        sprintf(num, "%07ld", i);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");

        cv::Mat img = imread(imagePath.c_str(), 1);
        cv::Mat imgResized;
        resize(img, imgResized, Size(), fxSmallVideo, fySmallVideo, CV_INTER_AREA);


        cv::Rect roi( cv::Point( frame[0].toInt(), frame[1].toInt() ), cv::Size( frame[2].toInt(), frame[3].toInt() ));
        imgResized = imgResized( roi );


        double x = smallVideoWidth/2 - frame[2].toInt()/2;
        double y = smallVideoHeight/2 - frame[3].toInt()/2;

        cv::Rect loc( cv::Point( x, y ), cv::Size( frame[2].toInt(), frame[3].toInt() ));
        cv::Mat regionOfInterest = copy( loc );
        imgResized.copyTo( regionOfInterest );


    }


}

void Analyze::play(QStringList& command){

    std::vector <std::string> list;
    bool groups = false;
    bool trackers = false;
    std::vector<bool> totalAppearances(frameCount);

    long startFrame = 1;
    long endFrame = frameCount;

    if (command[1].compare("start") && command[3].compare("end")){
        startFrame = command[2].toLong();
        endFrame = command[4].toLong();
    } else if (command[1].compare("groups")){
        for (int i = 2; i < command.size(); i++){
            list.push_back(command[i].toStdString());
        }
        fillTotalAppearances(true, list, totalAppearances);
        groups = true;
    } else if (command[1].compare("trackers")){
        for (int i = 2; i < command.size(); i++){
            list.push_back(command[i].toStdString());
        }
        fillTotalAppearances(false, list, totalAppearances);
        trackers = true;
    }

    int numGraphLines;

    if (list.size() > 3){
        numGraphLines = 3;
    } else
        numGraphLines = list.size();



    int xCoord = 20;

    std::string imagePath;
    char num[64];

    if (groups || trackers)
        while (!totalAppearances[startFrame - 1])
            startFrame++;

    while (startFrame < endFrame) {

        if ((!groups && !trackers) || totalAppearances[startFrame - 1]){
            imagePath = analysisImagesPath;
            memset(num, 0, 64);
            sprintf(num, "%07ld", startFrame);
            imagePath += QString(num).toStdString();
            imagePath += std::string(".png");

            cv::Mat img = imread(imagePath.c_str(), 1);
            cv::Mat imgResized;
            resize(img, imgResized, Size(), fxSmallVideo, fySmallVideo, CV_INTER_AREA);

            analyzeGui->showImage(imgResized, oneName);
        }

        if (xCoord > intelWidth){
            graphImgCrop = graphImg;
            graphImgCrop = graphImg(Rect(0, 0, intelWidth, intelHeight));
            xCoord = 20;
        }

        for (int i = 0; i < numGraphLines; i++){
            int id;
            bool draw;
            if (groups){
                id = groupNameToId[list[i]];
                draw = groupAppearances[id][startFrame - 1];
            }
            else if (trackers){
                id = trackerNameToId[list[i]];
                draw = trackerAppearances[id][startFrame - 1];
            } else {
                id = i;
                draw = groupAppearances[id][startFrame - 1];
            }
            if (draw){
               struct colors* c = groupColors[i];
               CvScalar pointColor = CV_RGB(c->r, c->g, c->b);
               int start = (i+2)*(intelHeight/6);
               int yCoord = intelHeight - start;
               line(graphImgCrop, cvPoint(xCoord, yCoord), cvPoint(xCoord, yCoord - 6), pointColor, 2);
            }
        }

        if (groups || trackers){
            if (totalAppearances[startFrame - 1]){
                CvScalar pointColor = CV_RGB(0, 0, 0);
                int start = (numGraphLines+2)*(intelHeight/6);
                int yCoord = intelHeight - start;
                line(graphImgCrop, cvPoint(xCoord, yCoord), cvPoint(xCoord, yCoord - 6), pointColor, 2);
             }
        }

        xCoord++;

        analyzeGui->showImage(graphImgCrop, intelName);

        startFrame++;

    }

}

int Analyze::doWork() {

    parse();
    //debugAnalyze();

    aWin->insertHtml(QString("Simba$ "));
    QStringList command;

    while (true){

        command = getCommand();
        std::string action = command[0].toStdString();
        std::cout << action << std::endl;
        std::string end = command[command.size() - 1].toStdString();
        std::cout << end << std::endl;

        if (end.compare("\n")){

            std::cout << "new line detected" << std::endl;

            if (action.compare("play") == 0){
                std::cout << "play" << std::endl;
                //play(command);
            }

          // aWin->insertHtml(QString("Simba$ "));

        }

        app->processEvents();
    }

   return true;
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

  std::cout << "Printing tracker appearances" << std::endl;

  for (int i = 0; i < numTrackers; i++){

      std::cout << idToTrackerName[i] << std::endl << std::endl;
      std::vector<bool> &v = trackerAppearances[i];
      for (int j = 0; j < v.size(); j++){
          std::cout << QString::number(j+1).toStdString() + QString(": ").toStdString();
          std::cout << v[j] << std::endl;
      }

  }

  std::cout << "Printing group appearances" << std::endl;

  for (int i = 0; i < numGroups; i++){

      std::cout << idToGroupName[i] << std::endl << std::endl;
      std::vector<bool> &v = groupAppearances[i];
      for (int j = 0; j < v.size(); j++){
          std::cout << QString::number(j+1).toStdString() + QString(": ").toStdString();
          std::cout << v[j] << std::endl;
      }

  }

  std::cout << "Printing tracker counts" << std::endl;

  for (int i = 0; i < numTrackers; i++){

      std::cout << idToTrackerName[i] + QString(": ").toStdString() + QString::number(trackerCounts[i]).toStdString() << std::endl;
      std::cout << std::endl;
      }

  std::cout << "Printing group counts" << std::endl;

  for (int i = 0; i < numGroups; i++){

      std::cout << idToGroupName[i] + QString(": ").toStdString() + QString::number(groupCounts[i]).toStdString() << std::endl;
      std::cout << std::endl;
      }


  std::cout << "Printing start and end frames" << std::endl;

  for (int i = 0; i < numTrackers; i++){

      std::cout << idToTrackerName[i] << std::endl << std::endl;
      std::cout << QString("startFrame: ").toStdString() + QString::number(startFrames[i]).toStdString() << std::endl;
      std::cout << QString("endFrame: ").toStdString() + QString::number(endFrames[i]).toStdString() << std::endl;

      std::cout << std::endl;
      }




}

