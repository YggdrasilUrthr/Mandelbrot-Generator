#pragma once

/* This is a very minimalistic implementation (it does not support any argument variable and it can only be used for floating point
** real and imaginary part) of complex numbers using some basic GMP functions. 
** A better alternative may include a specific library for arbitrary precision complex numbers (something like GMC). 
** Anyway, considering what we need for such a simple mathematical application, using such complete library might result somehow overkill. 
*/

typedef complex<mpf_class> complex_arb;
typedef complex<float> complex_fix;

template<typename T> class complex {

public:

    complex() {};
    complex(T in_re, T in_im) : re(in_re), im(in_im) {

        update_mods();

    };

    T get_re() { return re; };
    T get_im() { return im; };
    T get_mod() { return mod; };
    T get_mod_sqrd() { return mod_sqrd; };

    void set_re(const T &in_re) {

        re = in_re;

    };

    void set_im(const T &in_im) {

        im = in_im;

    };

    T mod = 0;
    T mod_sqrd = 0;

private:

    void update_mods() {

        mod_sqrd = re * re + im * im;
	    mod = sqrt(mod_sqrd);

    };

    T re = 0;
    T im = 0;

};

