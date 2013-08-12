/* ConfigDialog.h.
 * --------------
 * Modified by Anshul Samar.
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
    void on_pushButton_saveAnalysis_clicked();
    void on_pushButton_saveResults_clicked();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();

};

#endif // CONFIGDIALOG_H
