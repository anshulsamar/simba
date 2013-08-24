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

//Same notice above extends to the ConfigDialog.ui file

/**
  * @author Clemens Korner
  */


#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui/QApplication>
#include <QFileDialog>
#include <QDialog>
#include "ui_ConfigDialog.h"
#include "Settings.h"

class ConfigDialog: public QDialog
{
    Q_OBJECT
public:
    ConfigDialog() {}
    ConfigDialog(Settings *settings, int* err, QWidget *parent = 0);
    ~ConfigDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Settings *m_settings;
    int *err;
    Ui_TrackLearnDetect *ui;

private slots:
    void on_pushButton_loadTrackImages_clicked();
    void on_pushButton_loadAnalysisImages_clicked();
    void on_pushButton_saveResults_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

};

#endif // CONFIGDIALOG_H
