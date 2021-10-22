#include <iostream>

#include "mandelbrot.h"

int main(int argc, char ** argv){

    glewExperimental = true;

    if( !glfwInit() ) {
        
        std::cerr << "Failed to initialize GLFW" << std::endl;
        
        return -1;
    
    }


    GLFWwindow * window;
    window = glfwCreateWindow( 1024, 768, "Mandelbrot Set", NULL, NULL);

    if( window == NULL ){
        
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        
        return -1;
    
    }

    glfwMakeContextCurrent(window);
    
    if (glewInit() != GLEW_OK) {

        std::cerr << "Failed to initialize GLEW" << std::endl;
        
        return -1;
    
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    mandelbrot_set<double> mandelbrot(120, 120, 1000);
    mandelbrot.draw(window);

    return 0;

}