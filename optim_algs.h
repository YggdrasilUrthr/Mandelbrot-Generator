#include <list>
#include <algorithm>
#include <queue>

#include "complex.h"

#pragma once

template<typename T> uint32_t check_point(const complex<T> &point, uint32_t max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {
    
        //TODO Fix escape radius for BW render (same for bordetrace)

        if(z.get_mod() > 2) {  

            return i;

        }

        z = z * z + point;
    
    }

    return max_iter;

};

template<typename T> uint32_t check_point(const complex <T> &point, const complex<T> &ref_point, uint32_t max_iter) {

    double delta = 

}

template<typename T> T map(T old_max, T old_min, T new_max, T new_min, T value) {

    return (value - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;

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
    std::unique_ptr<uint32_t[]>& computed_iters

) {

    uint32_t current_iter = max_iter + 1;
    complex<T> current_point = get_point_from_index(vertices, current_index, width, height);


    if(computed_iters[current_index] != current_iter) {
        
        current_iter = computed_iters[current_index];

    } else {

        current_iter =  check_point(current_point, max_iter);

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

    long offsets[] = {1, -1, width, - width};

    /* Check if the four pixels along the main axis (if they exist and have not been already checked) are border points, in which
    ** case they are added to the output point list.
    */

    for (uint8_t i = 0; i < 4; ++i) {
        
        uint32_t point_index = current_index + offsets[i];
        complex<T> point_to_check = get_point_from_index(vertices, point_index, width, height);

        if(border_existence[i] && computed_iters[point_index] == max_iter + 1) {
            
            uint32_t neighbor_iter = check_point(point_to_check, max_iter);
            
            if(neighbor_iter != current_iter) {
            
                pixel_queue.push(point_index);

                computed_iters[point_index] = neighbor_iter;

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
    
                uint32_t neighbor_iter = check_point(point_to_check, max_iter);

                if(neighbor_iter != current_iter) {
            
                    pixel_queue.push(point_index);

                    computed_iters[point_index] = neighbor_iter;

                }

            }    

        }

    }

}

void color_all(
    
    uint32_t width, 
    uint32_t height, 
    uint32_t max_iter,
    std::unique_ptr<uint32_t[]>& computed_iters
    
) {

    for (size_t i = 1; i < width * height; ++i) {

        if(computed_iters[i] == max_iter + 1) {
            
            computed_iters[i] = computed_iters[i - 1];

        }
        
    }

}

template<typename T> std::unique_ptr<uint32_t[]> border_trace(    
    
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

    for (size_t i = 0; i < height; ++i) {
            
        for (size_t j = 0; j < width; ++j) {
    
            uint32_t position = j + i * width;  
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
        
        check_neighbours(pixel_queue.front(), width, height, step_re, step_im, max_iter, vertices, pixel_queue, computed_iters);
        pixel_queue.pop();

    }

    color_all(width, height, max_iter, computed_iters);

    return computed_iters;

}

