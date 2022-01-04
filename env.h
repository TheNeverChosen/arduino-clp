#pragma once

//#define DEBUG_ON

#define SERIAL_RATE 9600

#define QT_INP_DIG 6
#define QT_INP_ALG 6
#define QT_OUT_DIG 4

static const unsigned QT_MAX_DEV = QT_INP_ALG + QT_INP_DIG + QT_OUT_DIG;

#define QT_MAX_LD_VAR 20
typedef uint8_t sz_varr;

#define MAX_SZ_PROTOCOL 1000
typedef uint16_t sz_ptc;
