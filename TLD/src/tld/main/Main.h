#ifndef MAIN_H_
#define MAIN_H_

#include <QThread>
#include <QSemaphore>
#include <QSettings>
#include <vector>
#include <QTextEdit>
#include <QMutex>
#include "ImAcq.h"
#include "Gui.h"
#include "Settings.h"
#include "tracker.h"

extern "C" {
#include <ccv.h>
}

using tld::Settings;

enum Retval
{
    PROGRAM_EXIT = 0,
    SUCCESS = 1
};

class Main
{
public:
    ImAcq *imAcq;
    IplImage *graphImage;
    tld::Gui *gui;
    int graphHeight;
    int graphWidth;
    bool selectManually;
    bool loadIni;
    bool saveIni;
    bool saveResults;
    std::string videoPath;
    std::string iniPath;
    std::string saveIniDirectory;
    std::string resultsDirectory;
    int trackersInIniFile;
    QSettings* settingsOut;
    QTextEdit* analytics;
    QMutex* textMutex;

    Main() {
        selectManually = true;
        loadIni = false;
        saveIni = false;
        saveResults = false;
        videoPath = "";
        iniPath = "";
        resultsDirectory = "";
        settingsOut = NULL;
        graphImage = NULL;
        numTrackers = 0;
        numGroups = 0;
        frameCount = 1;

        imAcq = imAcqAlloc();
        gui = new Gui();
        textMutex = new QMutex;
    }
    ~Main();
    bool doWork();
    void addTrackerInfo(int startFrame, int endFrame, CvRect* add);
    void initGui(QTextEdit* textEdit, int videoX, int videoY, int graphWidth, int graphHeight, int graphX, int graphY);

private:
    void createTLD(CvRect *rect, const ccv_tld_param_t ccv_tld_params, int trackerId);
    void initializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params);
    void reinitializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params);
    void deleteTrackersAndGroups();
    bool loadInitializationFile();
    void analysis();

    std::vector<Tracker *> trackers;
    std::vector<CvRect*> rectangles;
    std::vector<QSemaphore *> trackerSems;
    std::vector<QSemaphore *> mainSems;
    std::vector<int> startFrames;
    std::vector<int> endFrames;
    std::map<string, int> trackerNamesToId;
    std::map<string, int> groupNameToId;
    std::map<int, int> trackersToGroupMap;
    std::vector< std::vector<int>* > groups;
    std::vector<int> groupNumTrackers;


    struct colors {
        int r;
        int g;
        int b;
    };

        std::vector< struct colors* > groupColors;

    std::vector<int> trackerConsecutiveMissed;

    int numTrackers;
    int frameCount;
    int numGroups;
};

#endif /* MAIN_H_ */
