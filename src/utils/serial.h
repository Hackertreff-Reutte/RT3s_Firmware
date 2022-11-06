#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include "ringBuffer.h"
#include "usb/cdcAcmUSB.h"

void print(std::string s);
void print(const char* s);
void print(char* s);
void println(std::string s);
void println(const char* s);
void println(char* s);
std::string readAll();
std::string readln();
char getChar();
#endif