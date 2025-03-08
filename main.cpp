#include <optional>
#include <iostream>
#include <utility>
#include "optional.h"
#include "unit-test.h"
#include "PerRvalue.h"

int main() {
    try {
        TestInitialization();
        TestAssignment();
        TestMoveAssignment();
        TestValueAccess();
        TestReset();
        TestEmplace();
        TestRvalue();
    }
    catch (...) {
        assert(false);
    }
}
