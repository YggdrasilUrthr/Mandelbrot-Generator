#pragma once

#include <iostream>
#include <memory>

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
            color_mode color = BOOLEAN
            
        ) : m_width(width), m_height(height), m_iter(iter), m_color_mode(color) {

            m_points = std::unique_ptr<uint32_t[]>(new uint32_t[m_width * m_height]);

        };

        void draw(GLFWwindow *);
        void draw_list(GLFWwindow *);

    private:

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_iter;
        uint32_t m_max_iter_encountered = 0;
        color_mode m_color_mode;

        std::unique_ptr<uint32_t[]> m_points;

        void bruteforce_compute();
        //void border_trace();

};

template<typename T> void mandelbrot_set<T>::bruteforce_compute(){

    for (size_t i = 0; i < m_height; ++i) {
        
        for (size_t j = 0; j < m_width; ++j) {

            complex<T> point;
            point.set_re(map<T>(static_cast<T>(m_width), 0.0, 2.0, -2.0, static_cast<T>(j)));
            point.set_im(map<T>(static_cast<T>(m_height), 0.0, 2.0, -2.0, static_cast<T>(i))); 

            uint32_t position = j + i * m_width;
            m_points[position] = check_point<T>(point, m_iter);

            if (m_points[position] > m_max_iter_encountered) {
                
                m_max_iter_encountered = m_points[position];

            }

        }

    }
        
}

/*template<typename T> void mandelbrot_set<T>::border_trace() {



}*/

//Only a tesing method, has to be removed once I get the algorithm working.

template<typename T> void mandelbrot_set<T>::draw_list(GLFWwindow * window) {

    T re_delta = map<T>(static_cast<T>(m_width), 0.0, 4.0, 0.0, 1.0);
    T im_delta = map<T>(static_cast<T>(m_height), 0.0, 4.0, 0.0, 1.0);

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[m_width * m_height * 3]);

    for (size_t i = 0; i < m_height; ++i) {
            
        for (size_t j = 0; j < m_width; ++j) {
    
            uint32_t position = j + i * m_width;

            for (uint8_t k = 0; k < 3; ++k) {

                pixels[position * 3 + k] = 0;     
                
            }

        }
    }
    
    complex<T> start_point;
    complex<T> backtrack;

    bool set_flag = false;

    for (size_t i = 0; i < m_height; ++i) {
        
        if (!set_flag) {
            
            for (size_t j = 0; j < m_width; ++j) {

                complex<T> point;
                point.set_re(map<T>(static_cast<T>(m_width), 0.0, 2.0, -2.0, static_cast<T>(j)));
                point.set_im(map<T>(static_cast<T>(m_height), 0.0, 2.0, -2.0, static_cast<T>(i))); 

                if(check_point(point, m_iter) >= 12) {

                    start_point.set_re(point.get_re());
                    start_point.set_im(point.get_im());

                    if(j != 0) {
                        
                        backtrack.set_re(map<T>(static_cast<T>(m_width), 0.0, 2.0, -2.0, static_cast<T>(j - 1)));
                        backtrack.set_im(map<T>(static_cast<T>(m_height), 0.0, 2.0, -2.0, static_cast<T>(i))); 

                    } else {

                        backtrack.set_re(map<T>(static_cast<T>(m_width), 0.0, 2.0, -2.0, static_cast<T>(m_width - 1)));
                        backtrack.set_im(map<T>(static_cast<T>(m_height), 0.0, 2.0, -2.0, static_cast<T>(i - 1))); 

                    }
                    

                    set_flag = !set_flag;
                    break;

                }

            }
        }

    }

    std::list<complex<T>> points = border_trace<T>(start_point, backtrack, re_delta, im_delta, m_iter);

    for(typename std::list<complex<T>>::iterator it = points.begin(); it != points.end(); ++it) {

        size_t j = static_cast<size_t>(map<T>(2.0, -2.0, m_width, 0.0, static_cast<T>(it->get_re())));
        size_t i = static_cast<size_t>(map<T>(2.0, -2.0, m_height, 0.0, static_cast<T>(it->get_im())));

        uint32_t position = j + i * m_width;

        for (uint8_t k = 0; k < 3; ++k) {

            pixels[position * 3 + k] = 255;     
            
        }

    }

    do {

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return;

}

template<typename T> void mandelbrot_set<T>::draw(GLFWwindow * window) {

    bruteforce_compute();

    std::unique_ptr<uint8_t[]> pixels(new uint8_t[m_width * m_height * 3]);

    if (m_color_mode) {

        for (size_t i = 0; i < m_height; ++i) {
            
            for (size_t j = 0; j < m_width; ++j) {

                uint32_t position = j + i * m_width;

                for (uint8_t k = 0; k < 3; ++k) {

                    pixels[position * 3 + k] = m_points[position] * 255 / m_max_iter_encountered;     
                
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
    
    do {

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return;

}