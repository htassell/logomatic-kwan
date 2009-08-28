#ifndef sirfwrite_h
#define sirfwrite_h
#include "circular.h"

void fillStartSirf(circular* buf);
void fillFinishSirf(circular* buf);
void fillFinishSirfSend(circular* buf, int port);

#endif

