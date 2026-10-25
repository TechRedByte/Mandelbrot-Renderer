#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <limits>
#include <mpfr.h>
#include <string>
#include <cmath>

#include "arbitrary.h"
#include "logger.h"
#include "global.h"

MPFR::MPFR(const char* value) {
    std::string str(value);

    size_t digits = 0;
    bool started = false;

    for (char c : str) {
        if (c >= '1' && c <= '9') {
            started = true;
        }

        if (started && c >= '0' && c <= '9') {
            digits++;
        }
    }

    digits = std::max(size_t(1), digits);

    mpfr_prec_t precision = std::ceil(digits * std::log2(10.0));

    mpfr_init2(_variable, precision);
    if (mpfr_set_str(_variable, value, 10, MPFR_RNDN)) {
        mpfr_clear(_variable);
        log_error("Failed to construct MPFR variable from string: %s", value);
        throw std::invalid_argument("Invalid number for MPFR");
    }
}

MPFR::MPFR(double value) {
    mpfr_init2(_variable, std::numeric_limits<double>::digits);
    mpfr_set_d(_variable, value, MPFR_RNDN);
}

MPFR::MPFR(long double value) {
    mpfr_init2(_variable, std::numeric_limits<long double>::digits);
    mpfr_set_ld(_variable, value, MPFR_RNDN);
}

MPFR::MPFR(float value) {
    mpfr_init2(_variable, std::numeric_limits<float>::digits);
    mpfr_set_flt(_variable, value, MPFR_RNDN);
}

MPFR::MPFR(int value) {
    mpfr_init2(_variable, std::numeric_limits<int>::digits);
    mpfr_set_si(_variable, value, MPFR_RNDN);
}

MPFR::MPFR(mpfr_prec_t precision) {
    if (precision >= MPFR_PREC_MIN && precision <= MPFR_PREC_MAX) {
        mpfr_init2(_variable, precision);
    } else {
        log_error("Failed to construct MPFR variable with precision: %ld", static_cast<long>(precision));
        throw std::invalid_argument("Invalid precision for MPFR");
    }
}

MPFR::MPFR(const MPFR& other) {
    mpfr_init2(_variable, mpfr_get_prec(other._variable));
    mpfr_set(_variable, other._variable, MPFR_RNDN);
}

void MPFR::set_precision(mpfr_prec_t precision) {
    if (precision >= MPFR_PREC_MIN && precision <= MPFR_PREC_MAX) {
        mpfr_prec_round(_variable, precision, MPFR_RNDN);
    } else {
        log_error("Failed to set precision for MPFR variable: %ld", static_cast<long>(precision));
        throw std::invalid_argument("Invalid precision for MPFR");
    }
}


// ASSIGNMENT
MPFR& MPFR::operator=(const MPFR& other) {
    set_precision(mpfr_get_prec(other._variable));
    mpfr_set(_variable, other._variable, MPFR_RNDN);
    return *this;
}


// ADDITION
MPFR MPFR::operator+(const MPFR& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    MPFR result(precision);
    mpfr_add(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

MPFR& MPFR::operator+=(const MPFR& other) {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    set_precision(precision);
    mpfr_add(_variable, _variable, other._variable, MPFR_RNDN);
    return *this;
}


// SUBTRACTION
MPFR MPFR::operator-(const MPFR& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    MPFR result(precision);
    mpfr_sub(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

MPFR operator-(const double& lhs, const MPFR& rhs) {
    mpfr_prec_t precision = std::max(static_cast<mpfr_prec_t>(std::numeric_limits<double>::digits), mpfr_get_prec(rhs._variable));
    MPFR result(precision);
    mpfr_d_sub(result._variable, lhs, rhs._variable, MPFR_RNDN);
    return result;
}

MPFR& MPFR::operator-=(const MPFR& other) {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    set_precision(precision);
    mpfr_sub(_variable, _variable, other._variable, MPFR_RNDN);
    return *this;
}


// MULTIPLICATION
MPFR MPFR::operator*(const MPFR& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    MPFR result(precision);
    mpfr_mul(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

MPFR MPFR::operator*(const double& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), static_cast<mpfr_prec_t>(std::numeric_limits<double>::digits));
    MPFR result(precision);
    mpfr_mul_d(result._variable, _variable, other, MPFR_RNDN);
    return result;
}

MPFR operator*(const double& lhs, const MPFR& rhs) {
    return rhs * lhs;
}

MPFR& MPFR::operator*=(const MPFR& other) {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    set_precision(precision);
    mpfr_mul(_variable, _variable, other._variable, MPFR_RNDN);
    return *this;
}


// DIVISION
MPFR MPFR::operator/(const MPFR& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    MPFR result(precision);
    mpfr_div(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

MPFR& MPFR::operator/=(const MPFR& other) {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    set_precision(precision);
    mpfr_div(_variable, _variable, other._variable, MPFR_RNDN);
    return *this;
}


// LOGIC
bool MPFR::operator==(const MPFR& other) const {
    return mpfr_equal_p(_variable, other._variable);
}

bool MPFR::operator!=(const MPFR& other) const {
    return !mpfr_equal_p(_variable, other._variable);
}

bool MPFR::operator>(const MPFR& other) const {
    return mpfr_greater_p(_variable, other._variable);
}

bool MPFR::operator>=(const MPFR& other) const {
    return mpfr_greaterequal_p(_variable, other._variable);
}

bool MPFR::operator<(const MPFR& other) const {
    return mpfr_less_p(_variable, other._variable);
}

bool MPFR::operator<=(const MPFR& other) const {
    return mpfr_lessequal_p(_variable, other._variable);
}


// OTHER
MPFR arbitrary::log2(const MPFR& value) {
    MPFR result(mpfr_get_prec(value._variable));
    mpfr_log2(result._variable, value._variable, MPFR_RNDN);
    return result;
}

MPFR arbitrary::abs(const MPFR& value) {
    MPFR result(mpfr_get_prec(value._variable));
    mpfr_abs(result._variable, value._variable, MPFR_RNDN);
    return result;
}

MPFR arbitrary::ceil(const MPFR& value) {
    MPFR result(mpfr_get_prec(value._variable));
    mpfr_ceil(result._variable, value._variable);
    return result;
}

MPFR arbitrary::max(const MPFR& a, const MPFR& b) {
    return a > b ? a : b;
}


// NEGATION
MPFR MPFR::operator-() const {
    MPFR result(mpfr_get_prec(_variable));
    mpfr_neg(result._variable, _variable, MPFR_RNDN);
    return result;
}


double MPFR::get_double() const {
    return mpfr_get_d(_variable, MPFR_RNDN);
}

long double MPFR::get_long_double() const {
    return mpfr_get_ld(_variable, MPFR_RNDN);
}

std::string MPFR::get_string() const {
    char* str = nullptr;

    if (mpfr_asprintf(&str, "%.*Rg", static_cast<int>(mpfr_get_prec(_variable)), _variable) < 0) {
        log_error("Failed to convert MPFR to string");
        throw std::runtime_error("Failed to convert MPFR to string");
    }

    std::string result(str);
    mpfr_free_str(str);

    return result;
}

long MPFR::get_long() const {
    return mpfr_get_si(_variable, MPFR_RNDN);
}

MPFR::~MPFR() {
    mpfr_clear(_variable);
}