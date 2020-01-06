#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

extern int interrupt_count;
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t st;
  if(timer_get_conf(timer, &st))
    return 1;
  if(timer_display_conf(timer, st, field))
    return 1;
  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  timer_set_frequency(timer, freq);

  //printf("%s is not yet implemented!\n", __func__);

  return 0;
}

int(timer_test_int)(uint8_t time) {
    int ipc_status;
    message msg;
    unsigned long r;

    uint8_t bit_no = 0; //bit que vai ser subscrito

    if (timer_subscribe_int(&bit_no)) {
         printf("Unable to subscribe interruption!");
         return 1;
    }
    bit_no = BIT(0);  //meter o bit 0 a 1 para detetar quando se gera interrupção=1

    if (time ==0)
        return 1;

    while (interrupt_count/ 60 < time) {  //Timer 0, by default, generates interruptions at a fixed rate, 60 HZ (60 per second).
        //60 interruptions / 60 = 1 second
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status)) { /* received notification */
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: /* hardware interrupt notification */
                    if (msg.m_notify.interrupts & bit_no) {
                        timer_int_handler();  //increment interruptions counter
                        //printf("interrup count:%d\n",interrupt_count);
                        if (interrupt_count% 60 == 0)  //function must invoke once per second, the function below
                            timer_print_elapsed_time();
                    }
                    break;
                default:
                    break; /* no other notifications expected: do nothing */
            }
        }
        else { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    if(timer_unsubscribe_int()){
     printf("Timer was unable to unsubscribe interruptions!");
     return 1;
    }

    return 0;


}
