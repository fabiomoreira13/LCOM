#include <lcom/lcf.h>
//#include <lcom/keyboard.h>


#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define OUT_BUF 0x60
#define KBD_IRQ                 1
#define ESC 0x81
#define read_command 0x20
#define write_command 0x60



int hook_id_kbd= 1;

uint8_t scancode;
uint8_t status_word;
uint8_t size = 0;
bool make_code = false;
bool read_error = false;
bool print = false;
uint8_t bytes[2];




int (keyboard_interrupt_activate)(){
    uint8_t command_byte;
    if (sys_outb(0x64,read_command))
        return 1;
    if (util_sys_inb(OUT_BUF, &command_byte))
        return 1;
    command_byte=(command_byte|BIT(0));
    if (sys_outb(0x64, write_command))
        return 1;
    if (sys_outb(OUT_BUF, command_byte))
        return 1;
    return 0;
}


int (keyboard_subscribe_int)(uint8_t *bit_no) {
    //must call int sys_irqsetpolicy( int irq_line, int policy, int *hook_id)
    //must return, via its input argument, the value that is passed to *hook_id (value that will be used in the interrupt
    //notifications.

    hook_id_kbd=*bit_no;
    //this is done so that the programmer can choose which bit to use, instead of having to stick to what is given, so that he can prevent compatibility issues with other libraries
    if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE|IRQ_EXCLUSIVE, &hook_id_kbd) !=0){
        printf("Error subscribing interruption!");
        return 1;
    }

    return 0;
}

int (keyboard_unsubscribe_int)(){
    if(sys_irqrmpolicy(&hook_id_kbd)!=0){
        printf("Error unsubscribing interruption!");
        return 1;
    }

    return 0;
}

bool two_byte_code= false;
void (kbc_ih)() {
    //read status word

    util_sys_inb(STAT_REG, &status_word);

    // printf("Status word read\n");
    //check if there were errors
    if (status_word & BIT(0)) {  //IF BIT 0 = 1 and BIT 7 6 5 = 0, no error
        //the output buffer is full,
        //printf("Output buffer full\n");
        util_sys_inb(OUT_BUF, &scancode);


        //printf("Output buffer read\n");

        if ((status_word & BIT(7)) || (status_word & BIT(6)) ||
            (status_word & BIT(5))) { //if one of them  ==1, there is error
            printf("There was an error!\n");
            read_error = true;

        }
        if (read_error == false && (scancode == 0xE0 || scancode == 0xE1)) {
            size = 2;
            bytes[0] = scancode;
            two_byte_code = true;
            print = false;
            return;
        }


    }
    if (read_error == false) {//if there were not, read scan code



        if (two_byte_code) {
            bytes[1] = scancode;
            if (bytes[1] & BIT(7) ) make_code = false;
            else make_code = true;
            print = true;
            two_byte_code = false;
        } else {
            /* if(scancode & BIT(15)){ //two bytes

                 bytes[0] = (uint8_t)(scancode>>8);
                 bytes[1] = (uint8_t)scancode;
                 if(bytes[1] & BIT(7)) make_code = false;
                 else make_code = true;
                 size = 2;
             }
             else{*/ // 1 byte
            two_byte_code = false;
            bytes[0] = (uint8_t) scancode;
            size = 1;
            two_byte_code = false;


            if (scancode & BIT(7)) {
                make_code = false;
            } else
                make_code = true;
            print = true;

            /*
            if (scancode & BIT(15)){ //it means it's 2 bytes

                size = 2;
            }
            else{
                temp_lsb = (uint8_t)(scancode);
                if ((temp_lsb & BIT(7)) == 0) {//its a make code
                    make_code = true;
                }
                size = 1;
            }*/
        }
    }
}
