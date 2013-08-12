#ifndef AnalyzeGui_H_
#define AnalyzeGui_H_

#include <string>
#include <opencv/highgui.h>

class AnalyzeGui
{
public:
    AnalyzeGui();
    ~AnalyzeGui();
    void initGui(int oneX, int oneY, int twoX, int twoY, int intelX, int intelY, std::string oneName, std::string twoName, std::string intelName);
    void showImage(cv::Mat image, std::string windowName);
    char getKey();

private:

};

#endif /* AnalyzeGui_H_ */
