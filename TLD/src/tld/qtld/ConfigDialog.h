/* ConfigDialog.h.
 * --------------
 * Modified by Anshul Samar.
 *
 * Other previous credits:
 * @author Clemens Korner
 */

#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui/QApplication>
#include <QFileDialog>
#include <QDialog>
#include "ui_ConfigDialog.h"

namespace tld
{
class Settings;
}

/**
 * Qt-Dialog to configure the program
 */
class ConfigDialog: public QDialog
{
    Q_OBJECT
public:
    /**
     * Creates a dialog
     * @param settings all parameters are stored in this object
     * @param correctClosed true if everything went right, false if not
     */
    ConfigDialog() {}
    ConfigDialog(tld::Settings *settings, bool *correctClosed, QWidget *parent = 0);
    ~ConfigDialog();

protected:
    void changeEvent(QEvent *e);

private:
    tld::Settings *m_settings;
    bool *m_correctClosed;
    Ui_TrackLearnDetect *ui;

private slots:
    void on_pushButton_loadVideo_clicked();
    void on_pushButton_loadIni_clicked();
    void on_pushButton_saveResults_clicked();
    void on_pushButton_saveIni_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

};

#endif // CONFIGDIALOG_H
