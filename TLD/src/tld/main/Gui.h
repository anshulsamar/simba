/* Modified by Anshul Samar
 * Previous Credits:
*  Copyright 2011 AIT Austrian Institute of Technology
*
*   This file is part of OpenTLD.
*
*   OpenTLD is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   OpenTLD is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with OpenTLD.  If not, see <http://www.gnu.org/licenses/>.
*
*/

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
