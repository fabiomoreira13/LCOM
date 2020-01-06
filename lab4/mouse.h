
#ifndef LAB4_MOUSE_H
#define LAB4_MOUSE_H
#include "i8042.h"
int (mouse_subscribe_int)(uint8_t *bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

void (process_packets)();

void (packets_parse)();

int write_cmd(unsigned long cmd);


#endif //LAB4_MOUSE_H
