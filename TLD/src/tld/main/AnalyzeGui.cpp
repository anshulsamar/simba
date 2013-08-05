#include "AnalyzeGui.h"
#include "Main.h"
#include <string>
#include <sstream>
#include "global.h"

using std::string;

namespace tld
{

AnalyzeGui::AnalyzeGui()
{

}

AnalyzeGui::~AnalyzeGui()
{
    cvDestroyAllWindows();
}

char AnalyzeGui::getKey()
{
    return cvWaitKey(100000);
}

void AnalyzeGui::showImage(cv::Mat image, std::string windowName)
{

    imshow(windowName, image );
    //cvShowImage(windowName.c_str(), image);
}

void AnalyzeGui::initVideoWindow(int mainVideoX, int mainVideoY, int secondaryVideoX, int secondaryVideoY, std::string mainWindowName, std::string secondaryWindowName)
{
    namedWindow(mainWindowName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(mainWindowName.c_str(), mainVideoX, mainVideoY);

    namedWindow(secondaryWindowName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(secondaryWindowName.c_str(), secondaryVideoX, secondaryVideoY);
}

}
