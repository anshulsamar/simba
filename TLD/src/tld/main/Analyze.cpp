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

bool Analyze::doWork(std::string resutlsDirectory, IplImage *img) {




}

Analyze::~Analyze(){

}

void Analyze::initGui(int desktopWidth, int desktopHeight, IplImage* img){

    analyzeGui->initVideoWindow(desktopWidth, desktopHeight);

}

