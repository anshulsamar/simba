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

void Analyze::showVideoImage(long frame, std::string winName){

    std::string imagePath = getImagePath(frame);
    cv::Mat img = imread(imagePath.c_str(), 1);
    cv::Mat imgCopy;
    img.copyTo(imgCopy);
    cv::resize(img, imgCopy, cv::Size(), fxSmallVideo, fySmallVideo);
    analyzeGui->showImage(imgCopy, winName);

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

void Analyze::fillTotalAppearancesAND(std::vector <std::string>& list, std::vector<bool> &totalAppearances){

    for (int f = 0; f < totalAppearances.size(); f++){

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

        totalAppearances[f] = b;


    }

}

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

void Analyze::show(QStringList& command){

    std::cout << "show" << std::endl;

    if (command.size() != 2){
       userError("show: Incorrect command format.");
       return;
    }

    std::string name = command[1].toStdString();
    if (!isTracker(name)){
        userError("show: Name given is not a tracker.");
        return;
    }

    int trackerId = trackerNameToId[name];
    std::vector<bool>& appearances = trackerAppearances[trackerId];
    std::vector< std::string >& result = trackerResults[trackerId];

    std::string imagePath;
    char num[64];

    std::cout << "start frame: "  + QString::number(startFrames[trackerId]).toStdString() + " end frame: "  + QString::number(endFrames[trackerId]).toStdString() << std::endl;

    for (long i = startFrames[trackerId] - 1; i < endFrames[trackerId]; i++){

        graphImg.copyTo(graphImgCrop);
        cv::resize(graphImgCrop, graphImgCrop, cv::Size(), (double)smallVideoWidth/graphImg.cols,(double)smallVideoHeight/graphImg.rows);

        QStringList frame = QString(result[i].c_str()).split(QString(" "));

        imagePath = analysisImagesPath;
        memset(num, 0, 64);
        sprintf(num, "%07ld", i);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");

        cv::Mat img = imread(imagePath.c_str(), 1);
        cv::Mat imgResized;
        img.copyTo(imgResized);

        int x;
        int y;
        int width = frame[2].toInt();
        int height = frame[3].toInt();

        if (frame[0].toInt() < 0)
            x = 0;
        else
            x = frame[0].toInt();
        if (frame[1].toInt() < 0)
            y = 0;
        else
            y = frame[1].toInt();

        cv::Rect roi( cv::Point(x, y), cv::Size(width, height));
        imgResized = imgResized( roi );

        resize(img, imgResized, Size(), fxSmallVideo, fySmallVideo, CV_INTER_AREA);

        double xd = smallVideoWidth/2 - (double)width/2;
        double yd = smallVideoHeight/2 - (double)width/2;

        cv::Rect loc( cv::Point( xd, yd ), cv::Size( width, height ));
        cv::Mat regionOfInterest = graphImgCrop( loc );
        imgResized.copyTo( regionOfInterest );

        std::cout << frame[0].toStdString() + " "  + frame[1].toStdString() + " "  + frame[2].toStdString() + " " + frame[3].toStdString() << std::endl;

        analyzeGui->showImage(imgResized, twoName);

    }


}

void Analyze::man(std::string command){

    if (command.compare("man") == 0);

      if (command.compare("show") == 0);


    if (command.compare("play") == 0){

        aWin->insertHtml(QString("<p><b>play</b><br></p>"));
        aWin->insertHtml(QString("<p>The \"play\" command allows you to see various segments of your video. You may, for example, want to watch all of it <i>(play -a)</i>, watch only some of it <i>(play -start 10 -end 50)</i>, watch only part of the video with tracker \"ball\" <i>(play -combo ball)</i>, or simply watch only those segments with your entire family <i>(play -combo mom && dad && sister && me)</i>. When the \"play\" command runs, Simba also creates a visual representation of segments across frames that have been tracked. Flags and commands: <br><br></p>"));
        aWin->insertHtml(QString("<b>play -a</b><br>"));
        aWin->insertHtml(QString("Play all<br>"));
        aWin->insertHtml(QString("<b>play -start startFrame -end endFrame</b><br>"));
        aWin->insertHtml(QString("Play tracked images from startFrame to endFrame<br>"));
        aWin->insertHtml(QString("<b>play -combo tracker1 group2 ... </b><br>"));
        aWin->insertHtml(QString("Play tracked images of tracker1, group2...<br>"));

    }


}

bool Analyze::check(std::vector<std::string>& list){

    bool ok = true;

    for (int i = 0; i < list.size(); i++){

        if (!isGroup(list[i]) && !isTracker(list[i])){
           aWin->insertHtml(QString(list[i].c_str()) + QString(" is neither a tracker nor group.<br>"));
           ok = false;
        }

    }

    return ok;

}

void Analyze::userError(std::string e){
    aWin->insertHtml(QString(e.c_str()) + QString("<br>"));
}

void Analyze::play(QStringList& command){

    std::vector <std::string> list;
    std::vector <std::string> list2;
    bool combo = false;
    bool single = false;
    std::vector<bool> totalAppearances(frameCount);
    graphImg.copyTo(graphImgCrop);
    resize(graphImgCrop, graphImgCrop, Size(), (double)intelWidth/graphImg.cols, (double)intelHeight/graphImg.rows);

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
            userError("You did not enter properly formatted frame numbers (note: both startFrame and endFrame must be in proper ranges.");
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
          userError("Tracker does not exist.");
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

    int numGraphLines = 10;

    if (list.size() <= 10)
        numGraphLines = list.size();

    int xCoord = 20;

    std::string imagePath;
    char num[64];

    std::cout << QString::number(startFrame).toStdString() << std::endl;

    if (combo || single)
        while (!totalAppearances[startFrame - 1])
            startFrame++;

    std::cout << QString::number(startFrame).toStdString() << std::endl;

    while (startFrame < endFrame) {

        if (combo || single){
            if (totalAppearances[startFrame - 1]){
                showVideoImage(startFrame, oneName);
            }
        } else {
           cv::Mat img = imread(imagePath.c_str(), 1);
           showVideoImage(startFrame, oneName);
        }

        if (xCoord > intelWidth){
            graphImg.copyTo(graphImgCrop);
            cv::resize(graphImgCrop, graphImgCrop, cv::Size(), (double)intelWidth/graphImg.cols,(double)intelHeight/graphImg.rows);
            xCoord = 20;
        }

        if (combo || single){
            for (int i = 0; i < numGraphLines; i++){
                int id;
                bool draw;
                int colorId;
                if (isGroup(list[i])){

                    id = groupNameToId[list[i]];
                    colorId = id;
                    draw = groupAppearances[id][startFrame - 1];
                }
                else {
                    id = trackerNameToId[list[i]];
                    colorId = trackersToGroupMap[id];
                    draw = trackerAppearances[id][startFrame - 1];
                }

                if (draw){
                   struct colors* c = groupColors[colorId];
                   CvScalar pointColor = CV_RGB(c->r, c->g, c->b);
                   int start = (i+1)*(intelHeight/5);
                   int yCoord = intelHeight - start;
                   line(graphImgCrop, cvPoint(xCoord, yCoord), cvPoint(xCoord, yCoord - 6), pointColor, 2);
                }
            }
        }

        if (combo){
            if (totalAppearances[startFrame - 1]){
                CvScalar pointColor = CV_RGB(255, 255, 255);
                int start = (numGraphLines+2)*(intelHeight/6);
                int yCoord = intelHeight - start;
                line(graphImgCrop, cvPoint(xCoord, yCoord), cvPoint(xCoord, yCoord - 6), pointColor, 2);
             }
        }

        if (!combo && !single) {
          for (int i = 0; i < numGroups; i++){
                int id = i;
                bool draw;
                draw = groupAppearances[id][startFrame - 1];

                if (draw){
                   struct colors* c = groupColors[i];
                   CvScalar pointColor = CV_RGB(c->r, c->g, c->b);
                   int start = (i+1)*(intelHeight/6);
                   int yCoord = intelHeight - start;
                   line(graphImgCrop, cvPoint(xCoord, yCoord), cvPoint(xCoord, yCoord - 6), pointColor, 2);
                }
          }
        }

        xCoord++;

        analyzeGui->showImage(graphImgCrop, intelName);

        cvWaitKey(30);
        startFrame++;

    }

}

int Analyze::doWork() {

    parse();
    debugAnalyze();

    showVideoImage(1, oneName);
    showVideoImage(1, twoName);
    analyzeGui->showImage(graphImgCrop, intelName);


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



            if (action.compare("show") == 0)
                show(command);


            if (action.compare("quit") == 0)
                break;

            aWin->insertHtml(QString("Simba$ "));


        }



        app->processEvents();
    }

   return 2;
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

