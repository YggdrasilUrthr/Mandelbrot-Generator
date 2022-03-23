/**
 * @ Author: Giorgio Chiurato
 * @ Create Time: 2022-03-17 15:02:29
 * @ Modified by: Giorgio Chiurato
 * @ Modified time: 2022-03-18 21:15:28
 * @ Description:
 */

#pragma once

#include <iostream>
#include <memory>
#include <chrono>
#include <thread>

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
            optimization_type optim_type = NONE,
            uint32_t thread_number = 1
            
        ) : m_width(width), m_height(height), m_iter(iter), m_color_mode(color), m_optim_type(optim_type), m_thread_number(thread_number) {

            m_points = std::unique_ptr<uint32_t[]>(new uint32_t[m_width * m_height]);
            
            m_vertices.push_back(complex<T>(-2.0, -2.0));
            m_vertices.push_back(complex<T>(2.0, 2.0));

        };

        std::unique_ptr<uint8_t[]> compute_pixels();
        void update_vertices(double x_pos, double y_pos);

    private:

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_iter;
        color_mode m_color_mode;
        optimization_type m_optim_type;
        uint8_t m_thread_number;

        std::vector<frame_data<T>> m_frame_array;
        std::unique_ptr<uint32_t[]> m_points;
        std::vector<complex<T>> m_vertices;

        void bruteforce_compute(frame_data<T> &frame);
        std::vector<uint8_t> generate_color(uint32_t iter);
        std::vector<frame_data<T>> generate_frame_array();
        std::unique_ptr<uint32_t[]> join_pixel_arrays(std::vector<frame_data<T>> &frame_array);
        uint32_t thread_subdivide(uint32_t value);

};

template<typename T> std::unique_ptr<uint32_t[]> mandelbrot_set<T>::join_pixel_arrays(std::vector<frame_data<T>> &frame_array) {

    std::unique_ptr<uint32_t[]> pixel_array = std::unique_ptr<uint32_t[]>(new uint32_t[m_width * m_height]);

    for (size_t k = 0; k < frame_array.size(); ++k) {

        for (size_t i = 0; i < frame_array[k].m_size[1]; ++i) {

            for (size_t j = 0; j < frame_array[k].m_size[0]; ++j) {

                uint32_t x_idx = frame_array[k].m_size[0] * frame_array[k].m_idx[1] + j;
                uint32_t y_idx = frame_array[k].m_size[1] * frame_array[k].m_idx[0] + i;
                uint32_t complete_idx = x_idx + y_idx * m_width;

                pixel_array[complete_idx] = frame_array[k].m_pixel_data[j + i * frame_array[k].m_size[0]];
            
            }
            
        }

    }

    return pixel_array;

}

template<typename T> std::vector<uint8_t> mandelbrot_set<T>::generate_color(uint32_t iter) {

    float t = static_cast<float>(iter) / static_cast<float>(m_iter);

    float R_ratio = 9.0 * (1 - t) * pow(t, 3);
    float G_ratio = 15.0 * pow((1 - t), 2) * pow(t, 2);
    float B_ratio = 8.5 * pow((1 - t), 3) * t;

    std::vector<uint8_t> RGB = {

        static_cast<uint8_t>(R_ratio * 255.0),
        static_cast<uint8_t>(G_ratio * 255.0),
        static_cast<uint8_t>(B_ratio * 255.0)

    };

    return RGB;

}

template<typename T> uint32_t mandelbrot_set<T>::thread_subdivide(uint32_t value) {

    if(value == 1 || value == 0) {
        
        return 1;

    } else if(value == 2) {

        return 2;

    } else {

        return value / 2;

    }
        
}

template<typename T> std::vector<frame_data<T>> mandelbrot_set<T>::generate_frame_array() {

    uint32_t vert_frames = thread_subdivide(m_thread_number);
    uint32_t hor_frames = thread_subdivide(m_thread_number / vert_frames);
    

    std::vector<frame_data<T>> frame_array;

    for(uint32_t i = 0; i < vert_frames; ++i) {

        for (uint32_t j = 0; j < hor_frames; ++j) {
        
            std::vector<uint32_t> current_size = {

                m_width / hor_frames,
                m_height / vert_frames

            };

            complex<T> bottom_left(

                map<T>(static_cast<T>(m_width), 0.0, m_vertices[1].get_re(), m_vertices[0].get_re(), static_cast<T>(j * current_size[0])),
                map<T>(static_cast<T>(m_height), 0.0, m_vertices[1].get_im(), m_vertices[0].get_im(), static_cast<T>(i * current_size[1])) 


            );

            complex<T> upper_right(

                map<T>(static_cast<T>(m_width), 0.0, m_vertices[1].get_re(), m_vertices[0].get_re(), static_cast<T>((j + 1) * current_size[0])),
                map<T>(static_cast<T>(m_height), 0.0, m_vertices[1].get_im(), m_vertices[0].get_im(), static_cast<T>((i + 1) * current_size[1])) 

            );

            std::vector<complex<T>> current_vertices = {

                bottom_left,
                upper_right

            };

            std::vector<uint32_t> idx = {i, j};

            frame_array.push_back(frame_data<T>(current_size, current_vertices, idx));

        }

    }
    
    return frame_array;

}

template<typename T> void mandelbrot_set<T>::bruteforce_compute(frame_data<T> &frame) {

    std::vector<complex<double>> ref_iters;
    complex<T> center_arb(static_cast<T>(0.0), static_cast<T>(0.0));

    if(m_optim_type == PERTURBATION) {

        //center_arb.set_re(static_cast<T>(m_vertices[1].get_re() + m_vertices[0].get_re()) / 2.0);
        //center_arb.set_im(static_cast<T>(m_vertices[1].get_im() + m_vertices[0].get_im()) / 2.0);

        ref_iters = generate_iter_vector(center_arb, m_iter);

    }

    complex<double> center;
    center.set_re(static_cast<double>(center_arb.get_re()));
    center.set_im(static_cast<double>(center_arb.get_im()));

    for (size_t i = 0; i < frame.m_size[1]; ++i) {
        
        for (size_t j = 0; j < frame.m_size[0]; ++j) {

            uint32_t position = j + i * frame.m_size[0];
            
            if(m_optim_type == PERTURBATION) {
                
                complex<double> point;

                point.set_re(map<double>(static_cast<double>(m_width), 0.0, static_cast<double>(m_vertices[1].get_re()), static_cast<double>(m_vertices[0].get_re()), static_cast<double>(j)));
                point.set_im(map<double>(static_cast<double>(m_height), 0.0, static_cast<double>(m_vertices[1].get_im()), static_cast<double>(m_vertices[0].get_im()), static_cast<double>(i))); 

                frame.m_pixel_data[position] = check_point(point, center, ref_iters);

            } else if (m_optim_type == NONE){

                complex<T> point;

                point.set_re(map<T>(
                    
                    static_cast<T>(frame.m_size[0]), 
                    0.0, 
                    frame.m_vertices[1].get_re(), 
                    frame.m_vertices[0].get_re(), 
                    static_cast<T>(j)
                    
                ));
                point.set_im(map<T>(
                    
                    static_cast<T>(frame.m_size[1]),
                    0.0, 
                    frame.m_vertices[1].get_im(), 
                    frame.m_vertices[0].get_im(), 
                    static_cast<T>(i)
                    
                )); 

                frame.m_pixel_data[position] = check_point<T>(point, m_iter);

            }
            
        
        }

    }
        
}

template<typename T> std::unique_ptr<uint8_t[]> mandelbrot_set<T>::compute_pixels() {

    m_frame_array = generate_frame_array();
    std::vector<std::thread> threads;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(size_t i = 0; i < m_frame_array.size(); ++i){

        if(m_optim_type == NONE || m_optim_type == PERTURBATION) {

            threads.push_back(std::thread(&mandelbrot_set<T>::bruteforce_compute, this, std::ref(m_frame_array[i])));  

        } else if(m_optim_type == BORDER_TRACE) {

            threads.push_back(std::thread(border_trace<T>, std::ref(m_frame_array[i]), m_iter));

        }

    }

    for(auto &thread : threads) {

        thread.join();

    }

    m_points = std::move(join_pixel_arrays(m_frame_array));
    
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[m_width * m_height * 3]);

    if (m_color_mode) {

        for (size_t i = 0; i < m_height; ++i) {

            for (size_t j = 0; j < m_width; ++j) {

                uint32_t position = j + i * m_width;
                std::vector<uint8_t> RGB = generate_color(m_points[position]);

                if (m_points[position] != m_iter) {

                    for (uint8_t i = 0; i < 3; ++i) {
                        
                        pixels[position * 3 + i] = RGB[i];

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

template<typename T> void mandelbrot_set<T>::update_vertices(double x_pos, double y_pos) {

    //Maybe save this?? (used in border-trace)

    T width = m_vertices[1].get_re() - m_vertices[0].get_re();
    T height = m_vertices[1].get_im() - m_vertices[0].get_im();

    complex<T> center(

        map<T>(static_cast<T>(m_width), 0.0, m_vertices[1].get_re(), m_vertices[0].get_re(), x_pos),
        map<T>(static_cast<T>(m_height), 0.0, m_vertices[1].get_im(), m_vertices[0].get_im(), y_pos)

    );

    // TODO adjust zoom factor esternally, now hard-coded to 10

    complex<T> diag_offset(width / 10, height / 10);

    m_vertices[0] = center - diag_offset;
    m_vertices[1] = center + diag_offset;

}