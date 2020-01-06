#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <keyboard.h>
#include <stdbool.h>

/*
#include "keyboard.h"
#include "utils.c"
*/
//#define LAB3;

//#define IRQ_1   1;



extern uint8_t scancode;
extern uint8_t status_word;
extern uint8_t size ;
extern uint8_t bytes[];
extern bool make_code, read_error;
extern int n_sys_inb_calls;
extern int interrupt_count ;
extern bool print;

extern bool two_byte_code;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
    int ipc_status;

    unsigned long r;
    message msg;
    //Must first subscribe the KBC interrupts
    //Every time the IH receives a byte from the keyboard, kbd_test_scan() should try to assemble a scancode.
    // If it succeeds it should print it on the console, by calling the following function that we provide you:
    // int kbd_print_scancode(bool make, uint8_t size, uint8_t bytes[]);
    uint8_t bit_no = 1;
    if (keyboard_subscribe_int(&bit_no)) {
        printf("Unable to subscribe interruption!");
        return 1;
    }
    bit_no = BIT(1);



    while (scancode != 0x81) { //use macro ESC
        //Everytime the IH receives a byte from the keyboard, kbd_test_scan() should try to assemble a scan code
        //if succeeds, call int kbd_print_scancode(bool make, uint8_t size, uint8_t bytes[]);

        //when function calls sys_inb, sys_inb_counter++;
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts &bit_no) {  //if it gets an interruption, must read the scancode from the OUT_BUF
                        kbc_ih();
                        if (read_error) {//discard read
                            continue;

                        } else { //there was no error
                            if (print == true) kbd_print_scancode(make_code, size, bytes);




                        }
                    }
                    tickdelay(micros_to_ticks(20000));
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

    }




    //the kbd_test_scan should exit when the user releases the ESC key, , whose code break is 0x81, must cancel subscription of the KBC
    //interrupt before exiting
    if(keyboard_unsubscribe_int()){
        printf("Keyboard was unable to unsubscribe interruptions!");
        return 1;
    }
    kbd_print_no_sysinb(n_sys_inb_calls);

    return 0;
}

int(kbd_test_poll)() {

    while (scancode!=0x81/*ESC*/){
        bool make_code=true;
        util_sys_inb(0x64/*STAT_REG*/, &status_word);
        //util_sys_inb(STAT_REG, *statusword);
        if (status_word & BIT(0)){
            if (util_sys_inb(0x60 /*OUT_BUF*/, &scancode))
                return 1;

            bytes[0]=scancode;
            size=1;

            if (scancode==0xE0||scancode==0xE1){
                if (util_sys_inb(0x60/*OUT_BUF*/, &scancode))
                    return 1;
                bytes[1]=scancode;
                size=2;
            }
            if (scancode & BIT(7) /*MSB*/){
                make_code=false;
            }
            if (!(status_word & (BIT(7) | BIT(6) | BIT(5)))){
                if (kbd_print_scancode(make_code,size,bytes))
                    return 1;
            }
        }
    }
#ifdef LAB3
    if (kbd_print_no_sysinb(n_sys_inb_calls))
    return 1;
#endif

    if (keyboard_interrupt_activate()){
        return 1;
    }

    return 0;
}


int(kbd_test_timed_scan)(uint8_t n) {

    uint8_t bitno0=0, bitno1=2;
    int ipc_status;

    unsigned long r;
    message msg;
    //uint32_t irq_set0 = BIT(0); //timer_id
    //uint32_t irq_set1 = BIT(1); //kbd id
    if (timer_subscribe_int(&bitno0)!=0){
        printf("Timer was unable to subscribe interruptions");
        return 1;
    }
    bitno0 = BIT(0);
    if (keyboard_subscribe_int(&bitno1)!=0){
        printf("Keyboard was unable to subscribe interruptions!");
        return 1;
    }
    bitno1 = BIT(2);

    while (scancode != 0x81 && (interrupt_count/60) < n) { //use macro ESC

        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts &bitno1) {  //if it gets an interruption, must read the scancode from the OUT_BUF
                        kbc_ih();
                        if (read_error) {//discard read
                            continue;

                        } else { //there was no error
                            if (print == true) kbd_print_scancode(make_code, size, bytes);
                            interrupt_count = 0;


                        }
                    }
                    else if(msg.m_notify.interrupts &bitno0){
                        timer_int_handler();
                        break;
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        } else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

    }

    if(keyboard_unsubscribe_int()){
        printf("Keyboard was unable to unsubscribe interruptions!");
        return 1;
    }

    if(timer_unsubscribe_int()) {
        printf("Timer was unable to unsubscribe interruptions!");
    }







    return 0;
}


