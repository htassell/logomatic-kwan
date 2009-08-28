#include "gps.h"
#include "conparse.h"
#include "circular.h"
#include "LPC21xx.h"
#include "main.h"

//SiRF packet processing and GPS status
int GPSLight;
int GPShasPos;

void parseSirf(int mode, circular* sirfBuf) {
  if(mode!=UART_SIRF) return; //Don't know nothin' 'bout NMEA...
  if(peekMid(sirfBuf,4)==0x29) {
    //Check if we have a position
    GPShasPos=(peekMid(sirfBuf,6)==0) & (peekMid(sirfBuf,5)==0);
	if(GPShasPos) {
	  GPSLight=1;
	  if(GPSSyncMode==1) {
	    YEAR=(((unsigned int)peekMid(sirfBuf,15))<<8) + peekMid(sirfBuf,16);
		  MONTH=peekMid(sirfBuf,17);
		  DOM=peekMid(sirfBuf,18);
		  HOUR=peekMid(sirfBuf,19);
		  MIN=peekMid(sirfBuf,20);
		  SEC=((((unsigned int)peekMid(sirfBuf,21))<<8) + peekMid(sirfBuf,22))/1000;
	  }
	}
  } else if(!GPShasPos && peekMid(sirfBuf,4)==4) {
    //Check if we have at least one sat
    if(peekMid(sirfBuf,12)>0) {
	    GPSLight=1-GPSLight;
  	} else {
	    GPSLight=0;
	  }
  }
}

