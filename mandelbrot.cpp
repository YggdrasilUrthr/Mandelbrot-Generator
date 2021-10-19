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

template<typename T> T mandelbrot_set::map(T old_max, T old_min, T new_max, T new_min, T value) {

    

}

template<typename T> void mandelbrot_set::bruteforce_compute(){

    for (size_t i = 0; i < m_width; ++i) {
    
        for (size_t j = 0; j < m_height; ++j) {
            
            

        }
    
    }
        
}
