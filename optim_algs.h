#include <list>
#include <algorithm>
#include <chrono>

#include "complex.h"

#pragma once

template<typename T> uint32_t check_point(const complex<T> &point, uint32_t max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {
    
        if(z.get_mod() > 2) {

            return i;

        }

        z = z * z + point;
    
    }

    return 0;

};

// Moore neighborhood algorithm

template<typename T> complex<T> clockwise_rotate(complex<T> center, complex<T> current_point, T step_re, T step_im) {

    double epsilon = 0.0000001; //TODO fix this for arbitrary precision numbers

    complex<T> delta_re(step_re, 0);
    complex<T> delta_im(0, step_im);

    std::list<complex<T>> surrounding_list = {

        center + delta_re,
        center + delta_re - delta_im,
        center - delta_im,
        center - delta_im - delta_re,
        center - delta_re,
        center + delta_im - delta_re,
        center + delta_im,
        center + delta_re + delta_im

    };
    
    typename std::list<complex<T>>::iterator it;
    it = std::find(surrounding_list.begin(), surrounding_list.end(), current_point);

    if(it != surrounding_list.end()) {

        return *(++it);

    } else {

        return surrounding_list.front();

    }

};

template<typename T> uint32_t test_box(complex<T> point){

    double epsilon = 0.000000000001; 

    if(abs(point.get_mod() - 0.5) <= 0.1) {

        return 0;

    } else {

        return 1;

    }

};

template<typename T> std::list<complex<T>> border_trace(
    
    const complex<T> &point, 
    const complex<T> &previous, 
    T step_re, 
    T step_im, 
    uint32_t max_iter
    
) {

    std::list<complex<T>> border_points;
    border_points.push_back(point);

    complex<T> current_border_point = point;
    complex<T> backtrack = previous;

    complex<T> current_point = clockwise_rotate<T>(current_border_point, backtrack, step_re, step_im);

    uint32_t iters = 0; //REMOVE THIS

    while(current_point != point) {
        
        if(check_point(current_point, max_iter) >= 12) {

            border_points.push_back(current_point);
            backtrack = current_border_point;
            current_border_point = current_point;
            current_point = clockwise_rotate<T>(current_border_point, backtrack, step_re, step_im);

        } else {

            backtrack = current_point;
            current_point = clockwise_rotate<T>(current_border_point, backtrack, step_re, step_im);

        }

    }

    return border_points;

};

template<typename T> T map(T old_max, T old_min, T new_max, T new_min, T value) {

    return (value - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;

}