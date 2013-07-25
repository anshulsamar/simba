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
   QGraphicsPixmapItem item(QPixmap("/Users/Anshul/Desktop/FrankLab/OpenTLD/loadingImage.jpg"));
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

        int spacing = 50;
        int heightOfBar = 20;

        double windowWidth = 300 + videoWidth + spacing*3; //300 for analytics bar, 75 spacing*3
        double windowHeight = desktop->height() - 100;
        double windowX = desktopWidth/2 - windowWidth/2;
        double windowY = desktopHeight/2 - windowHeight/2;

        double analyticsHeight = windowHeight - 100;
        double analyticsWidth = 300;
        double analyticsX = windowX + spacing;
        double analyticsY = windowY + spacing;

        double graphHeight = windowHeight - videoHeight - spacing*3;
        double graphWidth = videoWidth;
        double graphX = windowX + analyticsWidth + spacing*2;
        double graphY = windowY + videoHeight + spacing*2;

        double videoX = graphX;
        double videoY = windowY + spacing;

        QTextEdit* analytics = new QTextEdit;
        analytics->resize(analyticsWidth, analyticsHeight);
        analytics->setWindowTitle("Analytics and Results");
        analytics->move(analyticsX, analyticsY);
        QString b("QWidget {background-color: black}\nQWidget {color: rgb(255, 255, 255)}\nQWidget {font: 14pt \"Source Sans Pro\"}");
        analytics->setStyleSheet(b);
        analytics->show();
        analytics->append(QString("Track Learn Detect"));

        //Initialize main's gui (opencv)

        main->initGui(analytics, videoX, videoY, graphWidth, graphHeight, graphX, graphY);

        if (!main->doWork()){
            return EXIT_FAILURE;
        }

        delete main;
        analytics->close();
        delete analytics;
        delete settings;
    }

    return EXIT_SUCCESS;
}
