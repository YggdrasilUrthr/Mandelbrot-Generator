#pragma once

#include <iostream>

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

            m_pixels = new uint8_t[m_width * m_height * 3];

        };

        void draw(GLFWwindow *);

        ~mandelbrot_set() {

            delete[] m_pixels;

        }

        uint8_t * m_pixels = nullptr;

    private:

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_iter;
        color_mode m_color_mode;

        void bruteforce_compute();

};

template<typename T> void mandelbrot_set<T>::bruteforce_compute(){

    for (size_t i = 0; i < m_height; ++i) {
        
        for (size_t j = 0; j < m_width; ++j) {

            complex<T> point;
            point.set_re(map<T>(static_cast<T>(m_width), 0.0, 2.0, -2.0, static_cast<T>(j)));
            point.set_im(map<T>(static_cast<T>(m_height), 0.0, 2.0, -2.0, static_cast<T>(i))); 

            bool is_inside = check_point<double>(point, m_iter);

            for (size_t k = 0; k < 3; ++k) {
            
                uint32_t position = (j + i * m_width) * 3;
                m_pixels[position + k] = is_inside * 255;

            }

        }

    }
        
}

template<typename T> void mandelbrot_set<T>::draw(GLFWwindow * window) {

    bruteforce_compute();

    do {

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, m_pixels);

        glfwSwapBuffers(window);
        glfwPollEvents();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return;

}