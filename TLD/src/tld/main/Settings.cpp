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

/**
 * @author Clemens Korner
 */

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
    m_track(false),
    m_saveResults(false),
    m_analyze(false)

{}

Settings::~Settings()
{}

