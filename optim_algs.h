#include <list>
#include <algorithm>
#include <queue>

#include "complex.h"

#pragma once

float lerp(float a, float b, float t) {
    
    return a + t * (b - a);

}

template<typename T> float check_point(const complex<T> &point, uint32_t max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {
    
        //TODO Fix escape radius for BW render (same for bordetrace)

        if(z.get_mod() > 16) {  

            float log_z = log(z.get_mod_sqrd()) / 2;
            float nu = log(log_z / log(2)) / log(2);

            return (float)(i) + 1 - nu;

        }

        z = z * z + point;
    
    }

    return max_iter;

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
    std::unique_ptr<float[]>& computed_pixels

) {

    uint32_t current_iter = max_iter + 1;
    uint32_t current_index = get_array_position(current_point, width, height);

    if(computed_pixels[current_index] != current_iter) {
        
        current_iter = computed_pixels[current_index];

    } else {

        current_iter = check_point(current_point, max_iter);
        computed_pixels[current_index] = current_iter;

    }

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

        if(border_existence[i] && ceil(computed_pixels[point_index]) == max_iter + 1) {
            
            float neighbor_iter = check_point(point_to_check, max_iter);
            
            if(ceil(neighbor_iter) != ceil(current_iter)) {
            
                pixel_queue.push(point_to_check);
                computed_pixels[point_index] = neighbor_iter;

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

            if (border_existence[i] && border_existence[j] && ceil(computed_pixels[point_index]) == max_iter + 1) {
                
                float neighbor_iter = check_point(point_to_check, max_iter);

                if(ceil(neighbor_iter) != ceil(current_iter)) {
            
                    pixel_queue.push(point_to_check);
                    computed_pixels[point_index] = neighbor_iter;

                }

            }    

        }

    }

}

void color_all(
    
    uint32_t width, 
    uint32_t height, 
    uint32_t max_iter,
    std::unique_ptr<float[]>& computed_pixels
    
) {

    for (size_t i = 0; i < height; ++i) {

        uint32_t vertical_position = i * width;
        float current_iter = computed_pixels[vertical_position];

        for (size_t j = 0; j < width; ++j) {
            
            uint32_t full_position = vertical_position + j;
            float current_value = computed_pixels[full_position];

            if(ceil(current_value) == max_iter + 1) {
                
                computed_pixels[full_position] = current_iter;

            } else if(ceil(current_value) != max_iter + 1 && ceil(current_value) != current_iter) {

                current_iter = current_value;

            }
        
        }
        
    }

}

template<typename T> std::unique_ptr<float[]> border_trace(    
    
    std::vector<complex<T>> vertices,
    uint32_t width,
    uint32_t height,
    T step_re, 
    T step_im, 
    uint32_t max_iter
    
) {

    /* The frame is completely descriped by two opposite vertices (represented by their relative complex points). 
    ** Here the bottom-left and upper right are chosen, encoded in a vector as follows:
    ** {[0]: bottom-left, [1]: upper_right}
    */

    std::queue<complex<T>> pixel_queue;
    std::unique_ptr<float[]> computed_pixels(new float[width * height]);

    for (size_t i = 0; i < height; ++i) {
            
        for (size_t j = 0; j < width; ++j) {
    
            uint32_t position = j + i * width;
            computed_pixels[position] = max_iter + 1;     

        }

    }

    //Add image edges to pixel queue

    for (T im = vertices[0].get_im(); im <= vertices[1].get_im(); im += step_im) {

        pixel_queue.push(complex<T>(vertices[0].get_re(), im));
        pixel_queue.push(complex<T>(vertices[1].get_re(), im));

    }
    
    for (T re = vertices[0].get_re(); re <= vertices[1].get_re(); re += step_re) {
        
        pixel_queue.push(complex<T>(re, vertices[0].get_im()));
        pixel_queue.push(complex<T>(re, vertices[1].get_im()));     

    }

    while (!pixel_queue.empty()) {
        
        check_neighbours(pixel_queue.front(), width, height, step_re, step_im, max_iter, vertices, pixel_queue, computed_pixels);
        pixel_queue.pop();

    }

    color_all(width, height, max_iter, computed_pixels);

    return computed_pixels;

}

