#pragma once

#include <mpfr.h>
#include <string>

#include "global.h"

class DynPrec {
private:
    mpfr_t _variable;
public:
    DynPrec(const char* value);
    DynPrec(const double value);
    DynPrec(const long double value);
    DynPrec(mpfr_prec_t precision);
    DynPrec(const DynPrec& other);

    void set_precision(unsigned int precision);

    DynPrec& operator=(const DynPrec& other);

    DynPrec operator+(const DynPrec& other) const;
    DynPrec operator-(const DynPrec& other) const;
    DynPrec operator*(const DynPrec& other) const;
    DynPrec operator/(const DynPrec& other) const;

    DynPrec& operator-();

    bool operator==(const DynPrec& other) const;
    bool operator!=(const DynPrec& other) const;
    bool operator>(const DynPrec& other) const;
    bool operator>=(const DynPrec& other) const;
    bool operator<(const DynPrec& other) const;
    bool operator<=(const DynPrec& other) const;

    double get_double() const;
    long double get_long_double() const;
    std::string get_string() const;

    ~DynPrec();
};
    
void update_precision(Dimensions&);