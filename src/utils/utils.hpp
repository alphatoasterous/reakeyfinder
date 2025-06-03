#pragma once

#include <string>

namespace PROJECT_NAME {
    extern "C" {
        std::string StringizeSampleBuffer(double* buf, int size);
    }
}