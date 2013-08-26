/********************************************************************************
** Form generated from reading UI file 'ConfigDialog.ui'
**
** Created by: Qt User Interface Compiler version 4.8.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QTabWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TrackLearnDetect
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab_general;
    QVBoxLayout *verticalLayout_5;
    QScrollArea *scrollArea_general;
    QWidget *scrollAreaWidgetContents_general;
    QVBoxLayout *verticalLayout_4;
    QGroupBox *loadTrackImages;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_trackImagesPath;
    QLineEdit *trackImagesPath;
    QPushButton *pushButton_loadTrackImages;
    QGroupBox *saveResults;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_saveResultsDirectory;
    QLineEdit *resultsDirectory;
    QPushButton *pushButton_saveResults;
    QGroupBox *loadAnalysisImages;
    QHBoxLayout *horizontalLayout;
    QLabel *label_analysisImagesPath;
    QLineEdit *analysisImagesPath;
    QPushButton *pushButton_loadAnalysisImages;
    QWidget *tab_detector;
    QVBoxLayout *verticalLayout_3;
    QScrollArea *scrollArea_general_2;
    QWidget *scrollAreaWidgetContents_general_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *Lwin_size_width;
    QLineEdit *win_size_width;
    QLabel *Lwin_size_height;
    QLineEdit *win_size_height;
    QLabel *LLevel;
    QLineEdit *level;
    QLabel *Lmin_eigen;
    QLineEdit *min_eigen;
    QLabel *Lmin_forward_backward_error;
    QLineEdit *min_forward_backward_error;
    QLabel *Linterval;
    QLineEdit *interval;
    QLabel *Lshift;
    QLineEdit *shift;
    QLabel *Lmin_win;
    QLineEdit *min_win;
    QLabel *include_overlap;
    QLineEdit *include_overlap_2;
    QLabel *Lexclude_overlap;
    QLineEdit *exclude_overlap;
    QLabel *Lstructs;
    QLineEdit *structs;
    QLabel *Lfeatures;
    QLineEdit *features;
    QLabel *Lvalidate_set;
    QLineEdit *validate_set;
    QLabel *Lnnc_same;
    QLineEdit *nnc_same;
    QLabel *Lnnc_thres;
    QLineEdit *nnc_thres;
    QLabel *Lnnc_verify;
    QLineEdit *nnc_verify;
    QLabel *Lnnc_beyond;
    QLineEdit *nnc_beyond;
    QLabel *Lbad_patches;
    QLineEdit *bad_patches;
    QLabel *Lnew_deform;
    QLineEdit *new_deform;
    QLabel *Ltrack_deform;
    QLineEdit *track_deform;
    QLabel *Lnew_deform_angle;
    QLineEdit *new_deform_angle;
    QLabel *Ltrack_deform_angle;
    QLineEdit *track_deform_angle;
    QLabel *Lnew_deform_scale;
    QLineEdit *new_deform_scale;
    QLabel *Ltrack_deform_scale;
    QLineEdit *track_deform_scale;
    QLabel *Lnew_deform_shift;
    QLineEdit *new_deform_shift;
    QLabel *Ltrack_deform_shift;
    QLineEdit *track_deform_shift;
    QLabel *Ltop_n;
    QLineEdit *top_n;
    QLabel *Lrotation;
    QLineEdit *rotation;
    QLabel *Lnnc_collect;
    QLineEdit *nnc_collect;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TrackLearnDetect)
    {
        if (TrackLearnDetect->objectName().isEmpty())
            TrackLearnDetect->setObjectName(QString::fromUtf8("TrackLearnDetect"));
        TrackLearnDetect->resize(693, 494);
        TrackLearnDetect->setStyleSheet(QString::fromUtf8("font: 14pt \"Source Sans Pro\";"));
        TrackLearnDetect->setSizeGripEnabled(false);
        verticalLayout = new QVBoxLayout(TrackLearnDetect);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(TrackLearnDetect);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setStyleSheet(QString::fromUtf8(""));
        tab_general = new QWidget();
        tab_general->setObjectName(QString::fromUtf8("tab_general"));
        tab_general->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_5 = new QVBoxLayout(tab_general);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        scrollArea_general = new QScrollArea(tab_general);
        scrollArea_general->setObjectName(QString::fromUtf8("scrollArea_general"));
        scrollArea_general->setWidgetResizable(true);
        scrollAreaWidgetContents_general = new QWidget();
        scrollAreaWidgetContents_general->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_general"));
        scrollAreaWidgetContents_general->setGeometry(QRect(0, 0, 621, 379));
        verticalLayout_4 = new QVBoxLayout(scrollAreaWidgetContents_general);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        loadTrackImages = new QGroupBox(scrollAreaWidgetContents_general);
        loadTrackImages->setObjectName(QString::fromUtf8("loadTrackImages"));
        loadTrackImages->setStyleSheet(QString::fromUtf8(""));
        loadTrackImages->setCheckable(true);
        loadTrackImages->setChecked(false);
        horizontalLayout_2 = new QHBoxLayout(loadTrackImages);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_trackImagesPath = new QLabel(loadTrackImages);
        label_trackImagesPath->setObjectName(QString::fromUtf8("label_trackImagesPath"));

        horizontalLayout_2->addWidget(label_trackImagesPath);

        trackImagesPath = new QLineEdit(loadTrackImages);
        trackImagesPath->setObjectName(QString::fromUtf8("trackImagesPath"));

        horizontalLayout_2->addWidget(trackImagesPath);

        pushButton_loadTrackImages = new QPushButton(loadTrackImages);
        pushButton_loadTrackImages->setObjectName(QString::fromUtf8("pushButton_loadTrackImages"));

        horizontalLayout_2->addWidget(pushButton_loadTrackImages);


        verticalLayout_4->addWidget(loadTrackImages);

        saveResults = new QGroupBox(scrollAreaWidgetContents_general);
        saveResults->setObjectName(QString::fromUtf8("saveResults"));
        saveResults->setCheckable(true);
        saveResults->setChecked(false);
        horizontalLayout_4 = new QHBoxLayout(saveResults);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_saveResultsDirectory = new QLabel(saveResults);
        label_saveResultsDirectory->setObjectName(QString::fromUtf8("label_saveResultsDirectory"));

        horizontalLayout_4->addWidget(label_saveResultsDirectory);

        resultsDirectory = new QLineEdit(saveResults);
        resultsDirectory->setObjectName(QString::fromUtf8("resultsDirectory"));

        horizontalLayout_4->addWidget(resultsDirectory);

        pushButton_saveResults = new QPushButton(saveResults);
        pushButton_saveResults->setObjectName(QString::fromUtf8("pushButton_saveResults"));

        horizontalLayout_4->addWidget(pushButton_saveResults);


        verticalLayout_4->addWidget(saveResults);

        loadAnalysisImages = new QGroupBox(scrollAreaWidgetContents_general);
        loadAnalysisImages->setObjectName(QString::fromUtf8("loadAnalysisImages"));
        loadAnalysisImages->setCheckable(true);
        loadAnalysisImages->setChecked(false);
        horizontalLayout = new QHBoxLayout(loadAnalysisImages);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label_analysisImagesPath = new QLabel(loadAnalysisImages);
        label_analysisImagesPath->setObjectName(QString::fromUtf8("label_analysisImagesPath"));

        horizontalLayout->addWidget(label_analysisImagesPath);

        analysisImagesPath = new QLineEdit(loadAnalysisImages);
        analysisImagesPath->setObjectName(QString::fromUtf8("analysisImagesPath"));

        horizontalLayout->addWidget(analysisImagesPath);

        pushButton_loadAnalysisImages = new QPushButton(loadAnalysisImages);
        pushButton_loadAnalysisImages->setObjectName(QString::fromUtf8("pushButton_loadAnalysisImages"));

        horizontalLayout->addWidget(pushButton_loadAnalysisImages);


        verticalLayout_4->addWidget(loadAnalysisImages);

        scrollArea_general->setWidget(scrollAreaWidgetContents_general);

        verticalLayout_5->addWidget(scrollArea_general);

        tabWidget->addTab(tab_general, QString());
        tab_detector = new QWidget();
        tab_detector->setObjectName(QString::fromUtf8("tab_detector"));
        tab_detector->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_3 = new QVBoxLayout(tab_detector);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        scrollArea_general_2 = new QScrollArea(tab_detector);
        scrollArea_general_2->setObjectName(QString::fromUtf8("scrollArea_general_2"));
        scrollArea_general_2->setWidgetResizable(true);
        scrollAreaWidgetContents_general_3 = new QWidget();
        scrollAreaWidgetContents_general_3->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_general_3"));
        scrollAreaWidgetContents_general_3->setGeometry(QRect(0, 0, 606, 1696));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents_general_3);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        Lwin_size_width = new QLabel(scrollAreaWidgetContents_general_3);
        Lwin_size_width->setObjectName(QString::fromUtf8("Lwin_size_width"));

        verticalLayout_2->addWidget(Lwin_size_width);

        win_size_width = new QLineEdit(scrollAreaWidgetContents_general_3);
        win_size_width->setObjectName(QString::fromUtf8("win_size_width"));

        verticalLayout_2->addWidget(win_size_width);

        Lwin_size_height = new QLabel(scrollAreaWidgetContents_general_3);
        Lwin_size_height->setObjectName(QString::fromUtf8("Lwin_size_height"));

        verticalLayout_2->addWidget(Lwin_size_height);

        win_size_height = new QLineEdit(scrollAreaWidgetContents_general_3);
        win_size_height->setObjectName(QString::fromUtf8("win_size_height"));

        verticalLayout_2->addWidget(win_size_height);

        LLevel = new QLabel(scrollAreaWidgetContents_general_3);
        LLevel->setObjectName(QString::fromUtf8("LLevel"));

        verticalLayout_2->addWidget(LLevel);

        level = new QLineEdit(scrollAreaWidgetContents_general_3);
        level->setObjectName(QString::fromUtf8("level"));

        verticalLayout_2->addWidget(level);

        Lmin_eigen = new QLabel(scrollAreaWidgetContents_general_3);
        Lmin_eigen->setObjectName(QString::fromUtf8("Lmin_eigen"));

        verticalLayout_2->addWidget(Lmin_eigen);

        min_eigen = new QLineEdit(scrollAreaWidgetContents_general_3);
        min_eigen->setObjectName(QString::fromUtf8("min_eigen"));

        verticalLayout_2->addWidget(min_eigen);

        Lmin_forward_backward_error = new QLabel(scrollAreaWidgetContents_general_3);
        Lmin_forward_backward_error->setObjectName(QString::fromUtf8("Lmin_forward_backward_error"));

        verticalLayout_2->addWidget(Lmin_forward_backward_error);

        min_forward_backward_error = new QLineEdit(scrollAreaWidgetContents_general_3);
        min_forward_backward_error->setObjectName(QString::fromUtf8("min_forward_backward_error"));

        verticalLayout_2->addWidget(min_forward_backward_error);

        Linterval = new QLabel(scrollAreaWidgetContents_general_3);
        Linterval->setObjectName(QString::fromUtf8("Linterval"));

        verticalLayout_2->addWidget(Linterval);

        interval = new QLineEdit(scrollAreaWidgetContents_general_3);
        interval->setObjectName(QString::fromUtf8("interval"));

        verticalLayout_2->addWidget(interval);

        Lshift = new QLabel(scrollAreaWidgetContents_general_3);
        Lshift->setObjectName(QString::fromUtf8("Lshift"));

        verticalLayout_2->addWidget(Lshift);

        shift = new QLineEdit(scrollAreaWidgetContents_general_3);
        shift->setObjectName(QString::fromUtf8("shift"));

        verticalLayout_2->addWidget(shift);

        Lmin_win = new QLabel(scrollAreaWidgetContents_general_3);
        Lmin_win->setObjectName(QString::fromUtf8("Lmin_win"));

        verticalLayout_2->addWidget(Lmin_win);

        min_win = new QLineEdit(scrollAreaWidgetContents_general_3);
        min_win->setObjectName(QString::fromUtf8("min_win"));

        verticalLayout_2->addWidget(min_win);

        include_overlap = new QLabel(scrollAreaWidgetContents_general_3);
        include_overlap->setObjectName(QString::fromUtf8("include_overlap"));

        verticalLayout_2->addWidget(include_overlap);

        include_overlap_2 = new QLineEdit(scrollAreaWidgetContents_general_3);
        include_overlap_2->setObjectName(QString::fromUtf8("include_overlap_2"));

        verticalLayout_2->addWidget(include_overlap_2);

        Lexclude_overlap = new QLabel(scrollAreaWidgetContents_general_3);
        Lexclude_overlap->setObjectName(QString::fromUtf8("Lexclude_overlap"));

        verticalLayout_2->addWidget(Lexclude_overlap);

        exclude_overlap = new QLineEdit(scrollAreaWidgetContents_general_3);
        exclude_overlap->setObjectName(QString::fromUtf8("exclude_overlap"));

        verticalLayout_2->addWidget(exclude_overlap);

        Lstructs = new QLabel(scrollAreaWidgetContents_general_3);
        Lstructs->setObjectName(QString::fromUtf8("Lstructs"));

        verticalLayout_2->addWidget(Lstructs);

        structs = new QLineEdit(scrollAreaWidgetContents_general_3);
        structs->setObjectName(QString::fromUtf8("structs"));

        verticalLayout_2->addWidget(structs);

        Lfeatures = new QLabel(scrollAreaWidgetContents_general_3);
        Lfeatures->setObjectName(QString::fromUtf8("Lfeatures"));

        verticalLayout_2->addWidget(Lfeatures);

        features = new QLineEdit(scrollAreaWidgetContents_general_3);
        features->setObjectName(QString::fromUtf8("features"));

        verticalLayout_2->addWidget(features);

        Lvalidate_set = new QLabel(scrollAreaWidgetContents_general_3);
        Lvalidate_set->setObjectName(QString::fromUtf8("Lvalidate_set"));

        verticalLayout_2->addWidget(Lvalidate_set);

        validate_set = new QLineEdit(scrollAreaWidgetContents_general_3);
        validate_set->setObjectName(QString::fromUtf8("validate_set"));

        verticalLayout_2->addWidget(validate_set);

        Lnnc_same = new QLabel(scrollAreaWidgetContents_general_3);
        Lnnc_same->setObjectName(QString::fromUtf8("Lnnc_same"));

        verticalLayout_2->addWidget(Lnnc_same);

        nnc_same = new QLineEdit(scrollAreaWidgetContents_general_3);
        nnc_same->setObjectName(QString::fromUtf8("nnc_same"));

        verticalLayout_2->addWidget(nnc_same);

        Lnnc_thres = new QLabel(scrollAreaWidgetContents_general_3);
        Lnnc_thres->setObjectName(QString::fromUtf8("Lnnc_thres"));

        verticalLayout_2->addWidget(Lnnc_thres);

        nnc_thres = new QLineEdit(scrollAreaWidgetContents_general_3);
        nnc_thres->setObjectName(QString::fromUtf8("nnc_thres"));

        verticalLayout_2->addWidget(nnc_thres);

        Lnnc_verify = new QLabel(scrollAreaWidgetContents_general_3);
        Lnnc_verify->setObjectName(QString::fromUtf8("Lnnc_verify"));

        verticalLayout_2->addWidget(Lnnc_verify);

        nnc_verify = new QLineEdit(scrollAreaWidgetContents_general_3);
        nnc_verify->setObjectName(QString::fromUtf8("nnc_verify"));

        verticalLayout_2->addWidget(nnc_verify);

        Lnnc_beyond = new QLabel(scrollAreaWidgetContents_general_3);
        Lnnc_beyond->setObjectName(QString::fromUtf8("Lnnc_beyond"));

        verticalLayout_2->addWidget(Lnnc_beyond);

        nnc_beyond = new QLineEdit(scrollAreaWidgetContents_general_3);
        nnc_beyond->setObjectName(QString::fromUtf8("nnc_beyond"));

        verticalLayout_2->addWidget(nnc_beyond);

        Lbad_patches = new QLabel(scrollAreaWidgetContents_general_3);
        Lbad_patches->setObjectName(QString::fromUtf8("Lbad_patches"));

        verticalLayout_2->addWidget(Lbad_patches);

        bad_patches = new QLineEdit(scrollAreaWidgetContents_general_3);
        bad_patches->setObjectName(QString::fromUtf8("bad_patches"));

        verticalLayout_2->addWidget(bad_patches);

        Lnew_deform = new QLabel(scrollAreaWidgetContents_general_3);
        Lnew_deform->setObjectName(QString::fromUtf8("Lnew_deform"));

        verticalLayout_2->addWidget(Lnew_deform);

        new_deform = new QLineEdit(scrollAreaWidgetContents_general_3);
        new_deform->setObjectName(QString::fromUtf8("new_deform"));

        verticalLayout_2->addWidget(new_deform);

        Ltrack_deform = new QLabel(scrollAreaWidgetContents_general_3);
        Ltrack_deform->setObjectName(QString::fromUtf8("Ltrack_deform"));

        verticalLayout_2->addWidget(Ltrack_deform);

        track_deform = new QLineEdit(scrollAreaWidgetContents_general_3);
        track_deform->setObjectName(QString::fromUtf8("track_deform"));

        verticalLayout_2->addWidget(track_deform);

        Lnew_deform_angle = new QLabel(scrollAreaWidgetContents_general_3);
        Lnew_deform_angle->setObjectName(QString::fromUtf8("Lnew_deform_angle"));

        verticalLayout_2->addWidget(Lnew_deform_angle);

        new_deform_angle = new QLineEdit(scrollAreaWidgetContents_general_3);
        new_deform_angle->setObjectName(QString::fromUtf8("new_deform_angle"));

        verticalLayout_2->addWidget(new_deform_angle);

        Ltrack_deform_angle = new QLabel(scrollAreaWidgetContents_general_3);
        Ltrack_deform_angle->setObjectName(QString::fromUtf8("Ltrack_deform_angle"));

        verticalLayout_2->addWidget(Ltrack_deform_angle);

        track_deform_angle = new QLineEdit(scrollAreaWidgetContents_general_3);
        track_deform_angle->setObjectName(QString::fromUtf8("track_deform_angle"));

        verticalLayout_2->addWidget(track_deform_angle);

        Lnew_deform_scale = new QLabel(scrollAreaWidgetContents_general_3);
        Lnew_deform_scale->setObjectName(QString::fromUtf8("Lnew_deform_scale"));

        verticalLayout_2->addWidget(Lnew_deform_scale);

        new_deform_scale = new QLineEdit(scrollAreaWidgetContents_general_3);
        new_deform_scale->setObjectName(QString::fromUtf8("new_deform_scale"));

        verticalLayout_2->addWidget(new_deform_scale);

        Ltrack_deform_scale = new QLabel(scrollAreaWidgetContents_general_3);
        Ltrack_deform_scale->setObjectName(QString::fromUtf8("Ltrack_deform_scale"));

        verticalLayout_2->addWidget(Ltrack_deform_scale);

        track_deform_scale = new QLineEdit(scrollAreaWidgetContents_general_3);
        track_deform_scale->setObjectName(QString::fromUtf8("track_deform_scale"));

        verticalLayout_2->addWidget(track_deform_scale);

        Lnew_deform_shift = new QLabel(scrollAreaWidgetContents_general_3);
        Lnew_deform_shift->setObjectName(QString::fromUtf8("Lnew_deform_shift"));

        verticalLayout_2->addWidget(Lnew_deform_shift);

        new_deform_shift = new QLineEdit(scrollAreaWidgetContents_general_3);
        new_deform_shift->setObjectName(QString::fromUtf8("new_deform_shift"));

        verticalLayout_2->addWidget(new_deform_shift);

        Ltrack_deform_shift = new QLabel(scrollAreaWidgetContents_general_3);
        Ltrack_deform_shift->setObjectName(QString::fromUtf8("Ltrack_deform_shift"));

        verticalLayout_2->addWidget(Ltrack_deform_shift);

        track_deform_shift = new QLineEdit(scrollAreaWidgetContents_general_3);
        track_deform_shift->setObjectName(QString::fromUtf8("track_deform_shift"));

        verticalLayout_2->addWidget(track_deform_shift);

        Ltop_n = new QLabel(scrollAreaWidgetContents_general_3);
        Ltop_n->setObjectName(QString::fromUtf8("Ltop_n"));

        verticalLayout_2->addWidget(Ltop_n);

        top_n = new QLineEdit(scrollAreaWidgetContents_general_3);
        top_n->setObjectName(QString::fromUtf8("top_n"));

        verticalLayout_2->addWidget(top_n);

        Lrotation = new QLabel(scrollAreaWidgetContents_general_3);
        Lrotation->setObjectName(QString::fromUtf8("Lrotation"));

        verticalLayout_2->addWidget(Lrotation);

        rotation = new QLineEdit(scrollAreaWidgetContents_general_3);
        rotation->setObjectName(QString::fromUtf8("rotation"));

        verticalLayout_2->addWidget(rotation);

        Lnnc_collect = new QLabel(scrollAreaWidgetContents_general_3);
        Lnnc_collect->setObjectName(QString::fromUtf8("Lnnc_collect"));

        verticalLayout_2->addWidget(Lnnc_collect);

        nnc_collect = new QLineEdit(scrollAreaWidgetContents_general_3);
        nnc_collect->setObjectName(QString::fromUtf8("nnc_collect"));

        verticalLayout_2->addWidget(nnc_collect);

        scrollArea_general_2->setWidget(scrollAreaWidgetContents_general_3);

        verticalLayout_3->addWidget(scrollArea_general_2);

        tabWidget->addTab(tab_detector, QString());

        verticalLayout->addWidget(tabWidget);

        buttonBox = new QDialogButtonBox(TrackLearnDetect);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setStyleSheet(QString::fromUtf8(""));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

        QWidget::setTabOrder(pushButton_loadAnalysisImages, buttonBox);
        QWidget::setTabOrder(buttonBox, analysisImagesPath);

        retranslateUi(TrackLearnDetect);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(TrackLearnDetect);
    } // setupUi

    void retranslateUi(QDialog *TrackLearnDetect)
    {
        TrackLearnDetect->setWindowTitle(QApplication::translate("TrackLearnDetect", "Track Learn Detect", 0, QApplication::UnicodeUTF8));
        loadTrackImages->setTitle(QApplication::translate("TrackLearnDetect", "Track", 0, QApplication::UnicodeUTF8));
        label_trackImagesPath->setText(QApplication::translate("TrackLearnDetect", "Images", 0, QApplication::UnicodeUTF8));
        trackImagesPath->setText(QString());
        pushButton_loadTrackImages->setText(QApplication::translate("TrackLearnDetect", "Browse...", 0, QApplication::UnicodeUTF8));
        saveResults->setTitle(QApplication::translate("TrackLearnDetect", "Save Results", 0, QApplication::UnicodeUTF8));
        label_saveResultsDirectory->setText(QApplication::translate("TrackLearnDetect", "Directory", 0, QApplication::UnicodeUTF8));
        resultsDirectory->setText(QString());
        pushButton_saveResults->setText(QApplication::translate("TrackLearnDetect", "Browse...", 0, QApplication::UnicodeUTF8));
        loadAnalysisImages->setTitle(QApplication::translate("TrackLearnDetect", "Analyze", 0, QApplication::UnicodeUTF8));
        label_analysisImagesPath->setText(QApplication::translate("TrackLearnDetect", "Images", 0, QApplication::UnicodeUTF8));
        analysisImagesPath->setText(QString());
        pushButton_loadAnalysisImages->setText(QApplication::translate("TrackLearnDetect", "Browse...", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_general), QApplication::translate("TrackLearnDetect", "General", 0, QApplication::UnicodeUTF8));
        Lwin_size_width->setText(QApplication::translate("TrackLearnDetect", "win_size_width", 0, QApplication::UnicodeUTF8));
        win_size_width->setText(QApplication::translate("TrackLearnDetect", "15", 0, QApplication::UnicodeUTF8));
        Lwin_size_height->setText(QApplication::translate("TrackLearnDetect", "win_size_height", 0, QApplication::UnicodeUTF8));
        win_size_height->setText(QApplication::translate("TrackLearnDetect", "15", 0, QApplication::UnicodeUTF8));
        LLevel->setText(QApplication::translate("TrackLearnDetect", "level", 0, QApplication::UnicodeUTF8));
        level->setText(QApplication::translate("TrackLearnDetect", "5", 0, QApplication::UnicodeUTF8));
        Lmin_eigen->setText(QApplication::translate("TrackLearnDetect", "min_eigen", 0, QApplication::UnicodeUTF8));
        min_eigen->setText(QApplication::translate("TrackLearnDetect", "0.025", 0, QApplication::UnicodeUTF8));
        Lmin_forward_backward_error->setText(QApplication::translate("TrackLearnDetect", "min_forward_backward_error", 0, QApplication::UnicodeUTF8));
        min_forward_backward_error->setText(QApplication::translate("TrackLearnDetect", "100", 0, QApplication::UnicodeUTF8));
        Linterval->setText(QApplication::translate("TrackLearnDetect", "interval", 0, QApplication::UnicodeUTF8));
        interval->setText(QApplication::translate("TrackLearnDetect", "7", 0, QApplication::UnicodeUTF8));
        Lshift->setText(QApplication::translate("TrackLearnDetect", "shift", 0, QApplication::UnicodeUTF8));
        shift->setText(QApplication::translate("TrackLearnDetect", "0.5", 0, QApplication::UnicodeUTF8));
        Lmin_win->setText(QApplication::translate("TrackLearnDetect", "min_win", 0, QApplication::UnicodeUTF8));
        min_win->setText(QApplication::translate("TrackLearnDetect", "20", 0, QApplication::UnicodeUTF8));
        include_overlap->setText(QApplication::translate("TrackLearnDetect", "include_overlap", 0, QApplication::UnicodeUTF8));
        include_overlap_2->setText(QApplication::translate("TrackLearnDetect", "0.4", 0, QApplication::UnicodeUTF8));
        Lexclude_overlap->setText(QApplication::translate("TrackLearnDetect", "exclude_overlap", 0, QApplication::UnicodeUTF8));
        exclude_overlap->setText(QApplication::translate("TrackLearnDetect", "0.7", 0, QApplication::UnicodeUTF8));
        Lstructs->setText(QApplication::translate("TrackLearnDetect", "structs", 0, QApplication::UnicodeUTF8));
        structs->setText(QApplication::translate("TrackLearnDetect", "40", 0, QApplication::UnicodeUTF8));
        Lfeatures->setText(QApplication::translate("TrackLearnDetect", "features", 0, QApplication::UnicodeUTF8));
        features->setText(QApplication::translate("TrackLearnDetect", "3", 0, QApplication::UnicodeUTF8));
        Lvalidate_set->setText(QApplication::translate("TrackLearnDetect", "validate_set", 0, QApplication::UnicodeUTF8));
        validate_set->setText(QApplication::translate("TrackLearnDetect", "0.5", 0, QApplication::UnicodeUTF8));
        Lnnc_same->setText(QApplication::translate("TrackLearnDetect", "nnc_same", 0, QApplication::UnicodeUTF8));
        nnc_same->setText(QApplication::translate("TrackLearnDetect", "0.60", 0, QApplication::UnicodeUTF8));
        Lnnc_thres->setText(QApplication::translate("TrackLearnDetect", "nnc_thres", 0, QApplication::UnicodeUTF8));
        nnc_thres->setText(QApplication::translate("TrackLearnDetect", "0.65", 0, QApplication::UnicodeUTF8));
        Lnnc_verify->setText(QApplication::translate("TrackLearnDetect", "nnc_verify", 0, QApplication::UnicodeUTF8));
        nnc_verify->setText(QApplication::translate("TrackLearnDetect", "0.7", 0, QApplication::UnicodeUTF8));
        Lnnc_beyond->setText(QApplication::translate("TrackLearnDetect", "nnc_beyond", 0, QApplication::UnicodeUTF8));
        nnc_beyond->setText(QApplication::translate("TrackLearnDetect", "0.8", 0, QApplication::UnicodeUTF8));
        Lbad_patches->setText(QApplication::translate("TrackLearnDetect", "bad_patches", 0, QApplication::UnicodeUTF8));
        bad_patches->setText(QApplication::translate("TrackLearnDetect", "100", 0, QApplication::UnicodeUTF8));
        Lnew_deform->setText(QApplication::translate("TrackLearnDetect", "new_deform", 0, QApplication::UnicodeUTF8));
        new_deform->setText(QApplication::translate("TrackLearnDetect", "20", 0, QApplication::UnicodeUTF8));
        Ltrack_deform->setText(QApplication::translate("TrackLearnDetect", "track_deform", 0, QApplication::UnicodeUTF8));
        track_deform->setText(QApplication::translate("TrackLearnDetect", "40", 0, QApplication::UnicodeUTF8));
        Lnew_deform_angle->setText(QApplication::translate("TrackLearnDetect", "new_deform_angle", 0, QApplication::UnicodeUTF8));
        new_deform_angle->setText(QApplication::translate("TrackLearnDetect", "20", 0, QApplication::UnicodeUTF8));
        Ltrack_deform_angle->setText(QApplication::translate("TrackLearnDetect", "track_deform_angle", 0, QApplication::UnicodeUTF8));
        track_deform_angle->setText(QApplication::translate("TrackLearnDetect", "50", 0, QApplication::UnicodeUTF8));
        Lnew_deform_scale->setText(QApplication::translate("TrackLearnDetect", "new_deform_scale", 0, QApplication::UnicodeUTF8));
        new_deform_scale->setText(QApplication::translate("TrackLearnDetect", "0.02", 0, QApplication::UnicodeUTF8));
        Ltrack_deform_scale->setText(QApplication::translate("TrackLearnDetect", "track_deform_scale", 0, QApplication::UnicodeUTF8));
        track_deform_scale->setText(QApplication::translate("TrackLearnDetect", "0.02", 0, QApplication::UnicodeUTF8));
        Lnew_deform_shift->setText(QApplication::translate("TrackLearnDetect", "new_deform_shift", 0, QApplication::UnicodeUTF8));
        new_deform_shift->setText(QApplication::translate("TrackLearnDetect", "0.02", 0, QApplication::UnicodeUTF8));
        Ltrack_deform_shift->setText(QApplication::translate("TrackLearnDetect", "track_deform_shift", 0, QApplication::UnicodeUTF8));
        track_deform_shift->setText(QApplication::translate("TrackLearnDetect", "0.02", 0, QApplication::UnicodeUTF8));
        Ltop_n->setText(QApplication::translate("TrackLearnDetect", "top_n", 0, QApplication::UnicodeUTF8));
        top_n->setText(QApplication::translate("TrackLearnDetect", "100", 0, QApplication::UnicodeUTF8));
        Lrotation->setText(QApplication::translate("TrackLearnDetect", "rotation", 0, QApplication::UnicodeUTF8));
        rotation->setText(QApplication::translate("TrackLearnDetect", "0", 0, QApplication::UnicodeUTF8));
        Lnnc_collect->setText(QApplication::translate("TrackLearnDetect", "nnc_collect", 0, QApplication::UnicodeUTF8));
        nnc_collect->setText(QApplication::translate("TrackLearnDetect", "0.5", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_detector), QApplication::translate("TrackLearnDetect", "Detector", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TrackLearnDetect: public Ui_TrackLearnDetect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H
