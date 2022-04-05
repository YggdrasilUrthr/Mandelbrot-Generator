/**
 * @ Author: Giorgio Chiurato
 * @ Create Time: 2022-03-17 15:02:29
 * @ Modified by: Giorgio Chiurato
 * @ Modified time: 2022-03-18 20:42:07
 * @ Description: additional algorithm needed by mandelbrot.h
 */

#include <list>
#include <algorithm>
#include <queue>

#include "complex.h"

#pragma once

/* A very basic struct describing an image frame and its properties, such as width, height and pixel data.
*/

template<typename T> struct frame_data {

    frame_data(
        
        uint32_t width, 
        uint32_t height, 
        complex<T> bottom_left, 
        complex<T> upper_right,
        uint32_t y,
        uint32_t x
        
    ) {

        m_size.push_back(width);
        m_size.push_back(height);
        m_vertices.push_back(bottom_left);
        m_vertices.push_back(upper_right);
        m_idx.push_back(y);
        m_idx.push_back(x);

        m_pixel_data = std::unique_ptr<uint32_t[]>(new uint32_t[m_size[0] * m_size[1]]);

    };

    frame_data(

        std::vector<uint32_t> size,
        std::vector<complex<T>> vertices,
        std::vector<uint32_t> idx

    ) : m_size(size), m_vertices(vertices), m_idx(idx) {

        m_pixel_data = std::unique_ptr<uint32_t[]>(new uint32_t[m_size[0] * m_size[1]]);

    };

    // m_idx represents the frame index when using a frame array in multithreading operation.

    std::vector<uint32_t> m_size;
    std::vector<complex<T>> m_vertices;
    std::unique_ptr<uint32_t[]> m_pixel_data;
    std::vector<uint32_t> m_idx;
    std::vector<uint32_t> m_ref_iters;
    complex<T> m_center;

};

/* The two following functions check whether a point belongs to the the set, using respectively the stanard escape algorithm
// and the formula optained applying the pertubartion theory (hence the need to pass a precomputed reference iteration vector).
// for a better comprehension of the two algorithm see:
// https://en.wikipedia.org/wiki/Mandelbrot_set
// https://fractaltodesktop.com/perturbation-theory/index.html#:~:text=If%20we%20say%20that%20the,and%20Xn%20is%20the
*/
template<typename T> uint32_t check_point(complex<T> point, uint32_t max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {

        if(z.get_mod() > 2) {  

            return i;

        }

        z = z * z + point;
    
    }

    return max_iter;

};

template<typename T> uint32_t check_point(complex<T> point, complex<T> center, std::vector<complex<double>> ref_iters) {

    uint32_t max_iter = ref_iters.size();
    
    complex<double> z(0.0, 0.0);

    complex<T> delta_0_arb = point - center;
    complex<double> delta_0(

        static_cast<double>(delta_0_arb.get_re()),
        static_cast<double>(delta_0_arb.get_im())

    );
    complex<double> delta = delta_0;

    for (size_t i = 0; i < max_iter; ++i) {
        
        if(z.get_mod() > 2) {

            return i;

        }

        delta = delta * (ref_iters[i] + delta) + delta_0;
        z = (ref_iters[i] / 2.0) + delta; 


    }
    
    return max_iter;

}

/* The following function apply the escape algorithm to the center points and saves each iteration step to a vector, which will
// later be used as a reference point for the computations based on perturbation theory.
*/

template<typename T> std::vector<complex<double>> generate_iter_vector(const complex<T> center, uint32_t max_iter) {

    std::vector<complex<double>> iter_vector;

    complex<T> z(0.0, 0.0);

    for (size_t i = 0; i < max_iter; ++i) {

        z = z * z + center;

        complex<double> z_double(static_cast<double>(2.0 * z.get_re()), static_cast<double>(2.0 * z.get_im())); 
        iter_vector.push_back(z_double);

    }

    return iter_vector;

}

/* This function (name inspired from the well-known function available on Arduino), maps an interval onto a different one.
*/

template<typename T> T map(T old_max, T old_min, T new_max, T new_min, T value) {

    return (value - old_min) * (new_max - new_min) / (old_max - old_min) + new_min;

}

/* This fucntion computes the complex number at the center of a given pixel (represented by its index in a pixel array), knowing the
// image dimensions and the corresponding vertices on the complex plane.
*/

template<typename T> complex<T> get_point_from_index(std::vector<complex<T>> vertices, size_t idx, uint32_t width, uint32_t height) {

    uint32_t j = idx % width;
    uint32_t i = static_cast<uint32_t>((idx - j) / width);

    complex<T> point;
    point.set_re(map<T>(static_cast<T>(width), 0.0, vertices[1].get_re(), vertices[0].get_re(), static_cast<T>(j)));
    point.set_im(map<T>(static_cast<T>(height), 0.0, vertices[1].get_im(), vertices[0].get_im(), static_cast<T>(i))); 

    return point;

}

/* This function checks if any pixel surrounding a center has a different iteration number, thus identifyng a lemniscate. 
*/

template<typename T> void check_neighbours(
    
    size_t current_index,
    uint32_t width,
    uint32_t height,
    uint32_t max_iter, 
    std::vector<complex<T>> vertices,
    std::queue<size_t>& pixel_queue,
    std::unique_ptr<uint32_t[]>& computed_iters,
    std::function<uint32_t(complex<T>)> check_point_optim

) {

    uint32_t current_iter = max_iter + 1;
    complex<T> current_point = get_point_from_index(vertices, current_index, width, height);


    if(computed_iters[current_index] != current_iter) {
        
        current_iter = computed_iters[current_index];

    } else {

        current_iter = check_point_optim(current_point);
        computed_iters[current_index] = current_iter;

    }

    /* Check if the four points along the main axis do exist (i.e. the current point is not on the border of the image), and
    ** encode the respective boolean values as follows:
    ** {[0]: right, [1]: left, [2]: up, [3]: down}
    */

   uint32_t x_idx = current_index % width;
   uint32_t y_idx = (current_index - x_idx) / width; 

    bool border_existence[] = {

        x_idx < width - 1,
        x_idx > 0,
        y_idx < height - 1,
        y_idx > 0

    }; 

    /* Store the complex offsets which represents a 1-pixel movement along each axis in bot directions, saved as done for the existence boolean values.
    ** This is useful so as to condense many similar if conditions into a single for loop, cleaning up the code.
    ** {[0]: right, [1]: left, [2]: up, [3]: down}
    */

    long offsets[] = {1, -1, width, -width};

    /* Check if the four pixels along the main axis (if they exist and have not been already checked) are border points, in which
    ** case they are added to the output point list.
    */

    for (uint8_t i = 0; i < 4; ++i) {
        
        uint32_t point_index = current_index + offsets[i];
        complex<T> point_to_check = get_point_from_index(vertices, point_index, width, height);

        if(border_existence[i] && computed_iters[point_index] == max_iter + 1) {
            
            uint32_t neighbor_iter = check_point_optim(point_to_check);
            
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
    
                uint32_t neighbor_iter = check_point_optim(point_to_check);

                if(neighbor_iter != current_iter) {
            
                    pixel_queue.push(point_index);

                    computed_iters[point_index] = neighbor_iter;

                }

            }    

        }

    }

}

/* This function colors all the uncomputed pixel, scanning the image horizontally and setting a pixel color equally to the
// pixel on the left. The color changes automatically when hitting a lemniscate.
*/ 

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


