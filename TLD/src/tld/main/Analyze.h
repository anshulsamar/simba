#ifndef Analyze_H_
#define Analyze_H_

#include <QThread>
#include <QSemaphore>
#include <QSettings>
#include <vector>
#include <QMessageBox>
#include <QTextEdit>
#include "AnalyzeGui.h"
#include <opencv/cv.h>

extern "C" {
#include <ccv.h>
}

class Analyze
{
public:

    Analyze(QTextEdit* aWin, std::string analysisImagesPath, std::string oneName, std::string twoName,  std::string intelName, int smallVideoWidth, int smallVideoHeight, int intelWidth, int intelHeight, QApplication* app, bool& ok) {
        analyzeGui = new AnalyzeGui();
        numGroups = 0;
        numTrackers = 0;
        frameCount = 1;
        this->aWin = aWin;
        this->analysisImagesPath = analysisImagesPath;
        this->oneName = oneName;
        this->twoName = twoName;
        this->intelName = intelName;
        this->app = app;
        this->intelWidth = intelWidth;
        this->intelHeight = intelHeight;
        this->smallVideoHeight = smallVideoHeight;
        this->smallVideoWidth = smallVideoWidth;

        std::string imagePath = this->analysisImagesPath;
        char num[64];
        memset(num, 0, 64);
        long x = 2;
        sprintf(num, "%07ld", x);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");
        cv::Mat img = cv::imread(imagePath.c_str(), 1);

        fySmallVideo = (double)smallVideoHeight/img.rows;
        fxSmallVideo  = (double)smallVideoWidth/img.cols;

        graphImg = cv::imread("/Users/Anshul/Desktop/Track/Build/TLD.app/Contents/Resources/graph.png", 1);
        if (!graphImg.data) {
            QMessageBox msgBox;
            msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
            msgBox.setText("Unable to open ./TLD.app/Contents/Resources/graph.png");
            msgBox.exec();
            ok = false;
        }
        graphImg.copyTo(graphImgCrop);

        cv::resize(graphImgCrop, graphImgCrop, cv::Size(), (double)intelWidth/graphImg.cols, (double)intelHeight/graphImg.rows);


    }
    ~Analyze(){
        delete analyzeGui;
    }

    int doWork();
    void initGui(int oneX, int oneY, int twoX, int twoY, int intelX, int intelY);

private:
    long startFrame(int trackerId);
    void showVideoImage(long frame, std::string winName);
    bool check(std::vector<std::string>& list);
    void userError(std::string e);
    void getImage(long frame, std::string newImageName, int x, int y, int width, int height);
    std::string getImagePath(long frame);
    void saveImage(long frame, std::string saveImagePath, bool crop, int x, int y, int width, int height);
    long getStartFrame(int trackerId);
    bool isGroup(std::string group);
    bool isTracker(std::string tracker);
    void getStats();
    void fillTotalAppearancesAND(std::vector <std::string>& list, std::vector<bool> &totalAppearances);
    void fillTotalAppearancesOR(std::vector <std::string>& list, std::vector<bool> &totalAppearances);
    void man(std::string command);
    void play(QStringList& command);
    void show(QStringList& command);
    bool parse();
    void debugAnalyze();
    void getCommand(QStringList& commandParts, bool& success);

    QTextEdit* aWin;
    QTextEdit* intelWin;
    AnalyzeGui *analyzeGui;
    QApplication* app;

    std::map<std::string, int> trackerNameToId;
    std::map<std::string, int> groupNameToId;
    std::map<int, std::string> idToGroupName;
    std::map<int, std::string> idToTrackerName;
    std::map<int, int> trackersToGroupMap;
    std::vector< std::vector<int> > trackersPerGroup;
    std::vector< std::vector<std::string> > trackerResults;
    std::vector< std::vector<bool> > trackerAppearances;
    std::vector< std::vector<bool> > groupAppearances;
    std::vector< long > trackerCounts;
    std::vector< long > groupCounts;
    std::vector< long > endFrames;
    std::vector< long > startFrames;
    std::vector< std::vector<long> > startStopFrames;

    struct colors {
        int r;
        int g;
        int b;
    };

    std::vector< struct colors* > groupColors;

    int numTrackers;
    long frameCount;
    int numGroups;
    std::string analysisImagesPath;
    std::string oneName;
    std::string twoName;
    std::string intelName;
    int intelWidth;
    int intelHeight;
    int smallVideoHeight;
    int smallVideoWidth;
    double fxSmallVideo;
    double fySmallVideo;

    cv::Mat graphImg;
    cv::Mat graphImgCrop;
    cv::Mat twoImg;
    cv::Mat twoImgCrop;

};

#endif /* Analyze_H_ */
