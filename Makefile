CXXFLAGS = -std=c++11
GUI = -lGL -lGLEW -lglfw
ARBPREC = -lgmp -lgmpxx -lmpfr
THREAD = -lpthread
BOOST = -lboost_program_options
DEBUG = -g -O0
RELEASE = -Ofast

mandelbrot_generator: mandelbrot_generator.cpp mandelbrot.h optim_algs.h 
	$(CXX) $(CXXFLAGS) $(RELEASE) $(LINK) mandelbrot_generator.cpp -o mandelbrot_generator.out $(GUI) $(ARBPREC) $(THREAD) $(BOOST)

clean: ; rm -rf *.o
