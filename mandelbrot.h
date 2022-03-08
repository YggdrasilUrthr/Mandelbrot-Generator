#pragma once

#include <iostream>
#include <memory>
#include <chrono>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

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
        std::unique_ptr<uint8_t[]> m_palette;

        void bruteforce_compute();
        void generate_palette();

};

template<typename T> void mandelbrot_set<T>::generate_palette() {

    m_palette = std::unique_ptr<uint8_t[]>(new uint8_t[6144]);

    uint8_t ctrl_points[12] = {

        0, 7, 100,
        32, 107, 203,
        237, 255, 255,
        255, 170, 0

    };

    for (size_t i = 0; i < 6144; i += 12) {
        
        for (size_t j = 0; j < 12; ++j) {
            
            m_palette[i + j] = ctrl_points[j];

        }
        

    }

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

        // Need to implement coloring on bordertracing
        //m_points = border_trace<T>(frame_vertices, m_width, m_height, re_delta, im_delta, m_iter);

    }
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[m_width * m_height * 3]);

    if (m_color_mode) {

        for (size_t i = 0; i < m_height; ++i) {
            
            for (size_t j = 0; j < m_width; ++j) {

                uint32_t position = j + i * m_width;

                //for (uint8_t k = 0; k < 3; ++k) {

                    uint32_t palette_position1 = floor(m_points[position]);
                    uint32_t palette_position2 = floor(m_points[position]) + 1;

                    uint8_t RGB1[3] = {
                        
                        m_palette[palette_position1],
                        m_palette[palette_position1 + 1],
                        m_palette[palette_position1 + 2]
                        
                    };

                    uint8_t RGB2[3] = {
                        
                        m_palette[palette_position2],
                        m_palette[palette_position2 + 1],
                        m_palette[palette_position2 + 2]
                    
                    };

                    //pixels[position * 3 + k] = m_points[position] * 255 / m_max_iter_encountered;     
                
                    float junk;

                    uint8_t RGB[3] = {

                        lerp(RGB1[0], RGB2[0], modf(m_points[position], &junk)),
                        lerp(RGB1[1], RGB2[1], modf(m_points[position], &junk)),
                        lerp(RGB1[2], RGB2[2], modf(m_points[position], &junk)),

                    };

                    pixels[position * 3] = RGB[0];
                    pixels[position * 3 + 1] = RGB[1];
                    pixels[position * 3 + 2] = RGB[2];

                //}
                
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