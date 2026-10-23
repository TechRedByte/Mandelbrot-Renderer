#include <iostream>

#include "arbitrary.h"

int main() {
    MPFR a = "1";
    a.set_precision(100);
    MPFR b = "0.2";
    b.set_precision(100);

    std::printf("Adding 1 and 0.2: %f\n", (a + b).get_double());
}