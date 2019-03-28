
#include "TICKTASK.h"

typedef struct {
    unsigned long       timerCnt[TICK_COUNT_NUM];
}tTICKINFO;

static tTICKINFO tTick;

unsigned long sysTimerGet(unsigned char number)
{
    return (tTick.timerCnt[number]);
}

void sysTimerClr(unsigned char number)
{
    tTick.timerCnt[number] = 0;
}

void timerTick(void)
{
	tTick.timerCnt[0]++;
	tTick.timerCnt[1]++;
}

/*********************************************************************************************************
    end of file
*********************************************************************************************************/
