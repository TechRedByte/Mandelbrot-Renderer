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
    DynPrec(const char* value);
    DynPrec(const double value);
    DynPrec(const long double value);
    DynPrec(float value);
    DynPrec(int value);
    DynPrec(mpfr_prec_t precision);
    DynPrec(const DynPrec& other);

    void set_precision(mpfr_prec_t precision);

    // ASSIGNMENT
    DynPrec& operator=(const DynPrec& other);

    // ADDITION
    DynPrec operator+(const DynPrec& other) const;
    DynPrec& operator+=(const DynPrec& other);

    // SUBTRACTION
    DynPrec operator-(const DynPrec& other) const;
    friend DynPrec operator-(const double& lhs, const DynPrec& rhs);
    DynPrec& operator-=(const DynPrec& other);

    // MULTIPLICATION
    DynPrec operator*(const DynPrec& other) const;
    DynPrec operator*(const double& other) const;
    friend DynPrec operator*(const double& lhs, const DynPrec& rhs);
    DynPrec& operator*=(const DynPrec& other);

    // DIVISION
    DynPrec operator/(const DynPrec& other) const;
    DynPrec& operator/=(const DynPrec& other);

    // OTHER
    friend DynPrec arbitrary::log2(const DynPrec& value);
    friend DynPrec arbitrary::abs(const DynPrec& value);
    friend DynPrec arbitrary::ceil(const DynPrec& value);
    friend DynPrec arbitrary::max(const DynPrec& a, const DynPrec& b);

    // LOGIC
    bool operator==(const DynPrec& other) const;
    bool operator!=(const DynPrec& other) const;
    bool operator>(const DynPrec& other) const;
    bool operator>=(const DynPrec& other) const;
    bool operator<(const DynPrec& other) const;
    bool operator<=(const DynPrec& other) const;

    // NEGATION
    DynPrec operator-() const;

    double get_double() const;
    long double get_long_double() const;
    std::string get_string() const;
    long get_long() const;

    ~DynPrec();
};