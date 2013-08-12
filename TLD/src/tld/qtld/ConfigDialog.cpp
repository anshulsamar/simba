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

ConfigDialog::ConfigDialog(Settings *settings, int *err, QWidget *parent) :
    QDialog(parent),
    m_settings(settings),
    ui(new Ui_TrackLearnDetect)
{
    this->err = err;
    ui->setupUi(this);

}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

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

void ConfigDialog::on_pushButton_loadTrackImages_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Select directory containing images"),
                   QDir::currentPath());
    ui->trackImagesPath->setText(file);

}

void ConfigDialog::on_pushButton_loadAnalysisImages_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Select directory containing images"),
                                                     QDir::currentPath());
    ui->analysisImagesPath->setText(file);
}

void ConfigDialog::on_pushButton_saveResults_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Output Directory for Results"),
                   QDir::currentPath());
    ui->resultsDirectory->setText(file);
}

void ConfigDialog::on_pushButton_saveAnalysis_clicked()
{
    QString file = QFileDialog::getExistingDirectory(this, tr("Output directory for Ini file"),
                   QDir::currentPath());
    ui->saveAnalysisDirectory->setText(file);
}

void ConfigDialog::on_buttonBox_accepted()
{
    m_settings->m_track = ui->loadTrackImages->isChecked();
    m_settings->m_analyze = ui->loadAnalysisImages->isChecked();
    m_settings->m_saveResults = ui->saveResults->isChecked();
    m_settings->m_saveAnalysis = ui->saveAnalysis->isChecked();

    if ((!m_settings->m_track && !m_settings->m_analyze) || (m_settings->m_track && !m_settings->m_saveResults) || (m_settings->m_analyze && !m_settings->m_saveAnalysis) ){
        *err = 2;
        close();
        return;
    }

    if ((m_settings->m_track == true && ui->trackImagesPath->text().isEmpty()) || (m_settings->m_analyze == true && ui->analysisImagesPath->text().isEmpty()) || (m_settings->m_saveResults == true && ui->resultsDirectory->text().isEmpty()) || (m_settings->m_saveAnalysis ==true && ui->saveAnalysisDirectory->text().isEmpty())){
        *err = 3;
        close();
        return;
    }

    if(!ui->trackImagesPath->text().isEmpty()){
        m_settings->m_trackImagesPath = ui->trackImagesPath->text().toStdString();
        if (m_settings->m_trackImagesPath[m_settings->m_trackImagesPath.length() - 1] != '/')
            m_settings->m_trackImagesPath += std::string("/");
    }

    if(!ui->analysisImagesPath->text().isEmpty()){
        m_settings->m_analysisImagesPath = ui->analysisImagesPath->text().toStdString();
        if (m_settings->m_analysisImagesPath[m_settings->m_analysisImagesPath.length() - 1] != '/')
            m_settings->m_analysisImagesPath += std::string("/");
    }

    if(!ui->resultsDirectory->text().isEmpty()){
        m_settings->m_resultsDirectory = ui->resultsDirectory->text().toStdString();
        if (m_settings->m_resultsDirectory[m_settings->m_resultsDirectory.length() - 1] != '/')
            m_settings->m_resultsDirectory += std::string("/");
    }

    if(!ui->saveAnalysisDirectory->text().isEmpty()){
        m_settings->m_saveAnalysisDirectory = ui->saveAnalysisDirectory->text().toStdString();
        if (m_settings->m_saveAnalysisDirectory[m_settings->m_saveAnalysisDirectory.length() - 1] != '/')
            m_settings->m_saveAnalysisDirectory += std::string("/");
    }

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
    *err = 1;
    close();
}

void ConfigDialog::on_buttonBox_rejected()
{
    *err = 0;
    close();
}
