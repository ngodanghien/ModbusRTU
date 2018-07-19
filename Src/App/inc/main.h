#ifndef __MAIN_H
#define __MAIN_H

typedef u32 OS_STK;
typedef u32 INT32U;
typedef struct
{
  u32 *ptrTaskStack;
}OSTCB;

typedef struct OSDely_tag
{
  struct OSDely_tag *next;
  u32 time;
}OSDelay;

extern void TaskSwitch(void);
extern OSDelay OSDelayHead;
extern OSDelay OSDelayOne;
extern OSDelay OSDelaySecond;
extern OSTCB TCBTask[4];
extern OSTCB *OSTCBCur;
extern OSTCB *OSTCBHighRdy;

#endif


