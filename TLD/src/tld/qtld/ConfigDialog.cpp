/* ConfigDialog.cpp.
 * ----------------
 * Modified by Anshul Samar.
 *
 */

#include <QWidget>
#include <QDesktopWidget>
#include <QMainWindow>
#include <QTextEdit>
#include <iostream>
#include "ConfigDialog.h"
#include "ui_ConfigDialog.h"
#include "Settings.h"


using tld::Settings;

ConfigDialog::ConfigDialog(Settings *settings, bool *correctClosed, QWidget *parent) :
    QDialog(parent),
    m_settings(settings),
    m_correctClosed(correctClosed),
    ui(new Ui_TrackLearnDetect)
{
    *m_correctClosed = false;
    ui->setupUi(this);

}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

//This function is not used

void ConfigDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);

    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/* Gui related functions
 * --------------------
 * Based on the users input in the first window, the settings struct
 * is populated.
 */

void ConfigDialog::on_pushButton_loadVideo_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Select directory containing video images"),
                   QDir::currentPath());
    ui->videoPath->setText(file);

}

void ConfigDialog::on_pushButton_loadIni_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select ini file"),
                   QDir::currentPath());
    ui->iniPath->setText(file);
}

void ConfigDialog::on_pushButton_saveResults_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Output Directory for Results"),
                   QDir::currentPath());
    ui->resultsDirectory->setText(file);
}

void ConfigDialog::on_pushButton_saveIni_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Output directory for Ini file"),
                   QDir::currentPath());
    ui->iniDirectory->setText(file);
}

void ConfigDialog::on_buttonBox_accepted()
{
    m_settings->m_method = IMACQ_VID;
    m_settings->m_loadVideo = ui->loadVideo->isChecked();
    m_settings->m_loadIni = ui->loadIni->isChecked();
    m_settings->m_saveResults = ui->saveResults->isChecked();
    m_settings->m_saveIni = ui->saveIni->isChecked();

    if(!ui->videoPath->text().isEmpty()){
        m_settings->m_videoPath = ui->videoPath->text().toStdString();
        if (m_settings->m_videoPath[m_settings->m_videoPath.length() - 1] != '/'){
            m_settings->m_videoPath += std::string("/");
    }
    }
    else
        m_settings->m_videoPath = "";

    if(!ui->iniPath->text().isEmpty())
        m_settings->m_iniPath = (ui->iniPath->text()).toStdString();
    else
        m_settings->m_iniPath = "";

    if(!ui->resultsDirectory->text().isEmpty())
        m_settings->m_resultsDirectory = ui->resultsDirectory->text().toStdString();
    else
        m_settings->m_resultsDirectory = "";

    if(!ui->iniDirectory->text().isEmpty())
        m_settings->m_iniDirectory = ui->iniDirectory->text().toStdString();
    else
        m_settings->m_iniDirectory = "";

    m_settings->win_size_width = ui->win_size_width->text().toInt();
    m_settings->win_size_height = ui->win_size_height->text().toInt();
    m_settings->level = ui->level->text().toInt();
    m_settings->min_forward_backward_error = ui->min_forward_backward_error->text().toFloat();
    m_settings->min_eigen = ui->min_eigen->text().toFloat();
    m_settings->min_win = ui->min_win->text().toInt();
    m_settings->interval = ui->interval->text().toInt();
    m_settings->shift = ui->shift->text().toFloat();
    m_settings->top_n = ui->top_n->text().toInt();
    m_settings->rotation = ui->rotation->text().toInt();
    m_settings->include_overlap = ui->include_overlap->text().toFloat();
    m_settings->exclude_overlap = ui->exclude_overlap->text().toFloat();
    m_settings->structs = ui->structs->text().toInt();
    m_settings->features = ui->features->text().toInt();
    m_settings->validate_set = ui->validate_set->text().toFloat();
    m_settings->nnc_same = ui->nnc_same->text().toFloat();
    m_settings->nnc_thres = ui->nnc_thres->text().toFloat();
    m_settings->nnc_verify = ui->nnc_verify->text().toFloat();
    m_settings->nnc_beyond = ui->nnc_beyond->text().toFloat();
    m_settings->nnc_collect = ui->nnc_collect->text().toFloat();
    m_settings->bad_patches = ui->bad_patches->text().toInt();
    m_settings->new_deform = ui->new_deform->text().toInt();
    m_settings->track_deform = ui->track_deform->text().toInt();
    m_settings->new_deform_angle = ui->new_deform_angle->text().toFloat();
    m_settings->track_deform_angle = ui->track_deform_angle->text().toFloat();
    m_settings->new_deform_scale = ui->new_deform_scale->text().toFloat();
    m_settings->track_deform_scale = ui->track_deform_scale->text().toFloat();
    m_settings->new_deform_shift = ui->new_deform_shift->text().toFloat();
    m_settings->track_deform_shift = ui->track_deform_shift->text().toFloat();
    *m_correctClosed = true;

    close();
}

void ConfigDialog::on_buttonBox_rejected()
{
    *m_correctClosed = false;
    close();
}
