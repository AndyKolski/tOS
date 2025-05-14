#pragma once

#include <printf/printf.h>

char putchar(char chr);

void puts(const char *text);

void VMputchar_args(char c, void *arg);
void VMputchar(char c);
