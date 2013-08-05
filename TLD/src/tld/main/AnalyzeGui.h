#ifndef AnalyzeGui_H_
#define AnalyzeGui_H_

#include <string>
#include <QMutex>

#include <opencv/highgui.h>

namespace tld
{

class AnalyzeGui
{
public:
    AnalyzeGui();
    ~AnalyzeGui();
    void initVideoWindow(int mainVideoX, int mainVideoY, int secondaryVideoX, int secondaryVideoY, std::string mainWindowName, std::string secondaryWindowName);
    void showImage(cv::Mat image, std::string windowName);
char getKey();

private:

};

/**
 * Get a bounding box from the user.
 * @param img image to display
 * @param rect CvRect containing the coordinates of the bounding box
 * @param AnalyzeGui initialized AnalyzeGui
 * @return PROGRAM_EXIT if 'q' or 'Q' pressed, SUCCESS if everything went right
 */
int getBBFromUser(IplImage *img, CvRect &rect, AnalyzeGui *AnalyzeGui, int num, std::string message);

}

#endif /* AnalyzeGui_H_ */
