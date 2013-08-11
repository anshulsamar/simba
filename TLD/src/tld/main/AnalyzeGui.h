#ifndef AnalyzeGui_H_
#define AnalyzeGui_H_

#include <string>
#include <opencv/highgui.h>

namespace tld
{

class AnalyzeGui
{
public:
    AnalyzeGui();
    ~AnalyzeGui();
    void initVideoWindow(int oneX, int oneY, int twoX, int twoY, int graphX, int graphY, std::string oneName, std::string twoName, std::string graphName);
    void showImage(cv::Mat image, std::string windowName);
char getKey();

private:

};

}

#endif /* AnalyzeGui_H_ */
