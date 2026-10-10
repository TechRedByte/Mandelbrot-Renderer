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

DynPrec::DynPrec(const char* value) {
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
        log_error("Failed to construct DynPrec variable: %s", value);
        throw std::invalid_argument("Invalid number for DynPrec");
    }
}

DynPrec::DynPrec(double value) {
    mpfr_init2(_variable, std::numeric_limits<double>::digits);
    mpfr_set_d(_variable, value, MPFR_RNDN);
}

DynPrec::DynPrec(long double value) {
    mpfr_init2(_variable, std::numeric_limits<long double>::digits);
    mpfr_set_ld(_variable, value, MPFR_RNDN);
}

DynPrec::DynPrec(mpfr_prec_t precision) {
    if (precision > MPFR_PREC_MIN && precision < MPFR_PREC_MAX) {
        mpfr_init2(_variable, precision);
    } else {
        log_error("Failed to construct DynPrec variable: %lo", precision);
        throw std::invalid_argument("Invalid precision for DynPrec");
    }
}

DynPrec::DynPrec(const DynPrec& other) {
    mpfr_init2(_variable, mpfr_get_prec(other._variable));
    mpfr_set(_variable, other._variable, MPFR_RNDN);
}

void DynPrec::set_precision(unsigned int precision) {
    if (precision > MPFR_PREC_MIN) {
        mpfr_prec_round(_variable, precision, MPFR_RNDN);
    } else {
        log_error("Failed to construct DynPrec variable: %i", precision);
        throw std::invalid_argument("Invalid precision for DynPrec");
    }
}

DynPrec& DynPrec::operator=(const DynPrec& other) {
    mpfr_set(_variable, other._variable, MPFR_RNDN);
    return *this;
}

DynPrec DynPrec::operator+(const DynPrec& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    DynPrec result(precision);
    mpfr_add(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

DynPrec DynPrec::operator-(const DynPrec& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    DynPrec result(precision);
    mpfr_sub(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

DynPrec DynPrec::operator*(const DynPrec& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    DynPrec result(precision);
    mpfr_mul(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

DynPrec DynPrec::operator/(const DynPrec& other) const {
    mpfr_prec_t precision = std::max(mpfr_get_prec(_variable), mpfr_get_prec(other._variable));
    DynPrec result(precision);
    mpfr_div(result._variable, _variable, other._variable, MPFR_RNDN);
    return result;
}

DynPrec& DynPrec::operator-() {
    mpfr_neg(_variable, _variable, MPFR_RNDN);
    return *this;
}

bool DynPrec::operator==(const DynPrec& other) const {
    return mpfr_equal_p(_variable, other._variable);
}

bool DynPrec::operator!=(const DynPrec& other) const {
    return !mpfr_equal_p(_variable, other._variable);
}

bool DynPrec::operator>(const DynPrec& other) const {
    return mpfr_greater_p(_variable, other._variable);
}

bool DynPrec::operator>=(const DynPrec& other) const {
    return mpfr_greaterequal_p(_variable, other._variable);
}

bool DynPrec::operator<(const DynPrec& other) const {
    return mpfr_less_p(_variable, other._variable);
}

bool DynPrec::operator<=(const DynPrec& other) const {
    return mpfr_lessequal_p(_variable, other._variable);
}

double DynPrec::get_double() const {
    return mpfr_get_d(_variable, MPFR_RNDN);
}

long double DynPrec::get_long_double() const {
    return mpfr_get_ld(_variable, MPFR_RNDN);
}

std::string DynPrec::get_string() const {
    char* str = nullptr;

    if (mpfr_asprintf(&str, "%.*Rg", static_cast<int>(mpfr_get_prec(_variable)), _variable) < 0) {
        log_error("Failed to convert DynPrec to string");
        throw std::runtime_error("Failed to convert DynPrec to string");
    }

    std::string result(str);
    mpfr_free_str(str);

    return result;
}

DynPrec::~DynPrec() {
    mpfr_clear(_variable);
}

void update_precision(Dimensions&) {

}