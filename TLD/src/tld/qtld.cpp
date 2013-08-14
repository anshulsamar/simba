#include <cstdio>
#include <iostream>
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
#include "Main.h"
#include "Settings.h"
#include "ImAcq.h"
#include "Gui.h"
#include "ConfigDialog.h"
#include "Analyze.h"
#include "AnalyzeGui.h"

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
   QDesktopWidget *desktop = QApplication::desktop();
   double desktopHeight = desktop->height();
   double desktopWidth = desktop->width();

   //Loading "Application" Image

   QGraphicsScene scene;
   QGraphicsView view(&scene);
   QGraphicsPixmapItem item(QPixmap("./TLD.app/Contents/Resources/loadingImage.jpg"));
   scene.addItem(&item);
   view.setWindowFlags(Qt::FramelessWindowHint);

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
        app->exec();

        while (err != 1){
            if (err == 0) return EXIT_SUCCESS;
            if (err == 2){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("Either you haven't clicked one of \"track\" or \"analyze\" or didn't provide directories in which to save their results.");
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

        if (settings->m_track){

            imagePath = settings->m_trackImagesPath;

            memset(num, 0, 64);
            sprintf(num, "%07ld", 1L);
            imagePath += QString(num).toStdString();
            imagePath += std::string(".png");

            img = imread(imagePath.c_str(), 1);
            if (!img.data){
                QMessageBox msgBox;
                msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
                msgBox.setText("Track: Unable to read the first image in given directory.");
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
            QString t = QString("Images too big for the size of your screen (") + QString::number(desktopWidth) + QString(" x ") + QString::number(desktopHeight) + QString("). See our tutorial on using ffmpeg for image size changing.");
            msgBox.setText(t);
            msgBox.exec();
            delete settings;
            continue;

        }

        double videoX = desktopWidth/2 - (double)videoWidth/2;
        double videoY = desktopHeight/2 - (double)videoHeight/2;

        if (settings->m_track){
            Main *main = new Main(settings);
            main->initGui(videoX, videoY);

            err = main->doWork(settings);

            if (err == 0){
              QMessageBox msgBox;
              msgBox.setStyleSheet(QString("font: 14pt \"Source Sans Pro\""));
              msgBox.setText("Oops - there was an error. Ok to restart.");
              msgBox.exec();
              delete main;
              delete settings;
              continue;
            }

            delete main;
        }

        if (settings->m_analyze){

            int spacing = 50;

            double controlWidth = desktopWidth/4;
            double controlHeight = desktopHeight * (4.0/5);
            double smallVideoHeight = videoHeight;
            double smallVideoWidth = videoWidth;
            double fx;
            double fy;

            if (smallVideoHeight > controlHeight/2){
                smallVideoHeight = controlHeight/2;
                fy = smallVideoHeight/videoHeight;
                fx = fy;
                smallVideoWidth = videoWidth * fx;
            }

            while (smallVideoWidth*2 + spacing*4 + controlWidth > desktopWidth){
                smallVideoWidth *= .95;
                smallVideoHeight *= .95;
            }

            double intelWidth = 2*smallVideoWidth + spacing;
            double intelHeight = controlHeight - spacing - smallVideoHeight;

            double windowWidth = controlWidth + intelWidth + spacing;
            double windowHeight = controlHeight;
            double windowX = desktopWidth/2 - windowWidth/2;
            double windowY = desktopHeight/2 - windowHeight/2;

            double controlX = windowWidth - controlWidth + windowX;
            double controlY = windowY;
            double intelX = windowX;
            double intelY = windowY;

            double oneX = intelX;
            double oneY = intelY + intelHeight + spacing;
            double twoX = oneX + smallVideoWidth + spacing;
            double twoY = oneY;

            QTextEdit* aWin = new QTextEdit;
            aWin->resize(controlWidth, controlHeight);
            aWin->setWindowTitle("Simba");
            aWin->move(controlX, controlY);
            QString b("QWidget {background-color: black}\nQWidget {color: rgb(255, 255, 255)}\nQWidget {font: 14pt \"Source Sans Pro\"}");
            aWin->setStyleSheet(b);
            aWin->show();
            aWin->append(QString("Track Learn Detect\n"));

            std::string oneName = "One";
            std::string twoName = "Two";
            std::string intelName = "Intel";
            bool ok = true;
            Analyze *analyze = new Analyze(aWin, settings->m_analysisImagesPath, oneName, twoName, intelName, smallVideoWidth, smallVideoHeight, intelWidth, intelHeight, qApp, ok);

            if (ok){

                analyze->initGui(oneX, oneY, twoX, twoY, intelX, intelY);

                err = analyze->doWork();

                if (err == 1){
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
