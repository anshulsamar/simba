//Copyright 2013 Anshul Samar. All Rights Reserved.

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

/* Function: man(std::string command)
 * -----------------------------------
 * Gives man data about the commands.
 */

void Analyze::man(std::string command){

    if (command.compare("man") == 0) {
        aWin->insertHtml(QString("<p><b>Commands:</b><br></p>"));
        aWin->insertHtml(QString("<p>Type \"man\" followed by any of the commands to learn more: play, info, quit.<br></p>"));

    }

    if (command.compare("info") == 0) {

        aWin->insertHtml(QString("<p><b>info</b><br></p>"));
        aWin->insertHtml(QString("<p>The \"info\" command allows you to information about trackers. You may want to see which trackers you created <i>(info)</i>, information about a paricular tracker <i>(info -tracker face)</i>, or coordinates of a particular tracker on a particular frame <i>(info -tracker face 102)</i>. Flags and commands: <br><br></p>"));
        aWin->insertHtml(QString("<b>info</b><br>"));
        aWin->insertHtml(QString("Info all<br>"));
        aWin->insertHtml(QString("<b>info -tracker name</b><br>"));
        aWin->insertHtml(QString("Information about tracker \"name\"<br>"));
        aWin->insertHtml(QString("<b>play -tracker name frameNum</b><br>"));
        aWin->insertHtml(QString("Coordinates of tracker \"name\" at frameNum<br>"));
    }

    if (command.compare("play") == 0){

        aWin->insertHtml(QString("<p><b>play</b><br></p>"));
        aWin->insertHtml(QString("<p>The \"play\" command allows you to see various segments of your video. You may, for example, want to watch all of it <i>(play -a)</i>, watch only some of it <i>(play -start 10 -end 50)</i>, watch only part of the video with tracker \"ball\" <i>(play -combo ball)</i>, or simply watch only those segments with your entire family <i>(play -combo mom && dad && sister && me)</i>. Flags and commands: <br><br></p>"));
        aWin->insertHtml(QString("<b>play -a</b><br>"));
        aWin->insertHtml(QString("Play all<br>"));
        aWin->insertHtml(QString("<b>play -start startFrame -end endFrame</b><br>"));
        aWin->insertHtml(QString("Play tracked images from startFrame to endFrame<br>"));
        aWin->insertHtml(QString("<b>play -combo tracker1 group2 ... </b><br>"));
        aWin->insertHtml(QString("Play tracked images of tracker1, group2...<br>"));
    }

    if (command.compare("quit") == 0){

        aWin->insertHtml(QString("<p><b>quit</b><br></p>"));
        aWin->insertHtml(QString("<p>The \"quit\" command quits the program. <br><br></p>"));
    }
}

/* Function: play(QStringList& command)
 * -----------------------------------
 * Plays segments of video
 */

void Analyze::play(QStringList& command){

    std::vector <std::string> list;
    std::vector <std::string> list2;
    list.clear();
    list2.clear();
    bool combo = false;
    bool single = false;
    std::vector<bool> totalAppearances(frameCount);

    long startFrame = 1;
    long endFrame = frameCount;

    if (command.size() == 2 && command[1].compare("-a") == 0){
        startFrame = 1;
    }
    else if (command.size() == 5 && command[1].compare("-start") == 0 && command[3].compare("-end") == 0){
        bool ok1, ok2;
        startFrame = command[2].toLong(&ok1);
        endFrame = command[4].toLong(&ok2);
        if (!ok1 || !ok2 || startFrame < 1 || endFrame < 1 || endFrame > frameCount) {
            userError("You did not enter properly formatted frame numbers (note: both startFrame and endFrame must be in proper ranges.)");
            return;
        }
    } else if(command.size() >= 2 && command[1].compare("-tracker") == 0){
              if (command.size() != 3){
                  userError("You did not enter names of a tracker or entered more than one.");
                  return;
              }
              if (!isTracker(command[2].toStdString())){
                userError("Tracker does not exist.");
                return;

              }
              list.push_back(command[2].toStdString());

              fillTotalAppearancesOR(list, totalAppearances);
              single = true;

    } else if(command.size() >= 2 && command[1].compare("-group") == 0){
        if (command.size() != 3){
            userError("You did not enter names of a group or entered more than one.");
            return;
        }
        if (!isGroup(command[2].toStdString())){
          userError("Group does not exist.");
          return;

        }
        list.push_back(command[2].toStdString());

        fillTotalAppearancesOR(list, totalAppearances);
        single = true;

    }
    else if (command.size() >= 2 && command[1].compare("-combo") == 0){
        if (command.size() < 5){
            userError("You did not enter names of trackers or groups.");
            return;
        }
        else if (command.size() == 2){
            fillTotalAppearancesOR(list, totalAppearances);
        } else if (command.size() >= 5){

                std::string tracker1 = command[2].toStdString();
                std::string op = command[3].toStdString();
                std::string tracker2 = command[4].toStdString();
                list.clear();
                list.push_back(tracker1);
                list.push_back(tracker2);

              if ((!isTracker(tracker1) && !isGroup(tracker1)) || (!isTracker(tracker2) && !isGroup(tracker2))){
                userError("Tracker or group does not exist.");
                return;

              }

                if (op.compare("&&") == 0){
                    std::cout << "Combo function filling and appearances" << std::endl;
                     fillTotalAppearancesAND(list, totalAppearances);
                }
                else if (op.compare("||") == 0)
                    fillTotalAppearancesOR(list, totalAppearances);
                else {
                    userError("Incorrect usage");
                    return;
                }

                if ((7 - command.size()) %2 != 0){
                    userError("Incorrect usage");
                    return;
                }

                for (int i = 5; i < command.size(); i+=2){
                    std::string tracker1 = command[i+1].toStdString();

                      if (!isTracker(tracker1) && !isGroup(tracker1)){
                        userError("Tracker or group does not exist.");
                        return;

                      }

                    std::string op = command[i].toStdString();
                    list2.clear();
                    list2.push_back(tracker1);
                    list.push_back(tracker1);

                    if (op.compare("&&") == 0){
                         fillTotalAppearancesAND(list, totalAppearances);
                    }
                    else if (op.compare("||") == 0)
                        fillTotalAppearancesOR(list, totalAppearances);
                    else {
                        userError("Incorrect usage");
                        return;
                    }
                }

        combo = true;
        }
    } else {
        userError("Incorrect usage");
        return;
    }

    if (combo || single)
        while (!totalAppearances[startFrame - 1])
            startFrame++;

    while (startFrame < endFrame) {

        if (combo || single){
            if (totalAppearances[startFrame - 1]){
                showVideoImage(startFrame, oneName);
                cvWaitKey(30);
            }
        } else {
           showVideoImage(startFrame, oneName);
           cvWaitKey(30);
        }
        startFrame++;
    }

}

/* Function: getCoordinates(long frame, std::string tracker, int& x, int& y, int& width, int& height)
 * -----------------------------------
 * Fills x, y, width, and height with appropriate results from given tracker/frame.
 */

bool Analyze::getCoordinates(long frame, std::string tracker, int& x, int& y, int& width, int& height){

    int id = trackerNameToId[tracker];

    std::string result = trackerResults[id][frame - 1];
    QStringList s = QString(result.c_str()).split(",");
    x = s[0].toInt();
    y = s[1].toInt();
    width = s[2].toInt();
    height = s[3].toInt();

    return true;

}

/* Function: info(QStringList& command)
 * -----------------------------------
 * Gets information about trackers.
 */


void Analyze::info(QStringList& command){

    if (command.size() == 1){

        aWin->insertHtml(QString("<u>Trackers</u><br>"));
        for (int i = 0; i < numTrackers; i++){
            aWin->insertHtml(QString(idToTrackerName[i].c_str()) + QString(", ") + QString("Group: ") + QString(idToGroupName[trackersToGroupMap[i]].c_str()) + QString("<br>"));

        }
    }

    if (command.size() == 3 && command[1].compare("-tracker") == 0){

        if (!isTracker(command[2].toStdString())){
            userError("Not a tracker");
            return;
        }

        aWin->insertHtml(QString("<u>") + command[2] + QString("</u><br>"));
        int id = trackerNameToId[command[2].toStdString()];
        aWin->insertHtml(QString("Group: ") + QString(idToGroupName[trackersToGroupMap[id]].c_str()) + QString("<br>"));
        std::vector<bool>& appearances = trackerAppearances[id];
        long count = 0;
        for (int i = 0; i < appearances.size(); i++){
            if (appearances[i]) count++;
        }
        long totalFrames = endFrames[id] - startFrames[id] + 1;
        double percentDetections = (double)count/totalFrames;
        double totalPercentDetections = (double)count/frameCount;

        aWin->insertHtml(QString("Total Frames: ") +  QString::number(frameCount) + QString("<br>"));
        aWin->insertHtml(QString("Start frame: ") + QString::number(startFrames[id]) + QString(" End Frame: ") + QString::number(endFrames[id]) + QString("<br>"));

        aWin->insertHtml(QString("Number of detections: ") + QString::number(count) + QString("<br>"));
        \
        aWin->insertHtml(QString("% Detections from start to end frame: ") + QString::number(percentDetections) + QString("<br>"));
        aWin->insertHtml(QString("% Detections in entire video: ") + QString::number(totalPercentDetections) + QString("<br>"));

    }

    if (command.size() == 4 && command[1].compare("-tracker") == 0){

        if (!isTracker(command[2].toStdString())){
            userError("Not a tracker");
            return;
        }

        bool ok;
        long frame = command[3].toLong(&ok);
        if (!ok){
            userError("Not a number");
            return;
        }

        int id = trackerNameToId[command[2].toStdString()];

        if (frame > endFrames[id]){
            userError("Out of bounds");
            return;
        }

        int x, y, width, height;
        getCoordinates(frame, command[2].toStdString(), x, y, width, height);

        aWin->insertHtml(QString::number(x) + QString(" ") + QString::number(y) + QString(" ") + QString::number(width) + QString(" ") + QString::number(height) + QString("<br>"));

    }


}

/* Function: doWork()
 * -----------------------------------
 * Gets commands from user and executes commands.
 */

int Analyze::doWork() {

    parse();
    debugAnalyze();
    showVideoImage(1, oneName);

    aWin->insertHtml(QString("Simba$ "));
    QStringList command;

    std::string action = "";
    std::string end = "";

    while (true){

        bool success = false;
        QStringList command;
        getCommand(command, success);

        if (success){

            action = command[0].toStdString();

            if (action.compare("man") == 0){
                if (command.size() != 2)
                    man("man");
                else
                    man(command[1].toStdString());
            }

            if (action.compare("play") == 0)
                play(command);

            if (action.compare("quit") == 0)
                break;

            if (action.compare("info") == 0){
                info(command);
            }

            aWin->insertHtml(QString("Simba$ "));
            aWin->ensureCursorVisible();
        }

       app->processEvents();
    }

   return 1;
}


/* Function: initGui(int videoX, int videoY)
 * ------------------------------------------
 * Sets up the window to display the tracked images
 */

void Analyze::initGui(int videoX, int videoY){

    analyzeGui->initGui(videoX, videoY, oneName);

}

/* Function: getImagePath(long frame)
 * ------------------------------------------
 * Gets image for the provided frame number from the analysisImagesPath directory
 */

std::string Analyze::getImagePath(long frame){

    std::string imagePath = analysisImagesPath;
    char num[64];
    memset(num, 0, 64);
    sprintf(num, "%07ld", frame);
    imagePath += QString(num).toStdString();
    imagePath += std::string(".png");
    return imagePath;
}

/* Function: showVideoImage(long frame, std::string winName)
 * ------------------------------------------
 * Shows a particular frame on the gui window.
 */

void Analyze::showVideoImage(long frame, std::string winName){

    std::string imagePath = getImagePath(frame);
    cv::Mat img = imread(imagePath.c_str(), 1);
    analyzeGui->showImage(img, winName);

}

/* Function: saveImage(long frame, std::string saveImagePath, bool crop, int x, int y, int width, int height)
 * ------------------------------------------
 * Crops an image and saves it in the analysisImagesPath directory.
 */

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

/* Function: getStartFrame(int trackerId)
 * ------------------------------------------
 * Gets the starting frame for a particular tracker.
 */

long Analyze::getStartFrame(int trackerId){

    long sframe = 1;

    std::vector< std::string > *v = &trackerResults[trackerId];

    for (int i = 0; i < v->size(); i++){
        if (v->at(i).compare("0,0,0,0") != 0)
            break;
        sframe++;
    }

    return sframe;

}

/* Function: isGroup(std::string group)
 * ------------------------------------------
 * True if given string is an existing group.
 */

bool Analyze::isGroup(std::string group){

    return groupNameToId.find(group) != groupNameToId.end();

}

/* Function: isTracker(std::string group)
 * ------------------------------------------
 * True if given string is an existing tracker
 */

bool Analyze::isTracker(std::string tracker){

    return trackerNameToId.find(tracker) != trackerNameToId.end();

}

/* Function: getStats()
 * ------------------------------------------
 * Fills the trackerAppearances and groupAppearances vectors.
 */

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
                std::string first = result[j];
                if (first.compare("0,0,0,0") != 0){
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

/* Function: getCommand(QStringList& commandParts, bool& success)
 * ------------------------------------------
 * Gets command from user.
 */


void Analyze::getCommand(QStringList& commandParts, bool& success){

    success = false;

    QString info = aWin->toPlainText();
    QStringList commands = info.split("$ ");
    QString command = commands[commands.size() - 1];
    commandParts.clear();
    commandParts = command.split(" ");

    if (commandParts.size() > 0 && commandParts[0].compare("") != 0){
         std::string end = commandParts[commandParts.size() - 1].toStdString();
         if (end[end.size() - 1] == '\n'){
            success = true;
            end = end.substr(0, end.size() - 1);
            commandParts[commandParts.size() - 1] = QString(end.c_str());
         }
    }

}

/* Function: parse()
 * ------------------------------------------
 * Parses csv files and groupSettings.ini
 */

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

        struct colors *c = new struct colors;
        c->r = red;
        c->g = green;
        c->b = blue;
        groupColors.push_back(c);
        QString s = settingsIn->value("Trackers").toString();
        QStringList trackersStringList = s.split(' ');
        settingsIn->endGroup();
        std::vector<int> v;


        for (int j = 0; j < trackersStringList.size(); j++){
            std::string trackerName = trackersStringList[j].toStdString();
            std::string filePath = analysisImagesPath + groupsStringList[i].toStdString() + trackerName + ".csv";
            QFile file(filePath.c_str());
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
            QTextStream in(&file);
            QString text;
            text = in.readAll();
            file.close();
            trackerNameToId.insert(std::pair<string, int>(trackerName, numTrackers));
            idToTrackerName.insert(std::pair<int, string>(numTrackers, trackerName));
            v.push_back(numTrackers);
            QStringList trackerResultsPerFrame = text.split('\n');
            std::vector< std::string > v;
            for (int j = 0; j < trackerResultsPerFrame.size(); j++){
                v.push_back(trackerResultsPerFrame[j].toStdString());
            }
            trackerResults.push_back(v);
            endFrames.push_back(trackerResultsPerFrame.size());
            trackersToGroupMap.insert(std::pair<int, int>(numTrackers, numGroups));
            numTrackers++;
        }
        trackersPerGroup.push_back(v);
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

/* Function: fillTotalAppearancesAND(std::vector <std::string>& list, std::vector<bool> &totalAppearances)
 * ------------------------------------------
 * Populates a vector with appearances of trackers/group part of list using "AND." I.e. totalApperances[0] is
 * true only if every tracker or group is the list made an appearance at frame 0.
 */

void Analyze::fillTotalAppearancesAND(std::vector <std::string>& list, std::vector<bool> &totalAppearances){

    for (long f = 0; f < totalAppearances.size(); f++){

        bool b;

        if (isGroup(list[0]))
            b = (groupAppearances[groupNameToId[list[0]]])[f];
        else
            b = (trackerAppearances[trackerNameToId[list[0]]])[f];

        for (int i = 0; i < list.size(); i++){
            if (isGroup(list[i]))
                b = b && (groupAppearances[groupNameToId[list[i]]])[f];
            else
                b = b && (trackerAppearances[trackerNameToId[list[i]]])[f];

        }

        if (b)
            std::cout << QString::number(f).toStdString() + ": 1" << std::endl;
        else
                        std::cout << QString::number(f).toStdString() + ": 0" << std::endl;

        totalAppearances[f] = b;
    }

}

/* Function: fillTotalAppearancesOR(std::vector <std::string>& list, std::vector<bool> &totalAppearances)
 * ------------------------------------------
 * Populates a vector with appearances of trackers/group part of list using "OR." I.e. totalApperances[0] is
 * true only if any one of the trackers or groups is the list made an appearance at frame 0.
 */

void Analyze::fillTotalAppearancesOR(std::vector <std::string>& list, std::vector<bool> &totalAppearances){

    for (int i = 0; i < list.size(); i++){

        std::vector<bool> g;

        if (isGroup(list[i]))
            g = groupAppearances[groupNameToId[list[i]]];
        else
            g = trackerAppearances[trackerNameToId[list[i]]];

        for (long i = 0; i < g.size(); i++){
            if (g[i])
                totalAppearances[i] = true;
        }
    }



}

/* Function: userError(std::string e)
 * -----------------------------------
 * Outputs error to the user.
 */

void Analyze::userError(std::string e){
    aWin->insertHtml(QString(e.c_str()) + QString("<br>"));
}

/* Function: debugAnalyze()
 * -------------------------
 * Debugging function.
 */

void Analyze::debugAnalyze(){

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

