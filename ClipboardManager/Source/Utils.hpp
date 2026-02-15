#pragma once

#include <stdio.h>
#include "./CppPatterns.hpp"

#define LOGL(fmt, ...) { FILE* f = fopen("Data/out.log", "a"); fprintf(f, fmt, ## __VA_ARGS__); fclose(f); }
#define LOGN(fmt, ...) LOGL(fmt "\n", ## __VA_ARGS__)
#define LOG( fmt, ...) LOGL(fmt "\n", ## __VA_ARGS__)
