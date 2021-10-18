#include "mandelbrot.h"

mandelbrot_set::mandelbrot_set(
        
    unsigned int width = DEFAULT_WIDTH, 
    unsigned int height = DEFAULT_HEIGHT, 
    unsigned int iter = DEFAULT_ITER,
    bool colors = false

){

    m_width = width;
    m_height = height;
    m_colors = colors;

}

void mandelbrot_set::bruteforce_compute(){

    

}