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

std::map<int, int> trackersToGroupMap;
std::vector< std::vector<int> > trackersPerGroup;

struct colors {
    int r;
    int g;
    int b;
};

std::vector< struct colors* > groupColors;

bool Analyze::doWork(std::string resultsDirectory, IplImage *img) {

    std::string iniPath = resultsDirectory + "groupSettings.ini";
    std::ifstream infile(iniPath.c_str());
    QSettings* settingsIn = new QSettings(QString(iniPath.c_str()), QSettings::IniFormat);
    if (!infile || settingsIn->status() != QSettings::NoError){
        std::cerr << "Failed to open initialization file\n" << std::endl;
        return false;
    }
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

   debugAnalyze();




}

Analyze::~Analyze(){

}

void Analyze::initGui(int desktopWidth, int desktopHeight, IplImage* img, QTextEdit* aWin){

    this->aWin = aWin;
    //analyzeGui->initVideoWindow(desktopWidth, desktopHeight);

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

