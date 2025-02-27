#include <optional>
#include <iostream>
#include "optional.h"
#include "unit-test.h"

int main() {
    try {
        TestInitialization();
        TestAssignment();
        TestMoveAssignment();
        TestValueAccess();
        TestReset();
    }
    catch (...) {
        assert(false);
    }
}
