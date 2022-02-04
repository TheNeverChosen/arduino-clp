#pragma once

#include <Arduino.h>
#include <stdlib.h>

// #define DEBUG_ON

#define npos ((size_t)-1)

#define BAUD_RATE 9600

const uint8_t verSz = 3;
const PROGMEM uint8_t version[verSz] = {0,0,0};
const PROGMEM uint8_t reference = 1;

void get_version(uint8_t *version);
uint8_t get_reference();

#define QT_IN_DG 6
#define QT_IN_AL 6
#define QT_OUT_DG 6

static const unsigned QT_MAX_DEV = QT_IN_DG + QT_IN_AL + QT_OUT_DG;

#define MIN(x, y) (x<y ? x : y)

#define QT_MAX_LD_VAR 256 //caracas
typedef uint8_t sz_varr;

#define MAX_SZ_PROTOCOL 2000
typedef uint16_t sz_ptc;


