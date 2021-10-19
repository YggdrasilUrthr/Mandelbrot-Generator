#pragma once

#include <memory>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include "optim_algs.h"

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_ITER 100

class mandelbrot_set {

    public:

        enum optimization_type{

            MULTITHREAD = 0b0001,
            BORDER_TRACE = 0b0010,
            PERTURBATION = 0b0100,
            FULL = 0b1111

        };

        enum color_mode{

            COLORS = GL_RGB,
            BW = GL_LUMINANCE

        };

        mandelbrot_set(
        
            uint32_t width = DEFAULT_WIDTH, 
            uint32_t height = DEFAULT_HEIGHT, 
            uint32_t iter = DEFAULT_ITER, 
            color_mode color = BW
            
        ) : m_width(width), m_height(height), m_iter(iter), m_color_mode(color) {};
        
        void draw(GLFWwindow *);

    private:

        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_iter;
        color_mode m_color_mode;

        template<typename T> void bruteforce_compute();
        template<typename T> T map(T, T, T, T, T);
        

};
