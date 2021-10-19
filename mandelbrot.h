#pragma once

#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600
#define DEFAULT_ITER 100

#include <memory>
#include "optim_algs.h"

class mandelbrot_set {

    public:

        enum optimization{

            MULTITHREAD = 0b0001,
            BORDER_TRACE = 0b0010,
            PERTURBATION = 0b0100,
            FULL = 0b1111

        };

        mandelbrot_set(unsigned int, unsigned int, unsigned int, bool);
        void draw();

    private:

        unsigned int m_width;
        unsigned int m_height;
        bool m_colors;

        template<typename T> void bruteforce_compute();
        template<typename T> T map(T, T, T, T, T);
        

};
