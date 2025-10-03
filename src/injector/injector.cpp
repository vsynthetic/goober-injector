#include "injector.hpp"
#ifdef _WIN32
#include "injector_win32.cpp"
#else
#include "injector_linux.cpp"
#endif
