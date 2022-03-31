/**
 * @ Author: Giorgio Chiurato
 * @ Create Time: 2022-03-17 15:02:29
 * @ Modified by: Giorgio Chiurato
 * @ Modified time: 2022-03-27 00:00:15
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

            generate_optim_array();

        };

        std::unique_ptr<uint8_t[]> compute_pixels();
        void update_vertices(double x_pos, double y_pos);

    private:

        uint32_t m_width;
        uint32_t m_height;
        double m_x_pos;
        double m_y_pos;
        bool m_first_frame = true;
        uint32_t m_iter;
        color_mode m_color_mode;
        optimization_type m_optim_type;
        std::vector<bool> m_optim_vector = {0, 0, 0};
        uint8_t m_thread_number;

        std::vector<frame_data<T>> m_frame_array;
        std::unique_ptr<uint32_t[]> m_points;
        std::vector<complex<T>> m_vertices;
        complex<T> m_center = complex<T>(0.0, 0.0);
        std::vector<complex<double>> m_ref_iters;

        void bruteforce_compute(frame_data<T> &frame);
        void border_trace(frame_data<T> &frame);
        void generate_optim_array();
        std::vector<uint8_t> generate_color(uint32_t iter);
        std::vector<frame_data<T>> generate_frame_array();
        std::unique_ptr<uint32_t[]> join_pixel_arrays(std::vector<frame_data<T>> &frame_array);
        uint32_t thread_subdivide(uint32_t value);
        void find_center();

};

template<typename T> void mandelbrot_set<T>::find_center() {

    if(m_first_frame) {
        
        m_first_frame = !m_first_frame;
        return;

    }
    

    std::vector<uint32_t> max_iter_point;

    if (m_points[m_x_pos + m_y_pos * m_width] == m_iter) {

        m_center.set_re(static_cast<T>(m_vertices[1].get_re() + m_vertices[0].get_re()) / 2.0);
        m_center.set_im(static_cast<T>(m_vertices[1].get_im() + m_vertices[0].get_im()) / 2.0);

        return;

    }

    max_iter_point.push_back(m_x_pos);
    max_iter_point.push_back(m_y_pos);
    max_iter_point.push_back(m_points[m_x_pos + m_y_pos * m_width]);

    for (size_t i = 0; i < m_height; ++i) {

        for (size_t j = 0; j < m_width; ++j) {
         
            uint32_t position = j + i * m_width;

            if (m_points[position] == m_iter) {

                m_center.set_re(map<T>(static_cast<T>(m_width), 0.0, m_vertices[1].get_re(), m_vertices[0].get_re(), static_cast<T>(j)));
                m_center.set_im(map<T>(static_cast<T>(m_height), 0.0, m_vertices[1].get_im(), m_vertices[0].get_im(), static_cast<T>(i)));

                return;

            } else if(m_points[position] > max_iter_point[2]) {

                max_iter_point[0] = j;
                max_iter_point[1] = i;
                max_iter_point[2] = m_points[position];

            }

        }
    
    }

    m_center.set_re(map<T>(static_cast<T>(m_width), 0.0, m_vertices[1].get_re(), m_vertices[0].get_re(), static_cast<T>(max_iter_point[0])));
    m_center.set_im(map<T>(static_cast<T>(m_height), 0.0, m_vertices[1].get_im(), m_vertices[0].get_im(), static_cast<T>(max_iter_point[1])));
    
}

template<typename T> void mandelbrot_set<T>::generate_optim_array() {

    if(m_optim_type == NONE) {
        
        return;

    }

    m_optim_vector[0] = (m_optim_type & 0b0001);
    m_optim_vector[1] = (m_optim_type & 0b0010) >> 1;
    m_optim_vector[2] = (m_optim_type & 0b0100) >> 2;

}

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

    for (size_t i = 0; i < frame.m_size[1]; ++i) {
        
        for (size_t j = 0; j < frame.m_size[0]; ++j) {

            uint32_t position = j + i * frame.m_size[0];

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

            if(m_optim_vector[2]) {

                frame.m_pixel_data[position] = check_point(point, m_center, m_ref_iters);

            } else if (!m_optim_vector[2]){

                frame.m_pixel_data[position] = check_point<T>(point, m_iter);

            }
            
        
        }

    }
        
}

template<typename T> std::unique_ptr<uint8_t[]> mandelbrot_set<T>::compute_pixels() {

    if(m_optim_vector[2]) {

        std::vector<complex<double>> ref_iters;
        m_ref_iters = generate_iter_vector(m_center, m_iter);

    }

    m_frame_array = generate_frame_array();
    std::vector<std::thread> threads;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    for(size_t i = 0; i < m_frame_array.size(); ++i){

        if(m_optim_vector[1]) {

            threads.push_back(std::thread(&mandelbrot_set<T>::border_trace, this, std::ref(m_frame_array[i])));

        } else {

            threads.push_back(std::thread(&mandelbrot_set<T>::bruteforce_compute, this, std::ref(m_frame_array[i])));  

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

    m_x_pos = x_pos;
    m_y_pos = y_pos;

    if(m_optim_vector[2]) {
    
        find_center();

    }
    

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


template<typename T> void mandelbrot_set<T>::border_trace(frame_data<T> &frame) {

    /* The frame is completely descriped by two opposite vertices (represented by their relative complex points). 
    ** Here the bottom-left and upper right are chosen, encoded in a vector as follows:
    ** {[0]: bottom-left, [1]: upper_right}
    */

    uint32_t width = frame.m_size[0];
    uint32_t height = frame.m_size[1];
    std::vector<complex<T>> vertices = frame.m_vertices;

    std::queue<size_t> pixel_queue;
    std::unique_ptr<uint32_t[]> computed_iters(new uint32_t[width * height]);

    for (size_t i = 0; i < height; ++i) {
            
        for (size_t j = 0; j < width; ++j) {
    
            uint32_t position = j + i * width;  
            computed_iters[position] = m_iter + 1;

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

    std::function<uint32_t(complex<T>)> check_point_optim;

    if(m_optim_vector[2]) {

        check_point_optim = [=](complex<T> point){

            return check_point(point, m_center, m_ref_iters);

        };

    } else {

        check_point_optim = [=](complex<T> point){
            
            return check_point(point, m_iter);

        };

    }

    while (!pixel_queue.empty()) {
        
        //TODO change input params with frame_data, cleaner solution

        check_neighbours<T>(
            
            pixel_queue.front(), 
            width, 
            height, 
            m_iter, 
            vertices, 
            std::ref(pixel_queue), 
            std::ref(computed_iters), 
            check_point_optim
            
        );
        pixel_queue.pop();

    }

    color_all(width, height, m_iter, std::ref(computed_iters));
    frame.m_pixel_data = std::move(computed_iters);

}
