#ifndef MAIN_H_
#define MAIN_H_

#include <QThread>
#include <QSemaphore>
#include <QSettings>
#include <vector>
#include <QTextEdit>
#include <QMutex>
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
    ccv_dense_matrix_t* x;
    ccv_dense_matrix_t* y;

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
        x = 0;
        y = 0;
        gui = new Gui();
        textMutex = new QMutex;
    }
    ~Main();
    bool doWork(Settings* settings);
    void addTrackerInfo(int startFrame, int endFrame, CvRect* add);
    void initGui(int videoX, int videoY);

private:
    void createTLD(CvRect *rect, const ccv_tld_param_t ccv_tld_params, int trackerId);
    void initializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params);
    void reinitializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params);
    void deleteTrackersAndGroups();
    bool loadInitializationFile();
    void analysis();
    void deleteTracker(int i);
    void doDpm(IplImage* img, std::string imagePath, ccv_tld_param_t ccv_tld_params);
    void doBBF(IplImage* img, std::string imagePath, ccv_tld_param_t ccv_tld_params);

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
