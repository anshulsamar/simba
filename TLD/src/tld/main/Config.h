//missing attribution... todo/can't remember where this is from

#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>
#include <QSettings>

#include <libconfig.h++>

#include "Settings.h"
#include "Main.h"

namespace tld
{

class Config
{
public:

    Config();
    Config(Settings &settings);
    ~Config();
    int configure(Main *main);

private:

    libconfig::Config m_cfg; //!< libconfig++
    Settings m_settings; //!< adjusted settings
    QSettings* settingsIn;

};
}

#endif
