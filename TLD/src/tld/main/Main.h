/* Modified by Anshul Samar
 * Using libccv code from LiuLiu
 * Previous Credits:
 *
*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

/*
 * main.h
 *
 *  Created on: Nov 18, 2011
 *      Author: Georg Nebehay
 */



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

class Main
{
public:

    Main(Settings* settings, const ccv_tld_param_t ccv_tld_paramsUser, int videoX, int videoY, QTextEdit* aWin) : ccv_tld_params(ccv_tld_paramsUser) {
        trackImagesPath = settings->m_trackImagesPath;
        resultsDirectory = settings->m_resultsDirectory;
        numTrackers = 0;
        numGroups = 0;
        frameCount = 1;
        x = 0;
        y = 0;
        gui = new Gui();
        this->aWin = aWin;
        initGui(videoX, videoY);
        cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.35, 0.35, 0, 1, 8);
    }
    ~Main(){
        delete gui;
    }

    bool track(Settings* settings);

private:
    //Determines action based on user keyboard entry
    int userAction(char key);

    //Sets up gui related struct
    void initGui(int videoX, int videoY);

    //Creating settings file (which trackers belong to which group, etc) for later analysis
    void writeSettingsToFile();

    //Gets image path for specific frame number
    std::string getImagePath(long frame, std::string p);

    //Creating tracker objects
    bool createTLD(long startFrame, long endFrame, CvRect *rect, const ccv_tld_param_t ccv_tld_params, int trackerId);
    void initializeTracking(IplImage *img, const ccv_tld_param_t ccv_tld_params);
    void addTrackerInfo(long startFrame, long endFrame, CvRect* add, std::string trackerName, int trackerId);

    //deleting functions
    void deleteTrackersAndGroups();
    void deleteTracker(int i);

    //Experimental functions for autoinitializaitons using libccv's dpm/bbf (see minimum branch for start of implementation)
    void doDpm(IplImage* img, std::string imagePath, ccv_tld_param_t ccv_tld_params);
    void doBBF(IplImage* img, std::string imagePath, ccv_tld_param_t ccv_tld_params);

    //Holds tracker objects
    std::vector<Tracker *> trackers;

    //Holds tracking coordinates for each tracker
    std::vector<CvRect*> rectangles;

    //Semaphores for thread communication between main thread and tracker threads
    std::vector<QSemaphore *> trackerSems;
    std::vector<QSemaphore *> mainSems;

    //Start and end frames for each tracker object
    std::vector<long> startFrames;
    std::vector<long> endFrames;

    //Data structures to easily access tracker, group names and ids
    std::map<std::string, int> trackerNameToId;
    std::map<std::string, int> groupNameToId;
    std::map<int, std::string> idToGroupName;
    std::map<int, std::string> idToTrackerName;
    std::map<int, int> trackersToGroupMap;
    std::vector< std::vector<int> > trackersPerGroup;

    //FILE*s to csv files for outputing data
    std::vector<FILE *> files;

    struct colors {
        int r;
        int g;
        int b;
    };

    //The color code of each group (for visual purposes)
    std::vector< struct colors* > groupColors;

    //Gui for displaying images
    Gui *gui;

    //Text box for displaying information
    QTextEdit* aWin;

    //Where track gets images from
    std::string trackImagesPath;

    //Where results will be saved
    std::string resultsDirectory;

    //Holds images
    ccv_dense_matrix_t* x;
    ccv_dense_matrix_t* y;

    //Other book keeping
    int numTrackers;
    long frameCount;
    int numGroups;

    CvFont font;
    IplImage *img;

    //Settings for tracker
    const ccv_tld_param_t ccv_tld_params;


};

#endif /* MAIN_H_ */
