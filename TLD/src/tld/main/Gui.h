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

    //shows image on window
    void showImage(IplImage *image);

    //get keyboard key
    char getKey();

    //get mouse click
    char getVideoKey();

    //sets internal struct of window width and height
    void setWidthHeight(int width, int height);

    //sets up the mouse handler
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

//Gets bounding box from the user

int getBBFromUser(IplImage *img, CvRect &rect, Gui *gui, int num, std::string message);


#endif /* GUI_H_ */
