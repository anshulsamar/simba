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

/**
  * @author Clemens Korner
  */

#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QTextEdit>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QPixmap>
#include <QMessageBox>
#include <QWaitCondition>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <opencv2/core/core.hpp>
#include <cstdio>
#include <iostream>
#include "Main.h"
#include "Settings.h"
#include "ImAcq.h"
#include "Gui.h"
#include "ConfigDialog.h"
#include "Analyze.h"
#include "AnalyzeGui.h"

//Note to student developer: Any 'C' files (i.e. libccv) must be included with the extern keyword to avoid C++ compiling/prototype mangling issues.

extern "C" {
#include <ccv.h>
}
#ifdef HAVE_AVCODEC
extern "C" {
#include <libavcodec/avcodec.h>
}
#endif
#ifdef HAVE_AVFORMAT
extern "C" {
#include <libavformat/avformat.h>
}
#endif
#ifdef HAVE_SWSCALE
extern "C" {
#include <libswscale/swscale.h>
}
#endif

int main(int argc, char **argv)
{

   QApplication* app = new QApplication(argc, argv);

   //The height and width of the desktop is used for "relative" positioning of other windows
   QDesktopWidget *desktop = QApplication::desktop();
   double desktopHeight = desktop->height();
   double desktopWidth = desktop->width();

   //Loading "Application" Image. This should really be stored elsewhere in the mac bundle (see mac application deployment documentation online), but I have it here for now.
   QGraphicsScene scene;
   QGraphicsView view(&scene);
   QGraphicsPixmapItem item(QPixmap("./TLD.app/Contents/Resources/loadingImage.jpg"));
   scene.addItem(&item);
   view.setWindowFlags(Qt::FramelessWindowHint);

   //Waits 3 seconds before going to the configuration window
   view.show();
   QMutex a;
   a.lock();
   QWaitCondition waitCondition;
   waitCondition.wait(&a, 3000);
   view.close();

    while (true){

        Settings *settings = new Settings();

        int err;

        ConfigDialog w(settings, &err);
        w.show();

        //Starts the event loop so the user can enter values in the config box
        app->exec();

        //Error checking of config options

        while (err != 1){
            if (err == 0) return EXIT_SUCCESS;
            if (err == 2){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("Either you haven't clicked one of \"track\" or \"analyze\" or didn't provide directories in which to save tracker results.");
                msgBox.exec();
            }
            if (err == 3){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("A directory path is missing.");
                msgBox.exec();
            }
            ConfigDialog w(settings, &err);
            w.show();
            app->exec();
        }

        char num[64];
        std::string imagePath;
        cv::Mat img;

        //Error checking of images

        if (settings->m_track){

            imagePath = settings->m_trackImagesPath;

            //Image name: 0000001.png, 0000002.png, etc.
            memset(num, 0, 64);
            sprintf(num, "%07ld", 1L);
            imagePath += QString(num).toStdString();
            imagePath += std::string(".png");

            img = imread(imagePath.c_str(), 1);
            if (!img.data){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("Track: Unable to read the first image in given directory. Make sure images are of the form %07d.png - i.e. 0000001.png, 0000002.png, etc. If you need to convert your video into images, use the shell script on the Simba github page.");
                msgBox.exec();
                delete settings;
                continue;
            }

        }

        if (settings->m_analyze){

            imagePath = settings->m_analysisImagesPath;
            memset(num, 0, 64);
            sprintf(num, "%07ld", 1L);
            imagePath += QString(num).toStdString();
            imagePath += std::string(".png");

            img = imread(imagePath.c_str(), 1);
            if (!img.data){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("Analysis: unable to read the first image in given directory.");
                msgBox.exec();
                delete settings;
                continue;
            }

        }

        //Window spacing and formatting

        int videoWidth = img.cols;
        int videoHeight = img.rows;

        if (videoWidth > desktopWidth || videoHeight > desktopHeight){
            QMessageBox msgBox;
            msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
            QString t = QString("Images too big for the size of your screen (") + QString::number(desktopWidth) + QString(" x ") + QString::number(desktopHeight) + QString("). See our instructions on using ffmpeg for image size changing.");
            msgBox.setText(t);
            msgBox.exec();
            delete settings;
            continue;

        }

        int spacing = 50;
        double controlWidth = desktopWidth/4;
        double controlHeight = videoHeight;
        double videoX = desktopWidth/2 - (double)(videoWidth + controlWidth + spacing)/2;
        double videoY = desktopHeight/2 - (double)videoHeight/2;
        double controlX = videoX + videoWidth + spacing;
        double controlY = videoY;

        if (settings->m_track){

            //Loads parameters from settings into ccv_tld_param_t struct (this will be fed into a tld tracker)

            const ccv_tld_param_t ccv_tld_params = {
               .win_size = {
                   settings->win_size_width,
                   settings->win_size_height,
               },
               .level = settings->level,
               .min_forward_backward_error = settings->min_forward_backward_error,
               .min_eigen = settings->min_eigen,
               .min_win = settings->min_win,
               .interval = settings->interval,
               .shift = settings->shift,
               .top_n = settings->top_n,
               .rotation = settings->rotation,
               .include_overlap = settings->include_overlap,
               .exclude_overlap = settings->exclude_overlap,
               .structs = settings->structs,
               .features = settings->features,
               .validate_set = settings->validate_set,
               .nnc_same = settings->nnc_same,
               .nnc_thres = settings->nnc_thres,
               .nnc_verify = settings->nnc_verify,
               .nnc_beyond = settings->nnc_beyond,
               .nnc_collect = settings->nnc_collect,
               .bad_patches = settings->bad_patches,
               .new_deform = settings->new_deform,
               .track_deform = settings->track_deform,
               .new_deform_angle = settings->new_deform_angle,
               .track_deform_angle = settings->track_deform_angle,
               .new_deform_scale = settings->new_deform_scale,
               .track_deform_scale = settings->track_deform_scale,
               .new_deform_shift = settings->new_deform_shift,
               .track_deform_shift = settings->track_deform_shift,
           };


            QTextEdit* aWin = new QTextEdit;
            aWin->resize(controlWidth, controlHeight);
            aWin->setWindowTitle("Simba");
            aWin->move(controlX, controlY);
            QString b("QWidget {background-color: black}\nQWidget {color: rgb(192,192,192)}\nQWidget {font: 14pt \"Source Sans Pro\"}");
            aWin->setStyleSheet(b);
            aWin->show();
            aWin->append(QString("Track Learn Detect\n"));

            Main *main = new Main(settings, ccv_tld_params, videoX, videoY, aWin);

            err = main->track(settings);

            if (err == 0){
              QMessageBox msgBox;
              msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
              msgBox.setText("Oops - there was an error. Ok to restart.");
              msgBox.exec();
              delete main;
              delete settings;
              aWin->close();
              delete aWin;
              continue;
            }
            if (err == 2){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("Unable to load images.");
                msgBox.exec();
            }

            delete main;
            aWin->close();
            delete aWin;
        }

        if (settings->m_analyze){

            QTextEdit* aWin = new QTextEdit;
            aWin->resize(controlWidth, controlHeight);
            aWin->setWindowTitle("Simba");
            aWin->move(controlX, controlY);
            QString b("QWidget {background-color: black}\nQWidget {color: rgb(192,192,192)}\nQWidget {font: 14pt \"Source Sans Pro\"}");
            aWin->setStyleSheet(b);
            aWin->show();
            aWin->append(QString("Track Learn Detect\n"));

            std::string oneName = "One";
            bool ok = true;
            Analyze *analyze = new Analyze(aWin, settings->m_analysisImagesPath, oneName, qApp, ok);

            if (ok){

                analyze->initGui(videoX, videoY);

                err = analyze->doWork();

                if (err == 2){
                    QMessageBox msgBox;
                    msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                    msgBox.setText("Missing files - make sure ini, txt, and png files aren't misplaced.");
                    msgBox.exec();
                }
            }

            aWin->close();
            delete aWin;
            delete analyze;
        }

        delete settings;
    }

    return EXIT_SUCCESS;
}
