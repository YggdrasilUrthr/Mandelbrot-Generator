#include "complex.h"

#pragma once

template<typename T> bool check_point(const complex<T> &point, uint32_t max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {
    
        if(z.get_mod() > 2) {

            return false;

        }

        z = z * z + point;
    
    }

    return true;

};

template<typename T> T map(T old_max, T old_min, T new_max, T new_min, T value) {

    return ((new_max - new_min) / (old_max - old_min) * value + new_min);

}

bool border_trace();

