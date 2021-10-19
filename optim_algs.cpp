#include "optim_algs.h"

template<typename T> bool check_point(complex<T> point, unsigned int max_iter) {

    complex<T> z(0.0, 0.0);

    for (size_t i = 0; i < max_iter; ++i) {

        if(z.get_mod_sqrd() >= 2) {

            return false;

        }

    }

    return true;

}