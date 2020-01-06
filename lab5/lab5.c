// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>
#include <unistd.h> // header for sleep(delay)
// Any header files included below this line should have been created by you

#include "video_card.h"
#include "video_card.c"
#include "keyboard.h"
#include "keyboard.c"
extern vbe_mode_info_t mem_info;

extern uint8_t scancode;
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
    //SWITCH THE VIDEO ADAPTER TO GRAPHICS MODE SPECIFIED IN ARGUMENT MODE
    //sleep(delay) -> wait delay seconds
    //switch back to text mode



    if(vg_set_mode(mode)!=0){
        printf("Error initializing video test mode");
        return 1;
    }
    sleep(delay);
    if(vg_exit()!=0){
        printf("Error exiting video test mode");
        return 1;
    }






  return 0;
}




int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {


    int ipc_status;

    unsigned long r;
    message msg;

    uint8_t bit_no = 1;


    if (keyboard_subscribe_int(&bit_no)) {
        printf("Unable to subscribe interruption!");
        return 1;
    }
    bit_no = BIT(1);


    if(vg_set_mode(mode)!=0){
        printf("Error initializing video test mode");
        return 1;
    }

  if (mem_info.XResolution<(width+x) || mem_info.YResolution<(height+y)){
    return 1;
  }



  if (vg_draw_rectangle(x,y,width,height,color))
  {
      printf("Problem drawing the rectangle\n");
      return 1;
  }



    while (scancode != 0x81) {
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


    if(keyboard_unsubscribe_int()){
        printf("Keyboard was unable to unsubscribe interruptions!");
        return 1;
    }




    if(vg_exit()!=0){
        printf("Error exiting video test mode");
        return 1;
    }






    return 0;
}










int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {


    int ipc_status;

    unsigned long r;
    message msg;

    uint8_t bit_no = 1;


        if (keyboard_subscribe_int(&bit_no)) {
            printf("Unable to subscribe interruption!");
            return 1;
        }
        bit_no = BIT(1);


    if(vg_set_mode(mode)!=0){
        printf("Error initializing video test mode");
        return 1;
    }
    uint16_t rectangle_width = mem_info.XResolution / no_rectangles;
    uint16_t rectangle_height = mem_info.YResolution / no_rectangles;
    uint16_t right_stripe = mem_info.XResolution % no_rectangles;
    uint16_t bottom_stripe = mem_info.YResolution % no_rectangles;

    //create the matrix

    for (int i = 0; i< no_rectangles; i++){
        if ((i*rectangle_height) >= (mem_info.YResolution - bottom_stripe)) {
            break;
        }

        for (int j = 0; j < no_rectangles; j++){
            if ((j*rectangle_width) >= (mem_info.XResolution - right_stripe)) {
                break;
            }
            if (mode == 0x105){//if mode is indexed
                vg_draw_rectangle(i*rectangle_width, j*rectangle_height, rectangle_width, rectangle_height, (first + (j * no_rectangles + i) * step) % (1 << mem_info.BitsPerPixel));
            }
            else{
                //for the red color, mem_info.RedFieldPosition indicates the LSB of the red part
                //mem_info.RedMaskSize indicates the size of the red mask in bits. if size = 5, and position = 10, red goes form 10 to 14.

                uint32_t red_mask = 0;
                for (int i = mem_info.RedFieldPosition; i< mem_info.RedFieldPosition + mem_info.RedMaskSize; i++){
                    red_mask = red_mask | BIT(i);
                }
                uint32_t green_mask = 0;
                for (int i = mem_info.GreenFieldPosition; i< mem_info.GreenFieldPosition + mem_info.GreenMaskSize; i++){
                    green_mask= green_mask | BIT(i);
                }

                uint32_t blue_mask= 0;
                for (int i = mem_info.BlueFieldPosition; i< mem_info.BlueFieldPosition + mem_info.BlueMaskSize; i++){
                    blue_mask = blue_mask | BIT(i);
                }


                uint32_t red_first = first & red_mask;
                uint32_t green_first = first & green_mask;
                uint32_t blue_first = first & blue_mask;

                uint32_t red =  (red_first + i*step) % (1 << mem_info.RedMaskSize);
                uint32_t green =  (green_first+ j*step) % (1<< mem_info.GreenMaskSize);
                uint32_t blue =  (blue_first + (i+j)*step) % (1 << mem_info.BlueMaskSize);
                vg_draw_rectangle(i*rectangle_width, j*rectangle_height, rectangle_width, rectangle_height, red | green | blue);

            }
        }
    }








    while (scancode != 0x81) {
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





    if(keyboard_unsubscribe_int()){
        printf("Keyboard was unable to unsubscribe interruptions!");
        return 1;
    }




    if(vg_exit()!=0){
        printf("Error exiting video test mode");
        return 1;
    }





  return 0;
}










int colorPixel(unsigned x, unsigned y, unsigned long color){

    if (x >= h_res || y>= v_res){
        printf("Out of bounds\n");
        return 1;
    }
    int bytes_per_pixel = (int)((bits_per_pixel + 7) / 8);
    char* pixel_address=video_mem+(x+y*h_res)*bytes_per_pixel;
    *pixel_address = (uint8_t)color;

    return 0;
}








int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
    int ipc_status;

    unsigned long r;
    message msg;
    uint8_t bit_no = 1;


    if (keyboard_subscribe_int(&bit_no)) {
        printf("Unable to subscribe interruption!");
        return 1;
    }
    bit_no = BIT(1);


    if (vg_set_mode(0x105) != 0) {
        printf("Error initializing video test mode");
        return 1;
    }


    enum xpm_image_type type = XPM_INDEXED;
    xpm_image_t img;
    uint8_t *sprite = xpm_load(xpm, type, &img);

    if (sprite == NULL) {
        printf("Error getting image information!\n");
        return 1;
    }

    //typedef struct {
    //      enum xpm_image_type type;
    //      uint16_t width;
    //      uint16_t height;
    //      size_t size;
    //      uint8_t* bytes;
    //} xpm_image_t;
    //printf("Starting to draw the xpm\n");
    for (int i = 0; i < img.height; i++){ //i = 3                  01   23   45
        for (int j = 0; j < img.width; j++){// j = 3               67   89   1011
                //unsigned long color;
                //Mode 565 has 2 bytes/pixel
                //color = img.bytes[j*2 + 2*i*img.width] | img.bytes[j*2+1 + 2*i*img.width];
                colorPixel(x+j, y+i, img.bytes[i * img.width +j]);

        }
    }
    printf("Should have finished drawing the xpm!\n");







    while (scancode != 0x81) {
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





    if(keyboard_unsubscribe_int()){
        printf("Keyboard was unable to unsubscribe interruptions!");
        return 1;
    }




    if(vg_exit()!=0){
        printf("Error exiting video test mode");
        return 1;
    }
    free(sprite);

  return 0;
}













int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {






  return 0;
}






int(video_test_controller)() {

    vg_vbe_contr_info_t controller_info;
   //get the information of the controller by calling VBE function 0x0
   // Parse the VBE controller information  returned and display it on the console by calling the function below (display (...))
   //members of controller_info must be initialized with the information returned from 0x0

   //TODO




    vg_display_vbe_contr_info(&controller_info);

  return 0;
}
