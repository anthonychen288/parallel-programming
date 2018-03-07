#ifdef _MSC_VER
// MS Visual Studio gives warnings when using
// fopen. But fopen_s is not going to work well
// with most compilers, and fopen_s uses different
// syntax than fopen. (i.e., a macro won't work)
// So, we'lll use this:
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <iostream>
#include <cmath>
#include <cctype>
#include <cstring>

#ifndef MyBMP
#define MyBMP

#ifdef __BCPLUSPLUS__
// The Borland compiler must use this because something
// is wrong with their cstdio file.
#include <stdio.h>
#else
#include <cstdio>
#endif

#ifdef __GNUC__
// If g++ specific code is ever required, this is
// where it goes.
#endif

#ifdef __INTEL_COMPILER
// If Intel specific code is ever required, this is
// where it goes.
#endif

#ifndef _DefaultXPelsPerMeter_
#define _DefaultXPelsPerMeter_
#define DefaultXPelsPerMeter 3780
// set to a default of 96 dpi
#endif

#ifndef _DefaultYPelsPerMeter_
#define _DefaultYPelsPerMeter_
#define DefaultYPelsPerMeter 3780
// set to a default of 96 dpi
#endif

#include "MyBMP_DataStructures.h"
#include "MyBMP_BMP.h"
#include "MyBMP_VariousBMPutilities.h"

#ifndef _MyBMP_Version_
#define _MyBMP_Version_ 1.06
#define _MyBMP_Version_Integer_ 106
#define _MyBMP_Version_String_ "1.06"
#endif

#ifndef _MyBMPwarnings_
#define _MyBMPwarnings_
#endif

void SetMyBMPwarningsOff( void );
void SetMyBMPwarningsOn( void );
bool GetMyBMPwarningState( void );

#endif
