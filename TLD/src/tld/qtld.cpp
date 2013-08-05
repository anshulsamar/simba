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

   QApplication app(argc, argv);
   QDesktopWidget *desktop = QApplication::desktop();
   double desktopHeight = desktop->height();
   double desktopWidth = desktop->width();

   //std::cout << QString::number(desktopHeight).toStdString() + QString(" ").toStdString() + QString::number(desktopWidth).toStdString() << std::endl;

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

        Main *main = new Main();
        Settings *settings = new Settings();

        bool correctClosed;
        ConfigDialog w(settings, &correctClosed);
        w.show();
        app.exec();

        if (!correctClosed)
            return EXIT_FAILURE;

        Config config(*settings);
        if (!config.configure(main)){
            return EXIT_FAILURE;
        }

        //Window formatting, creation, and sizing

        std::string imagePath = settings->m_videoPath;
        char num[64];
        memset(num, 0, 64);
        sprintf(num, "%07ld", 1L);
        imagePath += QString(num).toStdString();
        imagePath += std::string(".png");

        IplImage *img = cvLoadImage(imagePath.c_str());
        if (!img)
            return EXIT_FAILURE;

        double videoWidth = img->height;
        double videoHeight = img->width;

        double videoX = desktopWidth/2 - videoWidth/2;
        double videoY = desktopHeight/2 - videoHeight/2;

          // std::cout << QString::number(videoHeight).toStdString() + QString(" ").toStdString() + QString::number(videoWidth).toStdString() << std::endl;
          // std::cout << QString::number(videoX).toStdString() + QString(" ").toStdString() + QString::number(videoY).toStdString() << std::endl;

        main->initGui(videoX, videoY);

        if (!main->doWork(settings)){
            return EXIT_FAILURE;
        }

        delete main;

        IplImage *img2;

        Analyze *analyze = new Analyze();
        analyze->initGui(desktopWidth, desktopHeight, img2);
        if (!analyze->doWork(settings->m_resultsDirectory, img2))
           return EXIT_FAILURE;


        delete analyze;

        delete settings;
    }

    return EXIT_SUCCESS;
}
