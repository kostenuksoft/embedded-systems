#include <avr/io.h>

.global motor

motor:
    ldi r16, 0xff ; HIGH 
    out _SFR_IO_ADDR(DDRD), r16 
    
    ldi r18, 0b10000011 
    out _SFR_IO_ADDR(TCCR0A), r18 
    
    ldi r18, 0b00000011 
    out _SFR_IO_ADDR(TCCR0B), r18  

slow: 
    ldi r19, 40         
    out _SFR_IO_ADDR(OCR0A), r19 
    rcall delay 
    rjmp medium  

medium: 
    ldi r19, 100       
    out _SFR_IO_ADDR(OCR0A), r19 
    rcall delay 
    rjmp fast  

fast: 
    ldi r19, 255    
    out _SFR_IO_ADDR(OCR0A), r19 
    rcall delay 
    rjmp slow  

delay: 
    ldi r24, 121 
    ldi r25, 78 
    ldi r26, 32  

L1: 
    dec r26     
    brne L1     
    dec r25     
    brne L1     
    dec r24     
    brne L1 
    nop 
    ret