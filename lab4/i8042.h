#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

#define WRITE_TO_MOUSE 0xD4
#define CTRL_REG 0x64
#define KBC_reg 0x64
#define MOUSE_reg 0x60
#define acknoledgement_reg 0x60
#define mouse_write 0x60
#define OUT_BUF_FULL BIT(0)
#define do_not_write BIT(1)
#define PAR_ERROR BIT(7)
#define TIM_ERROR BIT(6)
#define IBF BIT(1)
#define mouse_read_command 0x20
#define Read_Data (0xEB)
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC
#define Stream_mode 0xEA
#define enable_data_report 0xF4
#define disable_data_report 0xF5
#define write_command_byte 0x60
#define FIRST_BYTE byte_packet[0]
#define ACK 0xFA
#define IBF_FULL BIT(1)

#endif
