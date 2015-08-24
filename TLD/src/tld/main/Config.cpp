//missing attribution... todo/can't remember where this is from

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

    main->videoPath = m_settings.m_videoPath;
    main->loadIni = m_settings.m_loadIni;
    main->iniPath = m_settings.m_iniPath;
    main->saveIniDirectory = m_settings.m_iniDirectory;
    main->saveResults = m_settings.m_saveResults;
    main->resultsDirectory = m_settings.m_resultsDirectory;

    return SUCCESS;
}
}
