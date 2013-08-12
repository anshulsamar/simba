#include "Settings.h"

Settings::Settings() :
    win_size_width(15),
    win_size_height(15),
    level(5),
    min_forward_backward_error(100),
    min_eigen(0.025),
    min_win(20),
    interval(3),
    shift(0.1),
    top_n(100),
    rotation(0),
    include_overlap(0.7),
    exclude_overlap(0.2),
    structs(40),
    features(18),
    validate_set(0.5),
    nnc_same(0.95),
    nnc_thres(0.65),
    nnc_verify(0.7),
    nnc_beyond(0.8),
    nnc_collect(0.5),
    bad_patches(100),
    new_deform(20),
    track_deform(10),
    new_deform_angle(20),
    track_deform_angle(10),
    new_deform_scale(0.02),
    track_deform_scale(0.02),
    new_deform_shift(0.02),
    track_deform_shift(0.02),
    m_trackImagesPath(""),
    m_analysisImagesPath(""),
    m_resultsDirectory(""),
    m_saveAnalysisDirectory(""),
    m_track(false),
    m_saveResults(false),
    m_analyze(false),
    m_saveAnalysis(false)

{}

Settings::~Settings()
{}

