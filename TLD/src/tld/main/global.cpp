//missing attribution... todo/can't remember where this is from

#include "global.h"

ccv_tld_param_t ccv_tld_params = {
    .win_size = {
        15,
        15,
    },
    .level = 5,
    .min_forward_backward_error = 100,
    .min_eigen = 0.025,
    .min_win = 20,
    .interval = 7,
    .shift = 0.5,
    .top_n = 100,
    .rotation = 0,
    .include_overlap = 0.4,
    .exclude_overlap = 0.7,
    .structs = 40,
    .features = 3,
    .validate_set = 0.5,
    .nnc_same = 0.60,
    .nnc_thres = 0.65,
    .nnc_verify = 0.7,
    .nnc_beyond = 0.8,
    .nnc_collect = 0.5,
    .bad_patches = 100,
    .new_deform = 20,
    .track_deform = 40,
    .new_deform_angle = 20,
    .track_deform_angle = 50,
    .new_deform_scale = 0.02,
    .track_deform_scale = 0.02,
    .new_deform_shift = 0.02,
    .track_deform_shift = 0.02,
};
