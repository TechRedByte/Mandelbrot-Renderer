#pragma once

#include <mpfr.h>
#include <string>

struct Dimensions;
class MPFR;

namespace arbitrary {
    MPFR log2(const MPFR& value);
    MPFR abs(const MPFR& value);
    MPFR ceil(const MPFR& value);
    MPFR max(const MPFR& a, const MPFR& b);
}

class MPFR {
private:
    mpfr_t _variable;
public:
    MPFR(const char* value);
    MPFR(const double value);
    MPFR(const long double value);
    MPFR(float value);
    MPFR(int value);
    MPFR(mpfr_prec_t precision);
    MPFR(const MPFR& other);

    void set_precision(mpfr_prec_t precision);

    // ASSIGNMENT
    MPFR& operator=(const MPFR& other);

    // ADDITION
    MPFR operator+(const MPFR& other) const;
    MPFR& operator+=(const MPFR& other);

    // SUBTRACTION
    MPFR operator-(const MPFR& other) const;
    friend MPFR operator-(const double& lhs, const MPFR& rhs);
    MPFR& operator-=(const MPFR& other);

    // MULTIPLICATION
    MPFR operator*(const MPFR& other) const;
    MPFR operator*(const double& other) const;
    friend MPFR operator*(const double& lhs, const MPFR& rhs);
    MPFR& operator*=(const MPFR& other);

    // DIVISION
    MPFR operator/(const MPFR& other) const;
    MPFR& operator/=(const MPFR& other);

    // OTHER
    friend MPFR arbitrary::log2(const MPFR& value);
    friend MPFR arbitrary::abs(const MPFR& value);
    friend MPFR arbitrary::ceil(const MPFR& value);
    friend MPFR arbitrary::max(const MPFR& a, const MPFR& b);

    // LOGIC
    bool operator==(const MPFR& other) const;
    bool operator!=(const MPFR& other) const;
    bool operator>(const MPFR& other) const;
    bool operator>=(const MPFR& other) const;
    bool operator<(const MPFR& other) const;
    bool operator<=(const MPFR& other) const;

    // NEGATION
    MPFR operator-() const;

    double get_double() const;
    long double get_long_double() const;
    std::string get_string() const;
    long get_long() const;

    ~MPFR();
};