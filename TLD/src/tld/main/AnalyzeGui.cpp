#include "AnalyzeGui.h"
#include "Main.h"
#include <string>
#include <sstream>

using std::string;

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

void AnalyzeGui::initGui(int oneX, int oneY, int twoX, int twoY, int intelX, int intelY, std::string oneName, std::string twoName, std::string intelName)
{
    namedWindow(oneName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(oneName.c_str(), oneX, oneY);

    namedWindow(twoName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(twoName.c_str(), twoX, twoY);

    namedWindow(intelName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(intelName.c_str(), intelX, intelY);

}
