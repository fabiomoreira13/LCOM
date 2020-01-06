#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int interrupt_count = 0;

int hook_id = 0;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (TIMER_FREQ<freq || TIMER_FREQ/freq>0xFFFF) //limitação de numero de 16 bits
    return 1;

  if (timer<0||timer>2)
    return 1;

  uint8_t lsb, msb, config, control;
  
  if(timer_get_conf(timer, &config))
    return 1;

    //printf("Config: 0x%X\n", config);
  
  config=config & 0xF;  //get the last 4 bits from configuration (need to be preserved)
  //printf("Config last bits: 0x%X\n", config);

  uint16_t div=TIMER_FREQ/freq;  //the timer generates a square wave with a frequency given by the expression clock/div, where clock is the frequency of the Clock input and div is the value loaded initially in the timer and only has 16bit counter

  if (util_get_LSB(div, &lsb) || util_get_MSB(div, &msb))
    return 1;

  //printf("Freq: 0x%X\n", div);
  //printf("LSB: 0x%X\n", lsb);
  //printf("MSB: 0x%X\n", msb);

  //control word build, and timer configuration
  switch (timer){

  case 0:
    control=TIMER_SEL0;
    timer=TIMER_0;
    break;

  case 1:
    control=TIMER_SEL1;
    timer=TIMER_1;
    break;

  case 2:
    control=TIMER_SEL2;
    timer=TIMER_2;
    break;
  }


  control=control|config ;//add configuration previous to the control word
  //printf("control|config: 0x%X\n", control);
  control=control|TIMER_LSB_MSB; //state msb and lsb initialization in control word
  //printf("control|timer: 0x%X\n", control);

  if (sys_outb(TIMER_CTRL, control))  //send controlword to timer control
    return 1;

  if(sys_outb(timer, lsb) || sys_outb(timer, msb))
    return 1;
  
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    //must call int sys_irqsetpolicy( int irq_line, int policy, int *hook_id)
    //must return, via its input argument, the value that is passed to *hook_id (value that will be used in the interrupt
    //notifications.

    hook_id=*bit_no;
    //this is done so that the programmer can choose which bit to use, instead of having to stick to what is given, so that he can prevent compatibility issues with other libraries
    if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) !=0){
        printf("Error subscribing interruption!");
        return 1;
    }

    return 0;
}

int (timer_unsubscribe_int)() {
  //must call int sys_irqmpolicy(int *hook_id)
  //unsubscribes a previous interruption notification with the associated hook_id
  if(sys_irqrmpolicy(&hook_id)!=0){
      printf("Error unsubscribing interruption!");
      return 1;
  }

  return 0;
}

void (timer_int_handler)() {
  interrupt_count++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  if (timer<0||timer>2) {
      printf("Please input a valid timer");
      return 1;
  }
  uint8_t read_back= TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  
  if (sys_outb(TIMER_CTRL,read_back)) {
      printf("Error in sys_outb!");
      return 1;
  }
  switch(timer){
    case 0:
    if (util_sys_inb(TIMER_0, st)) {
        printf("Error in utils_sys_inb");
        return 1;
    }
    break;

    case 1:
    if (util_sys_inb(TIMER_1, st)){
        printf("Error in utils_sys_inb");
      return 1;
    }
    break;

    case 2:
    if (util_sys_inb(TIMER_2, st)){
        printf("Error in utils_sys_inb");
        return 1;
    }
    break;
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
    union timer_status_field_val conf;
  /* To be implemented by the students */

  /* -> Parse the value in the second argument, which is the return of the read_back command - 8 BIT NUMBER
   *    -> allows to see the programmed mode, the count value, and the current state of the OUT line of the selected timer.
   *
   * -> third argument specifies which field -> can be :                         must be displayed from the i8254 STATUS WORD
   *                                            -> status byte, in hexadecimal
   *                                            -> initialization mode, only
   *                                            -> counting mode, only
   *                                            -> counting base, only
   *
   *
   *
   * to print, must:
  Must call function int timer_print_config(uint8_t timer, enum timer_status_field field, union timer_status_field_val conf)

   -> the third argument can be:
                                             -> status byte
                                             -> IN_MODE(INITIALIZATIN)
   *                                                    -> INVAL_VAL
   *                                                    -> LSB_ONLY
   *                                                    -> MSB_ONLY
   *                                                    -> MSB_AFTER_LSB
   *                                        -> COUNTING MODE
   *                                        -> OR THE COUNTING BASE, TRUE IF BCD
   */

  //MUST CREATE 4 IF'S DEPENDING ON WHICH FIELD THEY ASK

  if(field ==tsf_all){   //must print the 8 bits
       conf.byte = st;
       if (timer_print_config(timer, field, conf))
             return 1;
       return 0;

  }
  else if(field == tsf_initial) {//display the initialization mode, only
        int aux = BIT(5)|BIT(4);
        printf("0x%X\n",aux);
        aux = st & aux;
        printf ("st:0x%X\n",st);
        printf("aux:,0x%X\n",aux);
        if (aux == BIT(4))
            conf.in_mode = LSB_only;
            //initialization mode = LSB;
        else if (aux ==BIT(5))
            conf.in_mode = MSB_only;
            //initialization mode = MSB;
        else if (aux == (BIT(5)|BIT(4)))
            conf.in_mode = MSB_after_LSB;
            //initialization mode = LSB FOLLOWED BY MSB;
       else
           conf.in_mode = INVAL_val;
           //invalid initialization

        timer_print_config(timer, field, conf);
        return 0;
  }

  else if(field ==tsf_mode) {//display the counting mode only
      int aux = BIT(3)|BIT(2)|BIT(1);
      aux = st & aux;
      if (aux == 0) //The 3 bits plus the final one are 000 0
          conf.count_mode=0;
      else if (aux ==BIT(1) ) //The final 4 bits are 0010
          conf.count_mode = 1;
      else if (aux == BIT(2)|| aux ==(BIT(2)|BIT(3))) //the final 4 bits are 0100 or 1100
          conf.count_mode = 2;
      else if(aux == (BIT(1)|BIT(2)) || aux == (BIT(1)|BIT(2)|BIT(3)))
          conf.count_mode = 3;
      else if (aux == BIT(3))
          conf.count_mode = 4;
      else if (aux == (BIT(1)|BIT(3)))
          conf.count_mode = 5;

       timer_print_config(timer, field, conf);
       return 0;
  }
  else if (field == tsf_base){  //display the counting base only

        //create an aux variable /00000001/ and perform & between them. If aux ==1, it means that the last bit is 1 (...)
        int aux= BIT(0);
        aux = st & aux;
      //if LSB is 0, conf.bcd = false;
      //if lsb is 1, conf.bcd = true;
        if (aux ==0)
            conf.bcd = false;
        else
            conf.bcd = true;

       timer_print_config(timer, field, conf);
       return 0;
  }
    else  return 1;
  printf("%s is not yet implemented!\n", __func__);
  return 0;
}
