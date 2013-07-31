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

        VideoCapture cap(settings->m_videoPath);
        if (!cap.isOpened())
            return EXIT_FAILURE;

        double videoWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
        double videoHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

        double videoX = desktopWidth/2 - videoWidth/2;
        double videoY = desktopHeight/2 - videoHeight/2;

        main->initGui(videoX, videoY);

        if (!main->doWork()){
            return EXIT_FAILURE;
        }

        delete main;
        delete settings;
    }

    return EXIT_SUCCESS;
}
