/**
 * @ Author: Giorgio Chiurato
 * @ Create Time: 2022-03-17 15:02:29
 * @ Modified by: Giorgio Chiurato
 * @ Modified time: 2022-03-31 19:21:20
 * @ Description:
 */

#include <iostream>
#include <string>
#include <boost/program_options.hpp>

#include "mandelbrot.h"

static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
    
    if(action == GLFW_PRESS) {

        double * data_ptr = static_cast<double *>(glfwGetWindowUserPointer(window));
        glfwGetCursorPos(window, data_ptr, data_ptr + 1);

    }

}

int main(int argc, char ** argv){

    uint32_t optim_code = 0;
    bool arb_prec = false;
    bool colors = true;

    boost::program_options::options_description description("Program usage");

    description.add_options()
        ("help,h", "Displays this message")
        ("width,W", boost::program_options::value<uint32_t>()->default_value(DEFAULT_WIDTH), "Sets image width. Default is ??px")
        ("height,H", boost::program_options::value<uint32_t>()->default_value(DEFAULT_HEIGHT), "Sets image height. Default is equal to width")
        ("iter,I", boost::program_options::value<uint32_t>()->default_value(DEFAULT_ITER), "Sets maximum iteration number. Default is ??")
        ("arb,A", "Switch to arbitrary precison numbers")
        ("bw,b", "Switch to black and white coloring")
        ("threads,T", boost::program_options::value<uint32_t>()->default_value(1), "Enables multithreading and sets thread number")
        ("bt,B", "Enables border-tracing")
        ("pert,P", "Enables perturbation approximation")
        //("full,F", boost::program_options::value<uint32_t>()->default_value(1), "Enables all optimizations an sets thread number")
    ;

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(description).run(), var_map);
    boost::program_options::notify(var_map);

    if(var_map.count("help")){
    
        std::cout << description;
        return 0;
    
    }
    /*if(var_map.count("full")){

        //optim_code = 15;

    }*/
    if(var_map.count("bt")) {

        optim_code |= mandelbrot_set<double>::optimization_type::BORDER_TRACE;

    }
    if(var_map.count("pert")){

        optim_code |= mandelbrot_set<double>::optimization_type::PERTURBATION;

    }
    if(var_map.count("threads")){

        optim_code |= mandelbrot_set<double>::optimization_type::MULTITHREAD;

    }
    if(var_map.count("bw")){

        colors = mandelbrot_set<double>::color_mode::BOOLEAN;

    }
    if(var_map.count("arb")){

        arb_prec = true;

    }

    uint32_t width = var_map["width"].as<uint32_t>();
    uint32_t height = var_map["height"].as<uint32_t>();

    glewExperimental = true;

    if( !glfwInit() ) {
        
        std::cerr << "Failed to initialize GLFW" << std::endl;
        
        return -1;
    
    }


    GLFWwindow * window;
    window = glfwCreateWindow(width, height, "Mandelbrot Set", NULL, NULL);

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
        
        width, height, var_map["iter"].as<uint32_t>(), 
        mandelbrot_set<double>::color_mode(colors), 
        mandelbrot_set<double>::optimization_type(optim_code),
        var_map["threads"].as<uint32_t>()
        
    );

    mandelbrot_set<boost::multiprecision::mpf_float_100> arb_mandelbrot = mandelbrot_set<boost::multiprecision::mpf_float_100>(
        
        width, height, var_map["iter"].as<uint32_t>(),
        mandelbrot_set<boost::multiprecision::mpf_float_100>::color_mode(colors), 
        mandelbrot_set<boost::multiprecision::mpf_float_100>::optimization_type(optim_code),
        var_map["threads"].as<uint32_t>()
        
    );

    std::unique_ptr<uint8_t[]> pixels;
    
    if(arb_prec) {
    
        pixels = arb_mandelbrot.compute_pixels();

    } else {

        pixels = fixed_mandelbrot.compute_pixels();

    }

    // Set OpenGL viewport and initialize scaling matrices with an identity matrix.

    glViewport(0.0f, 0.0f, width, height);
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
    glOrtho(0, width, height, 0, 0, 1); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity(); 

    double x_pos;
    double y_pos;

    uint32_t offset_x = static_cast<uint32_t>(width / 10);
    uint32_t offset_y = static_cast<uint32_t>(height / 10);

    do {

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.get());
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

            if(arb_prec) {
    
                arb_mandelbrot.update_vertices(click_point[0], height - click_point[1]);
                pixels = arb_mandelbrot.compute_pixels();

            } else {

                fixed_mandelbrot.update_vertices(click_point[0], height - click_point[1]);
                pixels = fixed_mandelbrot.compute_pixels();

            }

        }

        glfwSwapBuffers(window);
        glfwWaitEvents();

    } while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    return 0;

}