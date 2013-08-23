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

    Analyze(QTextEdit* aWin, std::string analysisImagesPath, std::string oneName, QApplication* app, bool& ok) {
        analyzeGui = new AnalyzeGui();
        numGroups = 0;
        numTrackers = 0;
        frameCount = 1;
        this->aWin = aWin;
        this->analysisImagesPath = analysisImagesPath;
        this->oneName = oneName;
        this->app = app;
    }
    ~Analyze(){
        delete analyzeGui;
    }

    int doWork();
    void initGui(int videoX, int videoY);

private:

    void man(std::string command);
    void play(QStringList& command);
    void info(QStringList& command);
    bool getCoordinates(long frame, std::string tracker, int& x, int& y, int& width, int& height);

    void getStats();
    bool parse();

    void getCommand(QStringList& commandParts, bool& success);
    void fillTotalAppearancesAND(std::vector <std::string>& list, std::vector<bool> &totalAppearances);
    void fillTotalAppearancesOR(std::vector <std::string>& list, std::vector<bool> &totalAppearances);
    bool isGroup(std::string group);
    bool isTracker(std::string tracker);
    long startFrame(int trackerId);
    void showVideoImage(long frame, std::string winName);
    void userError(std::string e);
    std::string getImagePath(long frame);
    void saveImage(long frame, std::string saveImagePath, bool crop, int x, int y, int width, int height);
    long getStartFrame(int trackerId);

    void debugAnalyze();

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
    std::vector< double > percentDetectionsTrackers;
    std::vector< double > percentDetectionsGroups;

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

};

#endif /* Analyze_H_ */
