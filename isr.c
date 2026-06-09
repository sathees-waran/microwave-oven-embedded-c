#include <xc.h>
extern unsigned char sec,min;
extern unsigned char pre_heat;


void __interrupt() isr(void)
{
    static unsigned int count = 0;
    
    if (TMR2IF == 1)
    {    
        if (++count == 1250) // 1sec
        {
            count = 0;
          if(min > 0 || sec > 0)
            {
                if(sec == 0)
                {
                    min--;
                    sec = 59;
                }
                else
                {
                    sec--;
                }
            }
            if(pre_heat>0)
            {
                pre_heat--;
            }
        }
        
        TMR2IF = 0;
    }
}