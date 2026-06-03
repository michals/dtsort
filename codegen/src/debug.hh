#pragma once
#include <iostream>

#ifdef NDEBUG
#define Debug(x)
#define Debugn(x)
#else
#define Debug(x) std::cerr << x
#define Debugn(x) std::cerr << x << "\n"
#endif
