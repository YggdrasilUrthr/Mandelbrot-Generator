#include <iostream>
#include <gmpxx.h>
#include "complex.h"

int main(int argc, char ** argv){

    mpf_class re("1.00000000000000");
    mpf_class im("1.00000000000000");

    complex<mpf_class> test_complex(re, im);

    std::cout << test_complex.get_mod() << std::endl;
    std::cout << test_complex.get_mod_sqrd() << std::endl;

    std::cout << test_complex.get_re() << std::endl;
    std::cout << test_complex.get_im() << std::endl;

    return 0;

}
