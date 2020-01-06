// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
//#include <lcom/lab4.h>
//#include <stdint.h>
#include <stdio.h>


// Any header files included below this line should have been created by you
#include "mouse.h"
extern int counter_mouse;
extern struct packet pp;
extern uint8_t byte_packet[3];
extern bool status_error;

extern uint8_t status_word_mouse;
extern uint8_t byte_read;
extern int interrupt_count;



int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    int ipc_status;

    unsigned long r;
    message msg;

    if (write_cmd(enable_data_report)!= 0){   //0xf4 is for enable
        printf("Error enabling mouse!");
        return 1;
    }





    uint8_t bit_no_mouse =    2;
    if (mouse_subscribe_int(&bit_no_mouse)){
        printf("Unable to subscribe interruptions!");
        return 1;

    }
    bit_no_mouse = BIT(2);



    while (cnt >0) {



        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & bit_no_mouse) {  //if it gets an interruption, must read the scancode from the OUT_BUF

                        mouse_ih();

                        if(status_error) {
                            printf("Error in status word. Printed from lab4.c\n");
                            continue;
                        }
                        else { //there was no error
                            //must check if bit (3) == 1, for the first byte aka counter % 3 ==1
                            if (counter_mouse == 0 && (byte_read & BIT(3))) {
                                byte_packet[0] = byte_read;
                                counter_mouse++;
                            } else if (counter_mouse == 1) {
                                byte_packet[1]=byte_read;
                                counter_mouse++;

                            }
                            else if(counter_mouse== 2) {
                                byte_packet[2]=byte_read;
                                counter_mouse++;

                                packets_parse();
                                mouse_print_packet(&pp);
                                counter_mouse = 0;
                                cnt--;
                            }



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


    if (mouse_unsubscribe_int() != 0 ){
        printf("Error unsubscribe mouse interruptions!");
        return 1;
    }


    if(write_cmd(disable_data_report)!=0){
        printf("Error disabling mouse data reporting!");
        return 1;

    }


    return 0;
}



int write_to_kbc(uint8_t command){
    uint8_t status_word;
    while(true){
            if(sys_outb(KBC_reg, write_command_byte))
                return 1;

        if (util_sys_inb(KBC_reg, &status_word))
            return 1;
        if (status_word & do_not_write){
            tickdelay(micros_to_ticks(20000));
            continue;
        }
            if(sys_outb(MOUSE_reg,command))
                return 1;
            break;
        }

    return 0;
}


int write_to_mouse(uint8_t command){
    uint8_t status_word;
    while(true){
        // if (write_to_kbc(WRITE_TO_MOUSE))
        //     return 1;
        if(sys_outb(KBC_reg, 0xD4))
                return 1;
        
        if (util_sys_inb(KBC_reg, &status_word))
            return 1;
        if (status_word & do_not_write)
            continue;
        if (sys_outb(MOUSE_reg, command))
            return 1;
        
        if (util_sys_inb(acknoledgement_reg, &status_word))
            return 1;
        if (status_word == ACK)
            break;

        if (status_word==ERROR)
            return 1;
        }
    return 0;
    }



int (mouse_test_remote)(uint16_t period, uint8_t cnt) {



    for (uint8_t j=0;j<cnt;j++){
        write_to_mouse(Read_Data);

        while (true){           //check if its the first package (bit3=1)
            if(util_sys_inb(MOUSE_reg, &byte_packet[0]))
                return 1;
            if (byte_packet[0] & BIT(3))
                break;
        }
        for (int i=1; i<=2; i++){  //receive rest of packets
            if(util_sys_inb(MOUSE_reg, &byte_packet[i]))
                return 1;
        }

        packets_parse();
        mouse_print_packet(&pp);
        tickdelay(micros_to_ticks(period));
    }

    if (write_to_mouse(Stream_mode))
        return 1;

    if (write_to_mouse(disable_data_report))
        return 1;

    if (write_to_kbc(minix_get_dflt_kbc_cmd_byte()))
        return 1;

    return 0;
}







int (mouse_test_async)(uint8_t idle_time) {
    int ipc_status;

    unsigned long r;
    message msg;


    if (write_cmd(enable_data_report) != 0) {   //0xf4 is for enable
        printf("Error enabling mouse!");
        return 1;
    }


    uint8_t bit_no_mouse = 2;
    if (mouse_subscribe_int(&bit_no_mouse)) {
        printf("Unable to subscribe interruptions!");
        return 1;

    }
    bit_no_mouse = BIT(2);

    uint8_t bit_no_timer = 0;
    if (timer_subscribe_int(&bit_no_timer) != 0) {
        printf("Timer was unable to subscribe interruptions");
        return 1;
    }
    bit_no_timer = BIT(0);


    bool continue_var = true;
    while (continue_var) {



        //Get a request message.
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { // received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:  //hardware interrupt notification
                    if (msg.m_notify.interrupts & bit_no_timer) {
                        timer_int_handler();

                        if ((interrupt_count / 60) >= idle_time) {
                            continue_var = false;
                        }

                    }


                    if (msg.m_notify.interrupts & bit_no_mouse) {  //if it gets an interruption, must read the scancode from the OUT_BUF

                        interrupt_count = 0;
                        mouse_ih();

                        if (status_error) {
                            printf("Error in status word. Printed from lab4.c\n");
                            continue;
                        } else { //there was no error
                            //must check if bit (3) == 1, for the first byte aka counter % 3 ==1
                            if (counter_mouse == 0 && (byte_read & BIT(3))) {
                                byte_packet[0] = byte_read;
                                counter_mouse++;
                            } else if (counter_mouse == 1) {
                                byte_packet[1] = byte_read;
                                counter_mouse++;

                            } else if (counter_mouse == 2) {
                                byte_packet[2] = byte_read;
                                counter_mouse++;

                                packets_parse();
                                mouse_print_packet(&pp);
                                counter_mouse = 0;

                            }


                        }
                    }
                    tickdelay(micros_to_ticks(20000));
                    break;
                default:
                    break; /// no other notifications expected: do nothing
            }
        } else {//  received a standard message, not a notification
            // no standard messages expected: do nothing
        }

    }


    if (mouse_unsubscribe_int() != 0) {
        printf("Error unsubscribe mouse interruptions!");
        return 1;
    }

    if (timer_unsubscribe_int() != 0) {
        printf("Error unsubscribing timer interruptions!");
        return 1;
    }

    if (write_cmd(disable_data_report) != 0) {
        printf("Error disabling mouse data reporting!");
        return 1;

    }
    return 1;

}







int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    uint8_t x_variacao1=0, y_variacao1=0;
    bool conditions[5]={false, false, false, false, false};
    while (true) {

        mouse_test_packet(1);//to print the packets in each interaction

        //exceptions
        if (pp.mb) {
            conditions[0] = false;
            conditions[1] = false;
            conditions[2] = false;
            conditions[3] = false;
            conditions[4] = false;
            continue;
        }

        if (pp.lb && !(pp.rb || pp.mb || conditions[0])) { //condição inicial
            conditions[0] = true;

    }
        if (conditions[0] && !(conditions[1])){
            if (pp.delta_x>=(0-tolerance) && pp.delta_y>=(0-tolerance) && pp.lb){
                conditions[1]=true;
                while (pp.delta_x>=(0-tolerance) && pp.delta_y>=(0-tolerance) && pp.lb){
                    mouse_test_packet(1);

                    x_variacao1 += pp.delta_x;
                    y_variacao1 += pp.delta_y;
                }
            }else{
                conditions[0]=false;
                continue;
            }
        }
        if (conditions[0] && conditions[1] && !(conditions[2])){ //check if its the end of line 1 correctly



            if (!(pp.lb || pp.mb || pp.rb) && abs(pp.delta_x)<=tolerance && abs(pp.delta_y)<=tolerance ){
                if (x_variacao1 ==0){
                    conditions[0] = conditions[1] = false;
                    y_variacao1 = 0;
                    continue;
                }

                if(y_variacao1 /x_variacao1 <1 || x_variacao1 < x_len) {
                    conditions[0] = conditions[1] = false;
                    x_variacao1 = 0;
                    y_variacao1 = 0;
                    continue; //declive nao esta dentro das especificacoes. Voltar ao inicio do while, restart

                }

                conditions[2]=true; //FINISHED LINE 1

                x_variacao1 = 0;
                y_variacao1 = 0;
                while ((!(pp.lb || pp.mb || pp.rb) && abs(pp.delta_x)<=tolerance && abs(pp.delta_y)<=tolerance)){
                    mouse_test_packet(1);
                    
                }
            }else{
                conditions[0]=false;
                conditions[1]=false;
                continue;
            }
        }

        if (conditions[0] && conditions[1] && conditions[2] && !(conditions[3])){
            if (pp.rb && !(pp.lb || pp.mb)){
                conditions[3]=true;
                /*if (abs(y_variacao1 / x_variacao1) < 1 ){
                    conditions[0]=conditions[1]=conditions[2]=conditions[3]=false;
                }
                while((pp.rb && !(pp.lb || pp.mb))){
                    mouse_test_packet(1);

                }*/
            }else{
                conditions[0]=conditions[1]=conditions[2]=conditions[3]=false;
            }
        }
        if (conditions[3]){
            if (pp.delta_x>=(0-tolerance) && pp.delta_y<=(0+tolerance)){
                conditions[4]=true;
                while (pp.delta_x>=(0-tolerance) && pp.delta_y<=(0+tolerance) && pp.rb){
                    mouse_test_packet(1);

                    x_variacao1 += pp.delta_x;
                    y_variacao1 += pp.delta_y;
                }
                
            }else{
                conditions[0]=false;
                conditions[1]=false;
                conditions[2]=false;
                conditions[3]=false;
                continue;
            }
        }
        
        if (conditions[4] && !(pp.rb)){
            if (x_variacao1 == 0){
                conditions[0] = conditions[1] = conditions[2] = conditions[3] = conditions[4] = false;
                y_variacao1 = 0;
                continue;
            }
            if (abs(y_variacao1 / x_variacao1) < 1 || x_variacao1 < x_len){
                conditions[0] = conditions[1] = conditions[2] = conditions[3] = conditions[4] = false;
                x_variacao1 = 0;
                y_variacao1 = 0;
                continue;
            }
            else break;
        }
            

    }






    return 0;
}
