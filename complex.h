#pragma once

#include <math.h>
#include <gmpxx.h>

/* This is a very minimalistic implementation (it does not support any argument variable and it can only be used for floating point
** real and imaginary part) of complex numbers using some basic GMP functions. 
** A better alternative may include a specific library for arbitrary precision complex numbers (something like GMC). 
** Anyway, considering what we need for such a simple mathematical application, using such complete library might result somehow overkill. 
*/

template<typename T> class complex {

public:

    complex() {};
    complex(T in_re, T in_im) : m_re(in_re), m_im(in_im) {

        update_mods();

    };

    T get_re() { return m_re; };
    T get_im() { return m_im; };
    
    T get_mod() { 
        
        update_mods();
        return m_mod; 
        
    };

    T get_mod_sqrd() { 
        
        update_mods();
        return m_mod_sqrd; 
        
    };

    void set_re(const T &in_re) {

        m_re = in_re;

    };

    void set_im(const T &in_im) {

        m_im = in_im;

    };

    complex<T> operator+(const complex<T> &complex_2) {
        
        return complex<T>(m_re + complex_2.m_re, m_im + complex_2.m_im);

    };

    complex<T> operator*(const complex<T> &complex_2) {

        T out_re = m_re * complex_2.m_re - m_im * complex_2.m_im;
        T out_im = m_re * complex_2.m_im + m_im * complex_2.m_re;

        return complex<T>(out_re, out_im);

    };

private:

    void update_mods() {

        m_mod_sqrd = m_re * m_re + m_im * m_im;
	    m_mod = sqrt(m_mod_sqrd);

    };

    T m_re = 0;
    T m_im = 0;
    T m_mod = 0;
    T m_mod_sqrd = 0;

};

typedef complex<mpf_class> complex_arb;
typedef complex<double> complex_fix;
