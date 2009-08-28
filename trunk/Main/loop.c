#include "LPC21XX.h"
#include "loop.h"
#include "adc.h"
#include "setup.h"
#include "uart.h"
#include "main.h"
#include "sdBuf.h"
#include "command.h"
#include "load.h"
#include "sirfwrite.h"
#include "nmeawrite.h"
#include "gps.h"
#include "conparse.h"
#include "debug.h"
#include "sd_raw.h"

static unsigned int lastcap;

static void fillTime(circular* buf) {
  if(YEAR<1000) fill(buf,'0');
  if(YEAR<100) fill(buf,'0');
  if(YEAR<10) fill(buf,'0');
  fillDec(buf,YEAR);
  fill(buf,'/');
  if(MONTH<10) fill(buf,'0');
  fillDec(buf,MONTH);
  fill(buf,'/');
  if(DOM<10) fill(buf,'0');
  fillDec(buf,DOM);
  fill(buf,',');
  if(HOUR<10) fill(buf,'0');
  fillDec(buf,HOUR);
  fill(buf,':');
  if(MIN<10) fill(buf,'0');
  fillDec(buf,MIN);
  fill(buf,':');
  if(SEC<10) fill(buf,'0');
  fillDec(buf,SEC);
}

static void checkPPS(void) {
  unsigned int thiscap=T1CR2 % 60000000;
  if(lastcap!=thiscap) {
    hasLoad(LOAD_LOAD);
    lastcap=thiscap;
    int MSEC=lastcap/60000;
    switch(adcMode) {
      case ADC_TEXT:
        fillString(&sdBuf, "PPS: ");
		fillTime(&sdBuf);
        fill(&sdBuf,'.');
        if(MSEC<100) fill(&sdBuf,'0');
        if(MSEC<10) fill(&sdBuf,'0');
        fillDec(&sdBuf,MSEC);
        fillShort(&sdBuf,0x0D0A);
        mark(&sdBuf);
      case ADC_NMEA:
        fillStartNMEA(&sdBuf,'P');
        fillTime(&sdBuf);
        fill(&sdBuf,'.');
        if(MSEC<100) fill(&sdBuf,'0');
        if(MSEC<10) fill(&sdBuf,'0');
        fillDec(&sdBuf,MSEC);
        fillFinishNMEA(&sdBuf);
        break;
      case ADC_SIRF:
        fillStartSirf(&sdBuf);
        fill(&sdBuf,0x19);
        fillShort(&sdBuf,YEAR);
        fill(&sdBuf,MONTH);
        fill(&sdBuf,DOM);
        fill(&sdBuf,HOUR);
        fill(&sdBuf,MIN);
        fill(&sdBuf,SEC);
        fillInt(&sdBuf,lastcap);
        fillFinishNMEA(&sdBuf);
        break;
    }
  }
}

void loop(void) {
  checkPPS();
#ifdef ADC_SYS  
  if(readylen(&adcBuf)>0) {
    hasLoad(LOAD_ADC);
	  if(isFlushSDNeeded()) {
      hasLoad(LOAD_FLUSH);
      flushSD();
    }
    drain(&adcBuf,&sdBuf);
  }
#endif  
#ifdef UART_SYS  
  for(int i=0;i<2;i++) if(readylen(&uartbuf[i])>0) {
    hasLoad(LOAD_UART);
	  if(isFlushSDNeeded()) {
      hasLoad(LOAD_FLUSH);
      flushSD();
    }
	  if(timestamp[i]) {
      fillTime(&sdBuf);
  	  fill(&sdBuf,' ');
	    fillDec(&sdBuf,i);
	    fill(&sdBuf,' ');
	  }
    drain(&uartbuf[i],&sdBuf);
  }
#endif 
  if(isFlushSDNeeded()) {
    hasLoad(LOAD_FLUSH); 
    flushSD();
  }
#ifdef COMMAND_SYS    
  writeCommand();
#endif    
//  set_light(0,GPSLight);
  // if button pushed, log file & quit
  if((IOPIN0 & 0x00000008) == 0) {
    VICIntEnClr = 0xFFFFFFFF; //Turn off interrupts
    //Write out the last buffers
#ifdef ADC_SYS    
    mark(&adcBuf);
    drain(&adcBuf,&sdBuf);
    flushSD();
#endif    
#ifdef UART_SYS    
    for(int i=0;i<2;i++) {
      mark(&uartbuf[i]);
      drain(&uartbuf[i],&sdBuf);
      flushSD();
    }
#endif    
    flushSDLast();
    sd_raw_sync();
    //quit
    blinklock(1,0);
  }
  set_light(0,GPSLight);
  if(countLoad()) loops++;
  clearLoad();
//  PCON|=1; //idle mode
}

