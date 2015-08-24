//missing attribution... todo/can't remember where this is from

#ifndef GLOBAL_H
#define GLOBAL_H

extern "C" {
#include <ccv.h>

struct colors {
    int r;
    int g;
    int b;
};

#define spacing 20
#define sizeBox 30

}

extern ccv_tld_param_t ccv_tld_params;

#endif // GLOBAL_H
