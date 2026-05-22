extern "C" {
  void motor(void);
}
#include <math.h>
void setup() {  
  
  __asm__ volatile(
    "ldi r16, 2\n"       
    "ldi r17, 3\n"         
    "mul r17, r16\n"       
    ::: "r16", "r17", "r0", "r1"
);

__asm__ volatile (
    "sbr r16, 0x22\n"  
    "ser r17\n"   
    "sec\n"                
    ::: "r16", "r17"
);

//
__asm__ volatile (
    "ser r30\n"           
    "sts 0x02, r30\n"     
    "sts 0x25, r30\n"      
    "sts 0x100, r30\n"     
    ::: "r30", "memory"
);

__asm__ volatile (
    "ser r16\n"           
    "ldi r30, 0x23\n"   
    "ldi r31, 0x01\n" 
    "st Z, r16\n" 
    ::: "r16", "r30", "r31", "memory"
);

__asm__ volatile (
    "ser r16\n" 
    "push r16\n"           
    "push r16\n"            
    "push r16\n"            
    ::: "r16", "memory"
);

  motor();
}

void loop() {}