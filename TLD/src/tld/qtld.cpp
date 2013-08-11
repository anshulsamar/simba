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
#include "Config.h"
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

using tld::Config;
using tld::Gui;
using tld::Settings;

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
   waitCondition.wait(&a, 1000);
   view.close();

    while (true){

        //Initializing and creating main

        Settings *settings = new Settings();

        int errno;

        ConfigDialog w(settings, &errno);
        w.show();
        app->exec();

        while (errno != 1){
            if (errno == 0) return EXIT_SUCCESS; //user hits cancel
            if (errno == 2){
                QMessageBox msgBox;
                msgBox.setText("Neither \"track\" nor \"analyze\" have been clicked.");
                msgBox.exec();
            }
            if (errno == 3){
                QMessageBox msgBox;
                msgBox.setText("A directory path is missing.");
                msgBox.exec();
            }
            ConfigDialog w(settings, &errno);
            w.show();
            app->exec();
        }

        std::string imagePath = settings->m_trackImagesPath;
        char num[64];
        memset(num, 0, 64);
        sprintf(num, "%07ld", 1L);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");

        cv::Mat img = imread(imagePath.c_str(), 1);
        if (!img.data){
            QMessageBox msgBox;
            msgBox.setText("Unable to read the first image in given directory.");
            msgBox.exec();
            delete settings;
            continue;
        }

        //Window spacing and formatting

        int videoWidth = img.cols;
        int videoHeight = img.rows;

        double videoX = desktopWidth/2 - (double)videoWidth/2;
        double videoY = desktopHeight/2 - (double)videoHeight/2;

        if (settings->m_track){
            Main *main = new Main(settings);
            main->initGui(videoX, videoY);

            errno = main->doWork(settings);

            if (errno == 0){
              QMessageBox msgBox;
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

            double controlWidth = desktopWidth/5;
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
            double graphX = windowX;
            double graphY = windowY;

            double oneX = graphX;
            double oneY = graphY + intelHeight + spacing;
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

            QTextEdit* intelWin = new QTextEdit;
            intelWin->resize(intelWidth, intelHeight);
            intelWin->setWindowTitle("Intel");
            intelWin->move(graphX, graphY);
            QString c("QWidget {background-color: black}\nQWidget {color: rgb(255, 255, 255)}\nQWidget {font: 14pt \"Source Sans Pro\"}");
            intelWin->setStyleSheet(c);
            intelWin->show();
            intelWin->append(QString("Track Learn Detect\n"));

            std::string oneName = "One";
            std::string twoName = "Two";

            Analyze *analyze = new Analyze(aWin, intelWin, settings->m_analysisImagesPath, oneName, twoName, qApp);
            analyze->initGui(oneX, oneY, twoX, twoY, smallVideoWidth, smallVideoHeight, intelWidth, intelHeight, oneName, twoName);

            errno = analyze->doWork();

            if (errno == 0){
                QMessageBox msgBox;
                msgBox.setText("Oops - there was an error. Ok to restart.");
                msgBox.exec();
            }

            aWin->close();
            delete aWin;
            delete analyze;
        }

        delete settings;
    }

    return EXIT_SUCCESS;
}
