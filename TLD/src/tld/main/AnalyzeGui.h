//Copyright 2013 Anshul Samar. All Rights Reserved.

#ifndef AnalyzeGui_H_
#define AnalyzeGui_H_

#include <string>
#include <opencv/highgui.h>

class AnalyzeGui
{
public:
    AnalyzeGui();
    ~AnalyzeGui();
    void initGui(int videoX, int videoY, std::string oneName);
    void showImage(cv::Mat image, std::string windowName);
    char getKey();

private:

};

#endif /* AnalyzeGui_H_ */
