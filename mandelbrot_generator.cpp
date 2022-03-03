#include <iostream>
#include <string>

#include "mandelbrot.h"

int main(int argc, char ** argv){

    uint32_t params[5] = {
        
        DEFAULT_WIDTH, 
        DEFAULT_HEIGHT, 
        DEFAULT_ITER, 
        mandelbrot_set<double>::color_mode::BOOLEAN,
        mandelbrot_set<double>::optimization_type::NONE

    };

    if (argc > 1 && argc <= 6) {
        
        for(size_t i = 1; i < argc; ++i) {

            params[i - 1] = std::stoi(std::string(argv[i]));

        }

    }
    
    

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

    mandelbrot_set<double> mandelbrot(
        
        params[0], params[1], params[2], 
        mandelbrot_set<double>::color_mode(params[3]), 
        mandelbrot_set<double>::optimization_type(params[4])
        
    );

    mandelbrot.draw(window);

    return 0;

}