#include "Gui.h"
#include "Main.h"
#include <string>
#include <sstream>
#include "global.h"

using std::string;

Gui::Gui() :  m_window_name("Track Learn Detect")
{
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 2, 8);
}

Gui::~Gui()
{
    cvDestroyAllWindows();
}

void Gui::showImage(IplImage *image)
{
    cvRectangle(image, cvPoint(i.width - sizeBox - spacing, i.height - sizeBox - spacing), cvPoint(i.width - spacing, i.height - spacing), CV_RGB(255, 137, 0), CV_FILLED, 8, 0);
    cvRectangle(image, cvPoint(i.width - sizeBox*2 - spacing*2, i.height - sizeBox - spacing), cvPoint(i.width - sizeBox - spacing*2, i.height - spacing), CV_RGB(255, 166, 64), CV_FILLED, 8, 0);
    cvRectangle(image, cvPoint(i.width - sizeBox*3 - spacing*3, i.height - sizeBox - spacing), cvPoint(i.width - sizeBox*2 - spacing*3, i.height - spacing), CV_RGB(255, 190, 115), CV_FILLED, 8, 0);
    cvRectangle(image, cvPoint(i.width - sizeBox*4 - spacing*4, i.height - sizeBox - spacing), cvPoint(i.width - sizeBox*3 - spacing*4, i.height - spacing), CV_RGB(255, 174, 0), CV_FILLED, 8, 0);


    cvPutText(image, "x", cvPoint(i.width - sizeBox - spacing + sizeBox*.2, i.height - sizeBox - spacing + sizeBox/1.25), &font, cvScalar(0, 0, 0));
    cvPutText(image, ">", cvPoint(i.width - sizeBox*2 - spacing*2 + sizeBox*.1, i.height - sizeBox - spacing + sizeBox/1.25), &font, cvScalar(0, 0, 0));
    cvPutText(image, "+", cvPoint(i.width - sizeBox*3 - spacing*3 + sizeBox*.1, i.height - sizeBox - spacing + sizeBox/1.25), &font, cvScalar(0, 0, 0));
    cvPutText(image, "i", cvPoint(i.width - sizeBox*4 - spacing*4 + sizeBox*.32, i.height - sizeBox - spacing + sizeBox/1.2), &font, cvScalar(0, 0, 0));

    cvShowImage(m_window_name.c_str(), image);
}

char Gui::getKey()
{
    return cvWaitKey(10);
}

std::string Gui::windowName()
{
    return m_window_name;
}

static string window_name;
static CvFont font;
static IplImage *img0;
static IplImage *img1;
static CvPoint point;
static CvRect *bb;
static int drag = 0;

static void mouseHandler(int event, int x, int y, int flags, void *param)
{

    /* user press left button */
    if(event == CV_EVENT_LBUTTONDOWN && !drag)
    {
        point = cvPoint(x, y);
        drag = 1;
    }

    /* user drag the mouse */
    if(event == CV_EVENT_MOUSEMOVE && drag)
    {
        img1 = (IplImage *) cvClone(img0);

        cvRectangle(img1, point, cvPoint(x, y), CV_RGB(255, 0, 0), 1, 8, 0);

        cvShowImage(window_name.c_str(), img1);
        cvReleaseImage(&img1);
    }

    /* user release left button */
    if(event == CV_EVENT_LBUTTONUP && drag)
    {
        *bb = cvRect(point.x, point.y, x - point.x, y - point.y);
        drag = 0;
    }
}

// TODO: member of Gui
// --> problem: callback function mouseHandler as member!
int getBBFromUser(IplImage *img, CvRect &rect, Gui *gui, int num, std::string message)
{
    window_name = gui->windowName();
    img0 = (IplImage *) cvClone(img);
    rect = cvRect(-1, -1, -1, -1);
    bb = &rect;
    bool correctBB = false;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
    CvScalar black = CV_RGB(0, 0, 0);
    cvSetMouseCallback(window_name.c_str(), mouseHandler, NULL);
     cvRectangle(img0, cvPoint(0, 0), cvPoint(img0->width, 30), black, CV_FILLED, 8, 0);
    cvPutText(img0, message.c_str(), cvPoint(10, 20),
              &font, cvScalar(255, 255, 0));

    cvShowImage(window_name.c_str(), img0);

    while(!correctBB)
    {
        char key = cvWaitKey(0);

        if(tolower(key) == 'q')
        {
            return PROGRAM_EXIT;
        }

        //Modification by Anshul: removed multi-character constant check
        //(key == '\r\n'). Was getting multi-character character constant
        //warning and decided it was not needed.

        if(((key == '\n') || (key == '\r')) && (bb->x != -1) && (bb->y != -1))
        {
            correctBB = true;
        }
    }

    if(rect.width < 0)
    {
        rect.x += rect.width;
        rect.width = abs(rect.width);
    }

    if(rect.height < 0)
    {
        rect.y += rect.height;
        rect.height = abs(rect.height);
    }

    cvSetMouseCallback(window_name.c_str(), NULL, NULL);

    cvReleaseImage(&img0);
    cvReleaseImage(&img1);

    return SUCCESS;
}

static void mouseHandlerVideo(int event, int x, int y, int flags, void *param)
{

    struct info {
        char key;
        int width;
        int height;
    };
    struct info* i = (struct info*)param;

    int width = i->width;
    int height = i->height;
    int region = 0;


    if(event == CV_EVENT_LBUTTONDOWN)
    {

        if (x > (width - sizeBox - spacing) && x < (width - spacing) && y > (height - sizeBox - spacing) && y < (height - spacing) ){
            i->key = 'q';
        }
        else if (x > (width - sizeBox*2 - spacing*2) && x < (width - sizeBox - spacing*2) && y > (height - sizeBox - spacing) && y < (height - spacing)) {
            i->key = 'p';
        }
        else if (x > (width - sizeBox*3 - spacing*3) && x < (width - sizeBox*2 - spacing*3) && y > (height - sizeBox - spacing) && y < (height - spacing)) {
            i->key = 'a';
        }
        else if (x > (width - sizeBox*4 - spacing*4) && x < (width - sizeBox*3 - spacing*4) && y > (height - sizeBox - spacing) && y < (height - spacing)) {
            i->key = 'i';
        }
        else {
            i->key = 0;
        }
    }
}

char Gui::getVideoKey(){
    cvWaitKey(10);
    char ret = i.key;
    i.key = 0;
    return ret;
}

void Gui::initVideoWindow(int videoX, int videoY)
{
    cvNamedWindow(m_window_name.c_str(), CV_WINDOW_AUTOSIZE);
    cvMoveWindow(m_window_name.c_str(), videoX, videoY);
}

void Gui::setMouseHandler(){
    cvSetMouseCallback(m_window_name.c_str(), mouseHandlerVideo, &i);
}

void Gui::setWidthHeight(int width, int height){
    i.width = width;
    i.height = height;
}

