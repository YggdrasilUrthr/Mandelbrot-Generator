#include "optim_algs.h"

template<typename T> bool check_point(const complex<T> &point, unsigned int max_iter) {

    complex<T> z(0.0, 0.0);

    for (int i = 0; i < max_iter; ++i) {
    
        if(z.get_mod_sqrd() > 4) {

            return false;

        }

        z = z * z + point; // TODO fix this
    
    }

    return true;

}