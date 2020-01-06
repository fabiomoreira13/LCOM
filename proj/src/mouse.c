#include <lcom/lcf.h>
#include "i8042.h"
//#include "mouse.h"
int hook_id_mouse = 12;
uint8_t status_word_mouse, byte_read;
uint8_t byte_packet[3];
struct packet pp;
int counter_mouse = 0;

bool status_error = false;



int (mouse_subscribe_int)(uint8_t *bit_no){
    hook_id_mouse = *bit_no;
    if (sys_irqsetpolicy(12, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != 0){
        printf("Error subscribing interrupts!");
        return 1;
    }
    return 0;
}

int (mouse_unsubscribe_int)(){
    if (sys_irqrmpolicy(&hook_id_mouse) != 0){
        printf("Error unsubscribing interrupts!");
        return 1;
    }
    return 0;
}


void (mouse_ih)() {
    if(util_sys_inb(CTRL_REG, &status_word_mouse) !=0) return;

    if (status_word_mouse & OUT_BUF_FULL) { // output buffer is full

        if ((status_word_mouse & TIM_ERROR) || (status_word_mouse & PAR_ERROR)) {
            status_error = true;
            printf("There was an error - status word mouse!\n");
            return;
        }
        else{
            if (util_sys_inb(MOUSE_reg, &byte_read) != 0) return; //read the output buffer
        }
    }

    //printf("Concluded the ih!\n");

}








void (packets_parse)(){
    if (FIRST_BYTE & BIT(0))
        pp.lb = true;
    else pp.lb = false;

    if (FIRST_BYTE & BIT(1))
        pp.rb = true;
    else pp.rb = false;

    if(FIRST_BYTE & BIT(2))
        pp.mb = true;
    else pp.mb = false;

    if(FIRST_BYTE & BIT(6))
        pp.x_ov = true;
    else pp.x_ov = false;

    if (FIRST_BYTE & BIT(7))
        pp.y_ov = true;
    else pp.y_ov = false;
    //TODO add condition if delta x ==0. 0 can be both negative and positive
    if (FIRST_BYTE& BIT(4)) //DELTA X IS NEGATIVE
        pp.delta_x = byte_packet[1] -256;
    else pp.delta_x = byte_packet[1];   //fazer asssa

    if (FIRST_BYTE & BIT(5)) //delta y is negative
        pp.delta_y = byte_packet[2] - 256;
    else pp.delta_y = byte_packet[2];

    for (int i = 0; i< 3; i++){
        pp.bytes[i] = byte_packet[i];
    }


}





//function to enable or disable mouse data reporting
int write_cmd(unsigned long cmd)
{

    uint8_t ack;

    do
    {
        if (util_sys_inb(CTRL_REG, &status_word_mouse) != OK)
            return 1;

        if ((status_word_mouse & BIT(1)) == 0)
        {

            if (sys_outb(CTRL_REG, WRITE_TO_MOUSE) != OK)
                return 1;
        }
        else
            continue;

        if (util_sys_inb(CTRL_REG, &status_word_mouse) != OK)
            return-1;

        if ((status_word_mouse & IBF_FULL) == 0)
        {
            if (sys_outb(MOUSE_reg, cmd) != OK)
                return 1;
        }
        else
            continue;

        tickdelay(micros_to_ticks(20000));

        if (util_sys_inb(MOUSE_reg, &ack) != OK)
            return -1;

    } while (ack != ACK);

    return 0;
}





