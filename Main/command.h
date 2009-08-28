#ifndef command_h
#define command_h

#include "setup.h"
#ifdef UART_SYS

extern int loops;

void writeCommand(void);
int loadCommands(void);

#endif

#endif
