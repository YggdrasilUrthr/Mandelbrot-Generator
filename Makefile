CXXFLAGS = -std=c++11
DEBUG = -pg -g -O0

mandelbrot_generator: mandelbrot_generator.cpp mandelbrot.h optim_algs.h 
	$(CXX) $(CXXFLAGS) $(DEBUG) $(LINK) mandelbrot_generator.cpp -o mandelbrot_generator.out -lGL -lGLEW -lglfw -lgmp -lgmpxx

clean: ; rm -rf *.o
