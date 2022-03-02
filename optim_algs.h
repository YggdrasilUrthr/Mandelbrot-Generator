#include <list>
#include <algorithm>
#include <queue>
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

template<typename T> T map(T old_max, T old_min, T new_max, T new_min, T value) {

    return (value - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;

}

// Moore neighborhood algorithm

/*template<typename T> complex<T> clockwise_rotate(complex<T> center, complex<T> current_point, T step_re, T step_im) {

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

    while(current_point != point  && iters < 1000000) {
        
        if(check_point(current_point, max_iter) >= 13) {

            border_points.push_back(current_point);
            backtrack = current_border_point;
            current_border_point = current_point;
            current_point = clockwise_rotate<T>(current_border_point, backtrack, step_re, step_im);

        } else {

            backtrack = current_point;
            current_point = clockwise_rotate<T>(current_border_point, backtrack, step_re, step_im);

        }

        ++iters;

    }

    return border_points;

};*/

template<typename T> uint32_t get_array_position(complex<T> point, uint32_t width, uint32_t height) {

    size_t j = static_cast<size_t>(map<T>(2.0, -2.0, width, 0.0, static_cast<T>(point.get_re())));
    size_t i = static_cast<size_t>(map<T>(2.0, -2.0, height, 0.0, static_cast<T>(point.get_im())));

    return j + i * width;

}

template<typename T> void check_neighbours(
    
    complex<T> current_pixel,
    uint32_t width,
    uint32_t height,
    T step_re,
    T step_im, 
    uint32_t max_iter, 
    std::vector<complex<T>> vertices,
    std::queue<complex<T>>& pixel_queue,
    std::shared_ptr<bool[]> checked_point,
    std::list<complex<T>>& border_points

) {

    uint32_t current_iter = check_point(current_pixel, max_iter);
    //checked_point[get_array_position(current_pixel, width, height)] = 1;

    /*if(checked_point[get_array_position(current_pixel, width, height)] != 1){ 
        
        border_points.push_back(current_pixel);
        
    }*/

    bool right_exists = vertices[1].get_re() > current_pixel.get_re();
    bool left_exists = vertices[0].get_re() < current_pixel.get_re();
    bool up_exists = vertices[1].get_im() > current_pixel.get_im();
    bool down_exists = vertices[0].get_im() < current_pixel.get_im();

    if (right_exists && checked_point[get_array_position(current_pixel + complex<T>(step_re, 0.0), width, height)] != 1) {

        if(check_point(current_pixel + complex<T>(step_re, 0.0), max_iter) != current_iter) {

            border_points.push_back(current_pixel + complex<T>(step_re, 0.0));

            pixel_queue.push(current_pixel + complex<T>(step_re, 0.0));
            checked_point[get_array_position(current_pixel + complex<T>(step_re, 0.0), width, height)] = 1;

        }

        if(down_exists && checked_point[get_array_position(current_pixel + complex<T>(step_re, -step_im), width, height)] != 1) {

            if(check_point(current_pixel + complex<T>(step_re, -step_im), max_iter) != current_iter) {

                border_points.push_back(current_pixel + complex<T>(step_re, -step_im));

                pixel_queue.push(current_pixel + complex<T>(step_re, -step_im));
                checked_point[get_array_position(current_pixel + complex<T>(step_re, -step_im), width, height)] = 1;

            }

        }

        if (up_exists && checked_point[get_array_position(current_pixel + complex<T>(step_re, step_im), width, height)] != 1) {

            if(check_point(current_pixel + complex<T>(step_re, step_im), max_iter) != current_iter) {

                border_points.push_back(current_pixel + complex<T>(step_re, step_im));

                pixel_queue.push(current_pixel + complex<T>(step_re, step_im));
                checked_point[get_array_position(current_pixel + complex<T>(step_re, step_im), width, height)] = 1;

            }

        }
        
    }
    if (left_exists && checked_point[get_array_position(current_pixel - complex<T>(step_re, 0.0), width, height)] != 1) {

        if(check_point(current_pixel - complex<T>(step_re, 0.0), max_iter) != current_iter) {

            border_points.push_back(current_pixel - complex<T>(step_re, 0.0));

            pixel_queue.push(current_pixel - complex<T>(step_re, 0.0));
            checked_point[get_array_position(current_pixel - complex<T>(step_re, 0.0), width, height)] = 1;

        }

        if(down_exists && checked_point[get_array_position(current_pixel + complex<T>(-step_re, -step_im), width, height)] != 1) {

            if(check_point(current_pixel + complex<T>(-step_re, -step_im), max_iter) != current_iter) {

                border_points.push_back(current_pixel + complex<T>(-step_re, -step_im));

                pixel_queue.push(current_pixel + complex<T>(-step_re, -step_im));
                checked_point[get_array_position(current_pixel + complex<T>(-step_re, -step_im), width, height)] = 1;

            }

        }

        if (up_exists && checked_point[get_array_position(current_pixel + complex<T>(-step_re, step_im), width, height)] != 1) {

            if(check_point(current_pixel + complex<T>(-step_re, step_im), max_iter) != current_iter) {

                border_points.push_back(current_pixel + complex<T>(-step_re, step_im));

                pixel_queue.push(current_pixel + complex<T>(-step_re, step_im));
                checked_point[get_array_position(current_pixel + complex<T>(-step_re, step_im), width, height)] = 1;

            }

        }

    }
    if (up_exists && checked_point[get_array_position(current_pixel + complex<T>(0.0, step_im), width, height)] != 1) {

        if(check_point(current_pixel + complex<T>(0.0, step_im), max_iter) != current_iter) {

            border_points.push_back(current_pixel + complex<T>(0.0, step_im));

            pixel_queue.push(current_pixel + complex<T>(0.0, step_im));
            checked_point[get_array_position(current_pixel + complex<T>(0.0, step_im), width, height)] = 1;

        }

    }
    if (down_exists && checked_point[get_array_position(current_pixel - complex<T>(0.0, step_im), width, height)] != 1) {

        if(check_point(current_pixel - complex<T>(0.0, step_im), max_iter) != current_iter) {

            border_points.push_back(current_pixel - complex<T>(0.0, step_im));

            pixel_queue.push(current_pixel - complex<T>(0.0, step_im));
            checked_point[get_array_position(current_pixel - complex<T>(0.0, step_im), width, height)] = 1;

        }

    }

}

template<typename T> std::list<complex<T>> border_trace(    
    
    //std::list<complex<T>> vertices,
    uint32_t width,
    uint32_t height,
    T step_re, 
    T step_im, 
    uint32_t max_iter
    
) {

    //WARNING: Hard-coded vertices need to be changed.

    complex<T> bottom_left(-2.0, -2.0);
    complex<T> upper_right(2.0, 2.0);

    std::vector<complex<T>> test_vertices = {bottom_left, upper_right}; 

    std::queue<complex<T>> pixel_queue;
    std::shared_ptr<bool[]> already_checked(new bool[width * height]);

    for (size_t i = 0; i < height; ++i) {
            
        for (size_t j = 0; j < width; ++j) {
    
            uint32_t position = j + i * width;
            already_checked[position] = 0;     

        }

    }

    //Add image edges to pixel queue

    for (T im = bottom_left.get_im(); im <= upper_right.get_im(); im += step_im) {

        pixel_queue.push(complex<T>(bottom_left.get_re(), im));
        pixel_queue.push(complex<T>(upper_right.get_re(), im));

    }
    
    for (T re = bottom_left.get_re(); re <= upper_right.get_re(); re += step_re) {
        
        pixel_queue.push(complex<T>(re, bottom_left.get_im()));
        pixel_queue.push(complex<T>(re, upper_right.get_im()));     

    }

    std::list<complex<T>> border_points;

    while (!pixel_queue.empty()) {
        
        check_neighbours(pixel_queue.front(), width, height, step_re, step_im, max_iter, test_vertices, pixel_queue, already_checked, border_points);
        pixel_queue.pop();
        //std::cout << pixel_queue.size() << std::endl;

    }

    return border_points;

}

