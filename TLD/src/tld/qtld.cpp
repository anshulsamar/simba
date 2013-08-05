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

        IplImage *img2;

        int spacing = 50;

        double newVideoHeight = videoHeight;
        double dif = desktopHeight/newVideoHeight;
        while (dif < 4){
            newVideoHeight *= (4.0/5);
            dif = desktopHeight/newVideoHeight;
        }
        double newVideoWidth = (newVideoHeight/videoHeight)*videoWidth;

        double analyticsWidth = desktopWidth/4;
        double analyticsHeight = desktopHeight * (4.0/5);

        double graphWidth = (desktopWidth/5)*3;
        double graphHeight = analyticsHeight - newVideoHeight - spacing;

        double commandWidth = graphWidth + analyticsWidth - spacing*2;
        double commandHeight = newVideoHeight;

        double windowWidth = analyticsWidth + graphWidth + spacing;
        double windowHeight = analyticsHeight;
        double windowX = desktopWidth/2 - windowWidth/2;
        double windowY = desktopHeight/2 - windowHeight/2;

        double analyticsX = windowX;
        double analyticsY = windowY;
        double graphX = analyticsX + analyticsWidth + spacing;
        double graphY = analyticsY;
        double commandX = graphX;
        double commandY = graphY + graphHeight + spacing;
        double newVideoX = commandX + commandWidth + spacing;
        double newVideoY = commandY;

        QTextEdit* aWin = new QTextEdit;
        aWin->resize(analyticsWidth, analyticsHeight);
        aWin->setWindowTitle("Analytics and Results");
        aWin->move(analyticsX, analyticsY);
        QString b("QWidget {background-color: black}\nQWidget {color: rgb(255, 255, 255)}\nQWidget {font: 14pt \"Source Sans Pro\"}");
        aWin->setStyleSheet(b);
        aWin->show();
        aWin->insertHtml(QString("Track Learn Detect<br><br>"));

        main->initGui(videoX, videoY);

        /*if (!main->doWork(settings)){
          return EXIT_FAILURE;
        }

        delete main;*/

        Analyze *analyze = new Analyze(aWin, settings->m_resultsDirectory);

        if (!analyze->doWork())
           return EXIT_FAILURE;

        aWin->close();

        delete analyze;
        delete main;
        delete aWin;
        delete settings;
    }

    return EXIT_SUCCESS;
}
