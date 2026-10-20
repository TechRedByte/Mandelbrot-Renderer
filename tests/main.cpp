#include <cassert>
#include "arbitrary.h"

int main() {
    DynPrec a(2.0);
    DynPrec b(3.0);

    assert((a + b).get_double() == 5.0);
    assert((b - a).get_double() == 1.0);
    assert((a * b).get_double() == 6.0);
    assert((b / a).get_double() == 1.5);
}