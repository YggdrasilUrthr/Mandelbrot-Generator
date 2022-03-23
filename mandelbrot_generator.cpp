/**
 * @ Author: Giorgio Chiurato
 * @ Create Time: 2022-03-17 15:02:29
 * @ Modified by: Giorgio Chiurato
 * @ Modified time: 2022-03-18 20:41:52
 * @ Description:
 */

#include <iostream>
#include <string>

#include "mandelbrot.h"

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
    
    if(action == GLFW_PRESS) {

        double * data_ptr = static_cast<double *>(glfwGetWindowUserPointer(window));
        glfwGetCursorPos(window, data_ptr, data_ptr + 1);

    }

}

int main(int argc, char ** argv){

    uint32_t params[7] = {
        
        DEFAULT_WIDTH, 
        DEFAULT_HEIGHT, 
        DEFAULT_ITER, 
        mandelbrot_set<double>::color_mode::BOOLEAN,
        mandelbrot_set<double>::optimization_type::NONE,
        0,
        0

    };

    if (argc > 1 && argc <= 8) {
        
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

    std::vector<double> click_point = {-1, -1}; 
    std::vector<double> old_click_point = click_point;

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowUserPointer(window, click_point.data());

    mandelbrot_set<double> fixed_mandelbrot = mandelbrot_set<double>(
        
        params[0], params[1], params[2], 
        mandelbrot_set<double>::color_mode(params[3]), 
        mandelbrot_set<double>::optimization_type(params[4]),
        params[6]
        
    );

    mandelbrot_set<boost::multiprecision::mpfr_float> arb_mandelbrot = mandelbrot_set<boost::multiprecision::mpfr_float>(
        
        params[0], params[1], params[2], 
        mandelbrot_set<boost::multiprecision::mpfr_float>::color_mode(params[3]), 
        mandelbrot_set<boost::multiprecision::mpfr_float>::optimization_type(params[4]),
        params[6]
        
    );

    std::unique_ptr<uint8_t[]> pixels;
    
    if(params[5]) {
    
        pixels = arb_mandelbrot.compute_pixels();

    } else {

        pixels = fixed_mandelbrot.compute_pixels();

    }

    // Set OpenGL viewport and initialize scaling matrices with an identity matrix.

    glViewport(0.0f, 0.0f, params[0], params[1]);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
    glOrtho(0, params[0], params[1], 0, 0, 1); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

    double x_pos;
    double y_pos;

    uint32_t offset_x = static_cast<uint32_t>(params[0] / 10);
    uint32_t offset_y = static_cast<uint32_t>(params[1] / 10);

    do {

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(params[0], params[1], GL_RGB, GL_UNSIGNED_BYTE, pixels.get());
        glfwGetCursorPos(window, &x_pos, &y_pos);

        // Draw box around cursor

        glPushMatrix();
        glBegin(GL_LINE_LOOP);
        glColor3f(1, 0, 0); 
        glVertex2f(x_pos - offset_x, y_pos - offset_y);    
        glVertex2f(x_pos - offset_x, y_pos + offset_y);
        glVertex2f(x_pos + offset_x, y_pos + offset_y);
        glVertex2f(x_pos + offset_x, y_pos - offset_y);       
        glEnd();
        glPopMatrix();

        // Handle events and update frame buffer 

        if(click_point[0] != old_click_point[0] || click_point[1] != old_click_point[1]) {
                
            old_click_point = click_point;

            if(params[5]) {
    
                arb_mandelbrot.update_vertices(click_point[0], params[1] - click_point[1]);
                pixels = arb_mandelbrot.compute_pixels();

            } else {

                fixed_mandelbrot.update_vertices(click_point[0], params[1] - click_point[1]);
                pixels = fixed_mandelbrot.compute_pixels();

            }

        }

        glfwSwapBuffers(window);
        glfwWaitEvents();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;

}