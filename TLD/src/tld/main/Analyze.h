#ifndef Analyze_H_
#define Analyze_H_

#include <QThread>
#include <QSemaphore>
#include <QSettings>
#include <vector>
#include <QTextEdit>
#include <QMutex>
#include "AnalyzeGui.h"

extern "C" {
#include <ccv.h>
}

class Analyze
{
public:

    tld::AnalyzeGui *analyzeGui;
    QTextEdit* aWin;

    Analyze() {
        analyzeGui = new tld::AnalyzeGui();
        numGroups = 0;
        numTrackers = 0;
        frameCount = 1;
    }
    ~Analyze();
    bool doWork(std::string resultsDirectory, IplImage *img);
    void initGui(int videoX, int videoY, IplImage *img, QTextEdit* aWin);
    void debugAnalyze();


private:

    std::map<std::string, int> trackerNameToId;
    std::map<std::string, int> groupNameToId;
    std::map<int, std::string> idToGroupName;
    std::map<int, std::string> idToTrackerName;
    std::map<int, int> trackersToGroupMap;
    std::vector< std::vector<int> > trackersPerGroup;
    std::vector< std::vector<std::string> > trackerResults;

    struct colors {
        int r;
        int g;
        int b;
    };

    std::vector< struct colors* > groupColors;

    int numTrackers;
    long frameCount;
    int numGroups;
};

#endif /* Analyze_H_ */
