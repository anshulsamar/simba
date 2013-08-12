#ifndef GUI_H_
#define GUI_H_

#include <string>
#include <QMutex>

#include <opencv/highgui.h>

class Gui
{
public:
    Gui();
    ~Gui();
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
 * @param gui initialized gui
 * @return PROGRAM_EXIT if 'q' or 'Q' pressed, SUCCESS if everything went right
 */
int getBBFromUser(IplImage *img, CvRect &rect, Gui *gui, int num, std::string message);


#endif /* GUI_H_ */
