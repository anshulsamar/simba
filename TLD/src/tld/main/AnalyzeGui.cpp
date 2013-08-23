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

void AnalyzeGui::initGui(int videoX, int videoY, std::string oneName)
{
    namedWindow(oneName.c_str(), CV_WINDOW_AUTOSIZE);
    moveWindow(oneName.c_str(), videoX, videoY);

}
