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
    void initVideoWindow(int videoX, int videoY);
    void showImage(IplImage *image);
    char getKey();
    char getVideoKey();
    void setWidthHeight(int width, int height);
    void setMouseHandler();
    std::string windowName();

private:
    std::string m_window_name;
    CvFont font;
    struct info {
        char key;
        int width;
        int height;
    };
    info i;
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
