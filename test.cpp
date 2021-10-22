#include <iostream>
#include "complex.h"

int main(int argc, char ** argv){

    mpf_class re("1.00000000000000");
    mpf_class im("1.00000000000000");

    complex<mpf_class> test_complex(re, im);

    std::cout << test_complex.get_mod() << std::endl;
    std::cout << test_complex.get_mod_sqrd() << std::endl;

    std::cout << test_complex.get_re() << std::endl;
    std::cout << test_complex.get_im() << std::endl;

    std::cout << std::endl;

    complex<double> cmp1(1.0, 1.0);
    complex<double> cmp2(2.0, 3.0);

    complex<double> sum = cmp1 + cmp2;
    complex<double> product = cmp1 * cmp2;

    std::cout << sum.get_re() << std::endl;
    std::cout << sum.get_im() << std::endl;    
    std::cout << sum.get_mod_sqrd() << std::endl;

    std::cout << product.get_re() << std::endl;
    std::cout << product.get_im() << std::endl;
    std::cout << product.get_mod_sqrd() << std::endl;

    return 0;

}
