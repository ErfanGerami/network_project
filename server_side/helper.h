
#ifndef HELPER_H

#define HELPER_H


#include <stdio.h>
#include <stdbool.h>

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define BLACK   "\x1b[30m"
#define PURPLE  "\x1b[35m"
#define RESET   "\x1b[0m"

void changeColor(const char* color);
void ERROR(bool _exit,const char* message);
int min(int a,int b);
#endif
