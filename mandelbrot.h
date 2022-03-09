#pragma once

#include <iostream>
#include <memory>
#include <chrono>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <boost/math/interpolators/barycentric_rational.hpp>

#include "optim_algs.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_ITER 100

template<typename T> class mandelbrot_set {

    public:

        enum optimization_type{

            NONE = 0b0000,
            MULTITHREAD = 0b0001,
            BORDER_TRACE = 0b0010,
            PERTURBATION = 0b0100,
            FULL = 0b1111

        };

        enum color_mode{

            COLORS = true,
            BOOLEAN = false

        };

        mandelbrot_set(
        
            uint32_t width = DEFAULT_WIDTH, 
            uint32_t height = DEFAULT_HEIGHT, 
            uint32_t iter = DEFAULT_ITER, 
            color_mode color = BOOLEAN,
            optimization_type optim_type = NONE 
            
        ) : m_width(width), m_height(height), m_iter(iter), m_color_mode(color), m_optim_type(optim_type) {

            m_points = std::unique_ptr<float[]>(new float[m_width * m_height]);

            if (m_color_mode) {

                generate_palette();

            }

        };

        std::unique_ptr<uint8_t[]> compute_pixels();

    private:

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_iter;
        uint32_t m_max_iter_encountered = 0;
        color_mode m_color_mode;
        optimization_type m_optim_type;

        std::unique_ptr<float[]> m_points;
        std::vector<boost::math::barycentric_rational<float>> m_interpolated_RGB;

        void bruteforce_compute();
        void generate_palette();

};

template<typename T> void mandelbrot_set<T>::generate_palette() {

    std::vector<float> x_ctrl = {0, 0.16, 0.42, 0.6425, 0.8575, 1.0};
    std::vector<float> R_ctrl = {0., 32., 237., 255., 0., 0.};
    std::vector<float> G_ctrl = {7., 107., 255., 170., 2., 7.};
    std::vector<float> B_ctrl = {100., 203., 255., 0., 0., 100.};

    m_interpolated_RGB.push_back(boost::math::barycentric_rational<float>(x_ctrl.data(), R_ctrl.data(), x_ctrl.size()));
    m_interpolated_RGB.push_back(boost::math::barycentric_rational<float>(x_ctrl.data(), G_ctrl.data(), x_ctrl.size()));
    m_interpolated_RGB.push_back(boost::math::barycentric_rational<float>(x_ctrl.data(), B_ctrl.data(), x_ctrl.size()));

}

template<typename T> void mandelbrot_set<T>::bruteforce_compute(){

    for (size_t i = 0; i < m_height; ++i) {
        
        for (size_t j = 0; j < m_width; ++j) {

            complex<T> point;
            point.set_re(map<T>(static_cast<T>(m_width), 0.0, 2.0, -2.0, static_cast<T>(j)));
            point.set_im(map<T>(static_cast<T>(m_height), 0.0, 2.0, -2.0, static_cast<T>(i))); 

            uint32_t position = j + i * m_width;
            
            m_points[position] = check_point<T>(point, m_iter);

        }

    }
        
}

template<typename T> std::unique_ptr<uint8_t[]> mandelbrot_set<T>::compute_pixels() {

    //TODO This vertices may be changed via GUI or externally.

    complex<T> bottom_left(-2.0, -2.0);
    complex<T> upper_right(2.0, 2.0);

    std::vector<complex<T>> frame_vertices = {bottom_left, upper_right}; 

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    if(m_optim_type == NONE) {

        bruteforce_compute();  

    } else if(m_optim_type == BORDER_TRACE) {

        T re_delta = static_cast<T>(4.0 * 1.0 / m_width);
        T im_delta =  static_cast<T>(4.0 * 1.0 / m_height);

        m_points = border_trace<T>(frame_vertices, m_width, m_height, re_delta, im_delta, m_iter);

    }
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[m_width * m_height * 3]);

    if (m_color_mode) {

        for (size_t i = 0; i < m_height; ++i) {

            for (size_t j = 0; j < m_width; ++j) {

                uint32_t position = j + i * m_width;

                if (m_points[position] != m_iter) {

                    float x_value = m_points[position] / m_iter;

                    for (uint8_t i = 0; i < 3; ++i) {
                        
                        pixels[position * 3 + i] = floor(m_interpolated_RGB[i](x_value));

                    }

                } else {

                    for (uint8_t i = 0; i < 3; ++i) {
                        
                        pixels[position * 3 + i] = 0;

                    }

                }

            }
            
        }
    
    } else {

        for (size_t i = 0; i < m_height; ++i) {
            
            for (size_t j = 0; j < m_width; ++j) {

                uint32_t position = j + i * m_width;
                uint8_t luminance;

                if (m_points[position] == 0){
                    
                    luminance = 255;

                } else {

                    luminance = 0;

                }
                
                for (uint8_t k = 0; k < 3; ++k) {

                    pixels[position * 3 + k] = luminance;     
                
                }
                
            }
            
        }

    }
    
    return pixels;
    
}