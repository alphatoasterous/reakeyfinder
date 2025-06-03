#pragma once

#include <string>

namespace PROJECT_NAME {
        extern "C" {
            std::string StringizeSampleBuffer(double* buf, int size);
            std::string FloatToString(double value, int precision = 6);
        }
}