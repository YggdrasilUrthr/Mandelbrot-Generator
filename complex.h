/**
 * @ Author: Giorgio Chiurato
 * @ Create Time: 2022-03-16 18:59:29
 * @ Modified by: Giorgio Chiurato
 * @ Modified time: 2022-03-18 20:41:59
 * @ Description:
 */

#pragma once

#include <math.h>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/mpfr.hpp>

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

    const T get_re() { return m_re; };
    const T get_im() { return m_im; };
    
    const T get_mod() { 
        
        //update_mods();
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

    complex<T> operator-(const complex<T> &complex_2) {
        
        return complex<T>(m_re - complex_2.m_re, m_im - complex_2.m_im);

    };

    complex<T> operator*(const complex<T> &complex_2) {

        T out_re = m_re * complex_2.m_re - m_im * complex_2.m_im;
        T out_im = m_re * complex_2.m_im + m_im * complex_2.m_re;

        return complex<T>(out_re, out_im);

    };

    complex<T> operator*(const T scalar) {

        return complex<T>(m_re * scalar, m_im * scalar);

    }

    complex<T> operator/(const T scalar) {

        return complex<T>(m_re / scalar, m_im / scalar);

    }

    bool operator==(const complex<T> &complex_2) {
        
        return (epsilon_compare(m_re, complex_2.m_re)) && (epsilon_compare(m_im, complex_2.m_im));

    };

    bool operator!=(const complex<T> &complex_2) {

        return !(operator==(complex_2));

    }

private:

    void update_mods() {

        m_mod_sqrd = m_re * m_re + m_im * m_im;
	    m_mod = sqrt(m_mod_sqrd);

    };

    bool epsilon_compare(T in_1, T in_2) {

        double epsilon = std::numeric_limits<double>::epsilon();

        if(abs(in_1 - in_2) < 0.0001) {

            return true;

        }

        return false;

    };

    T m_re = 0;
    T m_im = 0;
    T m_mod = 0;
    T m_mod_sqrd = 0;

};

typedef complex<boost::multiprecision::mpfr_float> complex_arb;
typedef complex<double> complex_fix;
