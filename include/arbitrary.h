#pragma once

#include <mpfr.h>
#include <string>

struct Dimensions;
class DynPrec;

namespace arbitrary {
    DynPrec log2(const DynPrec& value);
    DynPrec abs(const DynPrec& value);
    DynPrec ceil(const DynPrec& value);
    DynPrec max(const DynPrec& a, const DynPrec& b);
}

class DynPrec {
private:
    mpfr_t _variable;
public:
    DynPrec();
    DynPrec(const char* value);
    DynPrec(const double value);
    DynPrec(const long double value);
    DynPrec(float value);
    DynPrec(int value);
    DynPrec(mpfr_prec_t precision);
    DynPrec(const DynPrec& other);
    
    void set_precision(unsigned int precision);
    
    DynPrec& operator=(const DynPrec& other);
    
    DynPrec operator+(const DynPrec& other) const;
    DynPrec& operator+=(const DynPrec& other);
    DynPrec operator-(const DynPrec& other) const;
    DynPrec& operator-=(const DynPrec& other);
    DynPrec operator*(const DynPrec& other) const;
    DynPrec operator*(const double& other) const;
    DynPrec& operator*=(const DynPrec& other);
    DynPrec operator/(const DynPrec& other) const;
    DynPrec& operator/=(const DynPrec& other);
    
    bool operator==(const DynPrec& other) const;
    bool operator!=(const DynPrec& other) const;
    bool operator>(const DynPrec& other) const;
    bool operator>=(const DynPrec& other) const;
    bool operator<(const DynPrec& other) const;
    bool operator<=(const DynPrec& other) const;
    
    DynPrec operator-() const;
    
    double get_double() const;
    long double get_long_double() const;
    std::string get_string() const;
    long get_long() const;
    
    friend DynPrec arbitrary::log2(const DynPrec& value);
    friend DynPrec arbitrary::abs(const DynPrec& value);
    friend DynPrec arbitrary::ceil(const DynPrec& value);
    friend DynPrec arbitrary::max(const DynPrec& a, const DynPrec& b);
    friend DynPrec operator*(const double& lhs, const DynPrec& rhs);
    friend DynPrec operator-(const double& lhs, const DynPrec& rhs);
    
    ~DynPrec();
};