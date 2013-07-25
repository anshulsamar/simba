#include "Config.h"
#include "Main.h"
#include <QSettings>
#include <fstream>

using namespace std;
namespace tld
{

Config::Config() {}
Config::Config(Settings &settings) : m_settings(settings) {}
Config::~Config() {}

int Config::configure(Main *main)
{
    ImAcq *imAcq = main->imAcq;

    imAcq->method = m_settings.m_method;
    imAcq->imgPath = (m_settings.m_videoPath.empty()) ? NULL : m_settings.m_videoPath.c_str();
    imAcq->lastFrame = m_settings.m_lastFrame;
    imAcq->currentFrame = m_settings.m_startFrame;
    imAcq->camNo = m_settings.m_camNo;
    imAcq->fps = m_settings.m_fps;
    main->videoPath = m_settings.m_videoPath;
    main->loadIni = m_settings.m_loadIni;
    main->iniPath = m_settings.m_iniPath;
    main->saveIniDirectory = m_settings.m_iniDirectory;
    main->saveResults = m_settings.m_saveResults;
    main->resultsDirectory = m_settings.m_resultsDirectory;

    if (main->saveIni){
        QString path = QString(main->saveIniDirectory.c_str()) + QString("tracker.ini");
        std::ifstream outfile(path.toStdString().c_str());
        main->settingsOut = new QSettings(QString(main->iniPath.c_str()), QSettings::IniFormat);
        if (!outfile || main->settingsOut->status() != QSettings::NoError){
            //main->textEdit->append(QString("Failed to create initialization file"));
            std::cerr << "Failed to create initialization file" << std::endl;
            return PROGRAM_EXIT;
        }

    }

    if (main->loadIni){
        std::ifstream infile(main->iniPath.c_str());
        settingsIn = new QSettings(QString(main->iniPath.c_str()), QSettings::IniFormat);
        if (!infile || settingsIn->status() != QSettings::NoError){
            //main->textEdit->append(QString("Failed to open initialization file"));
            std::cerr << "Failed to open initialization file\n" << std::endl;
            return PROGRAM_EXIT;
        }
        if (!settingsIn->contains(QString("info"))){
            //main->textEdit->append(QString("Ini file does not include an info group"));
            std::cerr << "Ini file does not include an info group\n" << std::endl;
            return PROGRAM_EXIT;
        }

        settingsIn->beginGroup("info");
        main->trackersInIniFile = settingsIn->value("numTrackers").toInt();
        settingsIn->endGroup();

        if (main->trackersInIniFile == 0){
            //main->textEdit->append(QString("Ini file does not include data on trackers"));
            std::cerr << "Ini file does not include data on trackers\n" << std::endl;
            return PROGRAM_EXIT;
        }

        for (int i = 0; i < main->trackersInIniFile; i++){
            QString s = QString::number(i);
            string groupName = string("tracker") + s.toStdString();
            settingsIn->beginGroup(groupName.c_str());
            CvRect* add = new CvRect;
            add->x = settingsIn->value("x").toInt();
            add->y = settingsIn->value("y").toInt();
            add->width = settingsIn->value("width").toInt();
            add->height = settingsIn->value("height").toInt();
            main->addTrackerInfo(settingsIn->value("startFrame").toInt(), settingsIn->value("endFrame").toInt(), add);
            settingsIn->endGroup();
        }

        delete settingsIn;
    }


    return SUCCESS;
}
}
