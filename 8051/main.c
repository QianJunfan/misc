#include <8051.h>

void main(void)
{
        unsigned char i = 0;
        while(1) {
                P1 = ++i;
        }
}
