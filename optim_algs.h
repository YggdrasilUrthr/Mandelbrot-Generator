#include <list>
#include <algorithm>
#include <queue>

#include "complex.h"

#pragma once

template<typename T> struct escape_data {

    escape_data(uint32_t in_iteration, complex<T> in_point) : m_iteration(in_iteration), m_escaped_point(in_point) {};

    uint32_t m_iteration;
    complex<T> m_escaped_point;

};

template<typename T> float get_smooth_value(escape_data<T> esc_data) {

    float log_z = log(static_cast<T>(esc_data.m_escaped_point.get_mod_sqrd())) / 2.0;
    float nu = log(log_z / log(2)) / log(2);

    return static_cast<float>(esc_data.m_iteration) + 1.0 - nu;

}

template<typename T> escape_data<T> check_point(const complex<T> &point, uint32_t max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {
    
        //TODO Fix escape radius for BW render (same for bordetrace)

        if(z.get_mod() > 16) {  

            return escape_data<T>(i, z);

        }

        z = z * z + point;
    
    }

    return escape_data<T>(max_iter, z);

};

template<typename T> T map(T old_max, T old_min, T new_max, T new_min, T value) {

    return (value - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;

}

template<typename T> uint32_t get_array_position(std::vector<complex<T>> vertices, complex<T> point, uint32_t width, uint32_t height) {

    size_t j = static_cast<size_t>(map<T>(vertices[1].get_re(), vertices[0].get_re(), width, 0.0, static_cast<T>(point.get_re())));
    size_t i = static_cast<size_t>(map<T>(vertices[1].get_im(), vertices[0].get_im(), height, 0.0, static_cast<T>(point.get_im())));

    return j + i * width;

}

template<typename T> complex<T> get_point_from_index(std::vector<complex<T>> vertices, size_t idx, uint32_t width, uint32_t height) {

    uint32_t j = idx % width;
    uint32_t i = static_cast<uint32_t>((idx - j) / width);

    complex<T> point;
    point.set_re(map<T>(static_cast<T>(width), 0.0, vertices[1].get_re(), vertices[0].get_re(), static_cast<T>(j)));
    point.set_im(map<T>(static_cast<T>(height), 0.0, vertices[1].get_im(), vertices[0].get_im(), static_cast<T>(i))); 

    return point;

}

//TODO change names for size_t

template<typename T> void check_neighbours(
    
    size_t current_index,
    uint32_t width,
    uint32_t height,
    T step_re,
    T step_im, 
    uint32_t max_iter, 
    std::vector<complex<T>> vertices,
    std::queue<size_t>& pixel_queue,
    std::unique_ptr<uint32_t[]>& computed_iters,
    std::unique_ptr<float[]>& computed_pixels

) {

    uint32_t current_iter = max_iter + 1;
    complex<T> current_point = get_point_from_index(vertices, current_index, width, height);


    if(computed_iters[current_index] != current_iter) {
        
        current_iter = computed_iters[current_index];

    } else {

        escape_data<T> current_escape = check_point(current_point, max_iter);
        current_iter = current_escape.m_iteration;
        
        computed_iters[current_index] = current_iter;
        computed_pixels[current_index] = get_smooth_value(current_escape);

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

    int32_t offsets[] = {1, -1, width, - width};

    /* Check if the four pixels along the main axis (if they exist and have not been already checked) are border points, in which
    ** case they are added to the output point list.
    */

    for (uint8_t i = 0; i < 4; ++i) {
        
        uint32_t point_index = current_index + offsets[i];
        complex<T> point_to_check = get_point_from_index(vertices, point_index, width, height);

        if(border_existence[i] && computed_iters[point_index] == max_iter + 1) {
            
            escape_data<T> neighbor_escape = check_point(point_to_check, max_iter);
            uint32_t neighbor_iter = neighbor_escape.m_iteration;
            
            if(neighbor_iter != current_iter) {
            
                pixel_queue.push(point_index);

                computed_iters[point_index] = neighbor_iter;
                computed_pixels[point_index] = get_smooth_value(neighbor_escape);

            }

        }

    }
    
    /* Now to the exact same thing with the four points along the two diagonals. The implementation is basically the same,
    ** except for the for loop being broken into two loops so as to check the point existance pair-by-pair.
    */

    for (uint8_t i = 0; i < 2; ++i) {
       
        for (uint8_t j = 2; j < 4; ++j) {
            
            uint32_t point_index = current_index + offsets[i] + offsets[j];
            complex<T> point_to_check = get_point_from_index(vertices, point_index, width, height);

            if (border_existence[i] && border_existence[j] && computed_iters[point_index] == max_iter + 1) {
                
                escape_data<T> neighbor_escape = check_point(point_to_check, max_iter);
                uint32_t neighbor_iter = neighbor_escape.m_iteration;
            
                if(neighbor_iter != current_iter) {
            
                    pixel_queue.push(point_index);

                    computed_iters[point_index] = neighbor_iter;
                    computed_pixels[point_index] = get_smooth_value(neighbor_escape);

                }

            }    

        }

    }

}

void color_all(
    
    uint32_t width, 
    uint32_t height, 
    uint32_t max_iter,
    std::unique_ptr<uint32_t[]>& computed_iters,
    std::unique_ptr<float[]>& computed_pixels
    
) {

    for (size_t i = 0; i < height; ++i) {

        uint32_t vertical_position = i * width;
        
        uint32_t row_iter = computed_iters[vertical_position];
        float row_value = computed_pixels[vertical_position];

        for (size_t j = 0; j < width; ++j) {
            
            uint32_t full_position = vertical_position + j;

            uint32_t current_iter = computed_iters[full_position];
            float current_value = computed_pixels[full_position];

            if(current_iter == max_iter + 1) {
                
                computed_pixels[full_position] = row_value;

            } else if(current_iter != max_iter + 1 && row_iter != current_iter) {

                row_value = current_value;

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

    std::queue<size_t> pixel_queue;

    std::unique_ptr<uint32_t[]> computed_iters(new uint32_t[width * height]);
    std::unique_ptr<float[]> computed_pixels(new float[width * height]);

    for (size_t i = 0; i < height; ++i) {
            
        for (size_t j = 0; j < width; ++j) {
    
            uint32_t position = j + i * width;

            computed_pixels[position] = max_iter + 1;     
            computed_iters[position] = max_iter + 1;

        }

    }

    //Add image edges to pixel queue

    for (size_t i = 0; i < width; ++i) {

        pixel_queue.push(i);
        pixel_queue.push((height - 1) * width + i);

    }
    
    for (size_t i = 0; i < height; ++i) {
        
        pixel_queue.push(i * width);
        pixel_queue.push(width - 1 + i * width);   

    }

    while (!pixel_queue.empty()) {
        
        check_neighbours(pixel_queue.front(), width, height, step_re, step_im, max_iter, vertices, pixel_queue, computed_iters, computed_pixels);
        pixel_queue.pop();

    }

    color_all(width, height, max_iter, computed_iters, computed_pixels);

    return computed_pixels;

}

