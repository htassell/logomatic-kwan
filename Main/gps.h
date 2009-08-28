#ifndef gps_h
#define gps_h

#include "circular.h"

//GPS status
extern int GPSLight;
extern int GPShasPos;

void parseSirf(int mode, circular* sirfBuf);

#endif
