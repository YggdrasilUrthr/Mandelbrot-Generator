#include <list>
#include <algorithm>
#include <queue>

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

template<typename T> uint32_t get_array_position(complex<T> point, uint32_t width, uint32_t height) {

    size_t j = static_cast<size_t>(map<T>(2.0, -2.0, width, 0.0, static_cast<T>(point.get_re())));
    size_t i = static_cast<size_t>(map<T>(2.0, -2.0, height, 0.0, static_cast<T>(point.get_im())));

    return j + i * width;

}

template<typename T> void check_neighbours(
    
    complex<T> current_point,
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

    uint32_t current_iter = check_point(current_point, max_iter);
    //checked_point[get_array_position(current_point, width, height)] = 1;

    /*if(checked_point[get_array_position(current_point, width, height)] != 1){ 
        
        border_points.push_back(current_point);
        
    }*/

    /* Check if the four points along the main axis do exist (i.e. the current point is not on the border of the image), and
    ** encode the respective boolean values as follows:
    ** {[0]: right, [1]: left, [2]: up, [3]: down}
    */

    bool border_existence[] = {

        vertices[1].get_re() > current_point.get_re(),
        vertices[0].get_re() < current_point.get_re(),
        vertices[1].get_im() > current_point.get_im(),
        vertices[0].get_im() < current_point.get_im()

    }; 

    /* Store the complex offsets which represents a 1-pixel movement along each axis in bot directions, saved as done for the existence boolean values.
    ** This is useful so as to condense many similar if conditions into a single for loop, cleaning up the code.
    ** {[0]: right, [1]: left, [2]: up, [3]: down}
    */

    complex<T> offsets[] = {

        complex<T>(step_re, 0.0),
        complex<T>(-step_re, 0.0),
        complex<T>(0.0, step_im),
        complex<T>(0.0, -step_im)

    };

    /* Check if the four pixels along the main axis (if they exist and have not been already checked) are border points, in which
    ** case they are added to the output point list.
    */

    for (uint8_t i = 0; i < 4; ++i) {
        
        complex<T> point_to_check = current_point + offsets[i];
        uint32_t point_index = get_array_position(point_to_check, width, height);

        if(border_existence[i] && checked_point[point_index] != 1) {
            
            if(check_point(point_to_check, max_iter) != current_iter) {
            
                border_points.push_back(point_to_check);
                pixel_queue.push(point_to_check);

                checked_point[point_index] = 1;

            }

        }

    }
    
    /* Now to the exact same thing with the four points along the two diagonals. The implementation is basically the same,
    ** except for the for loop being broken into two loops so as to check the point existance pair-by-pair.
    */

    for (uint8_t i = 0; i < 2; ++i) {
       
        for (uint8_t j = 2; j < 4; ++j) {
            
            complex<T> point_to_check = current_point + offsets[i] + offsets[j];
            uint32_t point_index = get_array_position(point_to_check, width, height);

            if (border_existence[i] && border_existence[j] && checked_point[point_index] != 1) {
                
                if(check_point(point_to_check, max_iter) != current_iter) {
            
                    border_points.push_back(point_to_check);
                    pixel_queue.push(point_to_check);

                    checked_point[point_index] = 1;

                }

            }    

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

    //TODO: Hard-coded vertices need to be changed.

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

    }

    return border_points;

}

