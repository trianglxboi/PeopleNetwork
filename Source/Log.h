#pragma once

#include <iostream>

#define LOG_ERROR(...) { std::cerr << __VA_ARGS__ << '\n'; std::system("pause"); }
