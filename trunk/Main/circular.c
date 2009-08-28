#include <stdlib.h>
#include "circular.h"
#include "stringex.h"

int isFull(circular *buf) {
  return (buf->head+1)%1024==buf->tail;
}

int isEmpty(circular *buf) {
  return buf->head==buf->tail;
}

//returns 0 if character written, negative
//if character could not be written
int fill(circular *buf,char in) {
  if(!isFull(buf)) {
    buf->buf[buf->head]=in;
    buf->head=(buf->head+1)%1024;
    return 0;
  }
  return -1;
}

char get(circular *buf) {
  if(isEmpty(buf)) return 0;
  char result=buf->buf[buf->tail];
  buf->tail=(buf->tail+1)%1024;
  return result;
}

int fillString(circular* buf, char* in) {
  int i=0;
  while(in[i]!=0) {
    int result=fill(buf,in[i]);
    if (result<0) return result;
    i++;
  }
  return 0;
}

int fillDec(circular* buf, int in) {
  if(in<0) {
    int result=fill(buf,'-');
    if(result<0) return result;
    in=-in;
  }
  if(in==0) {
    return fill(buf,'0');
  }
  char dbuf[10];
  int i=0;
  while(in>0) {
    dbuf[i]=(in%10)+'0';
    in/=10;
    i++;
  }
  for(int j=i-1;j>=0;j--) {
    int result=fill(buf,dbuf[j]);
    if(result<0) return result;
  }
  return 0;
}

int fillHex(circular* buf, unsigned int in, int len) {
  int hexit;
  int result;
  for(int i=0;i<len;i++) {
    hexit=(in>>(4*(len-i-1))) & 0x0F;
    result=fill(buf,hexDigits[hexit]);
    if(result!=0) return result;
  }
  return 0;
}

int fillStringn(circular* buf, char* in, int len) {
  for(int i=0;i<len;i++) {
    int result=fill(buf,in[i]);
    if(result<0) return result;
  }
  return 0;
}

int fillShort(circular* buf, short in) {
  int result=fill(buf,(in>> 8) & 0xFF);
  if(result<0) return result;
  return     fill(buf,(in>> 0) & 0xFF);
}

int fillInt(circular* buf, int in) {
  int result=fillShort(buf,(in>> 16) & 0xFFFF);
  if(result<0) return result;
  return     fillShort(buf,(in>>  0) & 0xFFFF);
}

char peekMid(circular* buf, int pos) {
  pos+=buf->mid;
  pos&=0x3FF;
  return buf->buf[pos];
}

char peekHead(circular* buf, int pos) {
  pos+=buf->head;
  pos&=0x3FF;
  return buf->buf[pos];
}

char peekTail(circular* buf, int pos) {
  pos+=buf->tail;
  pos&=0x3FF;
  return buf->buf[pos];
}

void pokeMid(circular* buf, int pos, char poke) {
  pos+=buf->mid;
  pos&=0x3FF;
  buf->buf[pos]=poke;
}

void pokeHead(circular* buf, int pos, char poke) {
  pos+=buf->head;
  pos&=0x3FF;
  buf->buf[pos]=poke;
}

void pokeTail(circular* buf, int pos, char poke) {
  pos+=buf->tail;
  pos&=0x3FF;
  buf->buf[pos]=poke;
}

void mark(circular* buf) {
  buf->mid=buf->head;
}

int unreadylen(circular* buf) {
  int h=buf->head;
  int m=buf->mid;
  if(h<m) h+=1024;
  return h-m;
}

int readylen(circular* buf) {
  int m=buf->mid;
  int t=buf->tail;
  if(m<t) m+=1024;
  return m-t;
}

int drain(circular* from, circular* to) {
  while(readylen(from)>0) {
    if(isFull(to)) return -1;
    fill(to,get(from));
  }
  mark(to);
  return 0;
}

void empty(circular* buf) {
  buf->head=0;
  buf->tail=0;
  buf->mid=0;
}
