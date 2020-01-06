#ifndef LAB3_C_KEYBOARD_H
#define LAB3_C_KEYBOARD_H
//#include "utils.c"





int (keyboard_interrupt_activate());
int (keyboard_subscribe_int)(uint8_t *bit_no);
int (keyboard_unsubscribe_int)();


#endif //LAB3_C_KEYBOARD_H

