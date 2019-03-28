#ifndef _TICKTASK_H_
#define _TICKTASK_H_

#define SECOND_PER_TICK     100        
#define TICK_COUNT_NUM       3      
#define TRUE                 1
#define FLASE                0

extern unsigned long    sysTimerGet(unsigned char number);              
extern void             sysTimerClr(unsigned char number);             
extern void             timerTick(void);                               

#endif                                                                
/*********************************************************************************************************
    end of file
*********************************************************************************************************/
