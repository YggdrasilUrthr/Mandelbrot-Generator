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
    window = glfwCreateWindow(params[0], params[1], "Mandelbrot Set", NULL, NULL);

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
    //glfwSetCursorPosCallback(window, cursorPositionCallback);

    mandelbrot_set<double> mandelbrot(
        
        params[0], params[1], params[2], 
        mandelbrot_set<double>::color_mode(params[3]), 
        mandelbrot_set<double>::optimization_type(params[4])
        
    );

    std::unique_ptr<uint8_t[]> pixels = mandelbrot.compute_pixels();

    // Set OpenGL viewport and initialize scaling matrices with an identity matrix.

    glViewport(0.0f, 0.0f, params[0], params[1]);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
    glOrtho(0, params[0], params[1], 0, 0, 1); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

    double x_pos;
    double y_pos;

    do {

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(params[0], params[1], GL_RGB, GL_UNSIGNED_BYTE, pixels.get());

        glfwGetCursorPos(window, &x_pos, &y_pos);

        // Draw box around cursor

        glPushMatrix();
        glBegin(GL_LINE_LOOP);
        glColor3f(1, 0, 0); 
        glVertex2f(x_pos - 50, y_pos - 50);    
        glVertex2f(x_pos - 50, y_pos + 50);
        glVertex2f(x_pos + 50, y_pos + 50);
        glVertex2f(x_pos + 50, y_pos - 50);       
        glEnd();
        glPopMatrix();

        // Handle events and update frame buffer 

        glfwPollEvents();
        glfwSwapBuffers(window);

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;

}