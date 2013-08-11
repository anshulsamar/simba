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
}

void AnalyzeGui::initVideoWindow(int oneX, int oneY, int twoX, int twoY, int graphX, int graphY, std::string oneName, std::string twoName, std::string graphName)
{
    namedWindow(oneName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(oneName.c_str(), oneX, oneY);

    namedWindow(twoName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(twoName.c_str(), twoX, twoY);

    //namedWindow(graphName.c_str(), CV_WINDOW_AUTOSIZE);
    //moveWindow(graphName.c_str(), graphX, graphY);
}

}
