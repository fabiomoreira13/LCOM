// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include "i8254.h"




#include <stdbool.h>
#include <stdint.h>
// Any header files included below this line should have been created by you
#include "rtc.h"
#include "keyboard.h"
#include "mouse.h"
#include "video_card.h"



#include "sprites/menu_xpm.xpm"
#include "sprites/play_button.xpm"
#include "sprites/exit_button.xpm"
#include "sprites/left_button.xpm"
#include "sprites/escape_button.xpm"
#include "sprites/highscore.xpm"
#include "sprites/highscore_message.xpm"


#include "sprites/gameimage.xpm"
#include "sprites/gameimage_5.xpm"
#include "sprites/gameimage_6.xpm"
#include "sprites/gameimage_7.xpm"
#include "sprites/victoryscreen.xpm"
#include "sprites/victoryscreenv2.xpm"
#include "sprites/gameover.xpm"
#include "sprites/gameover_no0.xpm"
#include "sprites/datedot.xpm"


#include "sprites/letterA.xpm"
#include "sprites/letterB.xpm"
#include "sprites/letterC.xpm"
#include "sprites/letterD.xpm"
#include "sprites/letterE.xpm"
#include "sprites/letterF.xpm"
#include "sprites/letterG.xpm"
#include "sprites/letterH.xpm"
#include "sprites/letterI.xpm"
#include "sprites/letterJ.xpm"
#include "sprites/letterK.xpm"
#include "sprites/letterL.xpm"
#include "sprites/letterM.xpm"
#include "sprites/letterN.xpm"
#include "sprites/letterO.xpm"
#include "sprites/letterP.xpm"
#include "sprites/letterQ.xpm"
#include "sprites/letterR.xpm"
#include "sprites/letterS.xpm"
#include "sprites/letterT.xpm"
#include "sprites/letterU.xpm"
#include "sprites/letterV.xpm"
#include "sprites/letterW.xpm"
#include "sprites/letterX.xpm"
#include "sprites/letterY.xpm"
#include "sprites/letterZ.xpm"
#include "sprites/number0.xpm"
#include "sprites/number1.xpm"
#include "sprites/number2.xpm"
#include "sprites/number3.xpm"
#include "sprites/number4.xpm"
#include "sprites/number5.xpm"
#include "sprites/number6.xpm"
#include "sprites/number7.xpm"
#include "sprites/number8.xpm"
#include "sprites/number9.xpm"
#include "sprites/numberslash.xpm"
#include "sprites/date9.xpm"
#include "sprites/date8.xpm"
#include "sprites/date7.xpm"
#include "sprites/date6.xpm"
#include "sprites/date5.xpm"
#include "sprites/date4.xpm"
#include "sprites/date3.xpm"
#include "sprites/date2.xpm"
#include "sprites/date1.xpm"
#include "sprites/date0.xpm"
#include "sprites/dateslash.xpm"





//Macros to disable/enable data reporting on the mouse
#define enable_date_report 0xF4
#define disable_data_report 0xF5

#define HIGHSCORE_PATH "home/lcom/labs/proj/highscores.txt"


//Breakcodes of the letters
#define A 0x9E
#define B 0xB0
#define C 0XAE
#define D 0xA0
#define E 0x92
#define F 0xA1
#define G 0XA2
#define H 0xA3
#define I 0x97
#define J 0xA4
#define K 0XA5
#define L 0xA6
#define M 0xB2
#define N 0xB1
#define O 0X98
#define P 0x99
#define Q 0x90
#define R 0x93
#define S 0X9F
#define T 0x94
#define U 0x96
#define V 0xAF
#define W 0X91
#define X 0xAD
#define Y 0x95
#define Z 0xAC



//The following x coordinates are the positions where the letters should be drawn when playing the ga,e
#define x1 20
#define x2 135
#define x3 255
#define x4 360
#define x5 485
#define x6 610
#define y1 345

//Y coordinate of the letters to be drawn on the Game Over screen
#define y_pos_wrong 485



int gameFlow(char* string);
int gameOver(char* string);
int victoryScreen();
int mainMenu();
char* getRandomWord();


//Arrays containing the real date/time
extern uint8_t realDate[];
extern uint8_t realTime[];

extern uint8_t scancode;
extern int counter_mouse;
extern struct packet pp;
extern uint8_t byte_packet[3];
extern bool status_error;
extern uint8_t status_word_mouse;
extern uint8_t byte_read;

//Global variables to handle the score of the player
int number_of_wins = 0;
int current_highscore = 0; //Will be updated accordding to the information on the highscores text file


vbe_mode_info_t mem_info;

void draw_xpm(xpm_image_t sprite, int x, int y){
    for (int i = 0;i< sprite.height; i++){
        for (int j = 0; j< sprite.width; j++){
            colorPixel(x+j, y+i, sprite.bytes[j*2 + 2*i*sprite.width] | (sprite.bytes[j*2+1 + 2*i*sprite.width]));
        }
    }
}


int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {



    mainMenu();
    return 0;



}


/**
 * Function to display the main menu of the game. It will:
 * - Display current date and time
 * - Display the current high score
 * - Display the options and instructions (to play or exit)
 * @return 1 if an error occurred somewhere, 0 otherwise
 */
int mainMenu(){
    if (vg_set_mode(0x114)!=0){
        printf("Error initializing video test mode");
        return 1;
    }


    xpm_image_t menuSprite;
    xpm_image_t play_sprite;
    xpm_image_t exit_sprite;
    xpm_image_t left_button_sprite;
    xpm_image_t escape_button_sprite;
    xpm_image_t date0_sprite;
    xpm_image_t date1_sprite;
    xpm_image_t date2_sprite;
    xpm_image_t date3_sprite;
    xpm_image_t date4_sprite;
    xpm_image_t date5_sprite;
    xpm_image_t date6_sprite;
    xpm_image_t date7_sprite;
    xpm_image_t date8_sprite;
    xpm_image_t date9_sprite;
    xpm_image_t dateslash_sprite;
    xpm_image_t datedot_sprite;
    xpm_image_t highscore_sprite;


    uint8_t* sprite = xpm_load(menu_xpm_xpm, XPM_5_6_5, &menuSprite);
    uint8_t* sprite_play = xpm_load(play_button_xpm, XPM_5_6_5, &play_sprite);
    uint8_t* sprite_exit = xpm_load(exit_button_xpm, XPM_5_6_5, &exit_sprite);
    uint8_t *sprite_date0 = xpm_load(date0_xpm, XPM_5_6_5, &date0_sprite);
    uint8_t *sprite_date1 = xpm_load(date1_xpm, XPM_5_6_5, &date1_sprite);
    uint8_t *sprite_date2 = xpm_load(date2_xpm, XPM_5_6_5, &date2_sprite);
    uint8_t *sprite_date3 = xpm_load(date3_xpm, XPM_5_6_5, &date3_sprite);
    uint8_t *sprite_date4 = xpm_load(date4_xpm, XPM_5_6_5, &date4_sprite);
    uint8_t *sprite_date5 = xpm_load(date5_xpm, XPM_5_6_5, &date5_sprite);
    uint8_t *sprite_date6 = xpm_load(date6_xpm, XPM_5_6_5, &date6_sprite);
    uint8_t *sprite_date7 = xpm_load(date7_xpm, XPM_5_6_5, &date7_sprite);
    uint8_t *sprite_date8 = xpm_load(date8_xpm, XPM_5_6_5, &date8_sprite);
    uint8_t *sprite_date9 = xpm_load(date9_xpm, XPM_5_6_5, &date9_sprite);
    uint8_t *sprite_dateslash = xpm_load(dateslash_xpm, XPM_5_6_5, &dateslash_sprite);
    uint8_t* sprite_datedot = xpm_load(datedot_xpm, XPM_5_6_5, &datedot_sprite);
    uint8_t* sprite_highscore = xpm_load(highscore_xpm, XPM_5_6_5, &highscore_sprite);
    uint8_t* sprite_left = xpm_load(left_button_xpm, XPM_5_6_5, &left_button_sprite);
    uint8_t* sprite_escape = xpm_load(escape_button_xpm, XPM_5_6_5, &escape_button_sprite);


    //Drawing menu and the play and exit buttons!
    draw_xpm(menuSprite, 0,0);
    draw_xpm(play_sprite, 300, 220);
    draw_xpm(left_button_sprite, 240, 295);
    draw_xpm(exit_sprite, 300, 355);
    draw_xpm(escape_button_sprite, 250, 420);
    draw_xpm(highscore_sprite, 200, 125);



    //GETTING HIGHSCORE FROM FILE!!
    FILE* fptr;
    fptr = fopen(HIGHSCORE_PATH, "r");

    if (fptr != NULL){  //FILE IS OPENED

        fscanf(fptr, "%d", &current_highscore);


        switch(current_highscore){
            case(0):
                draw_xpm(date0_sprite, 550, 150);
                break;
            case(1):
                draw_xpm(date1_sprite, 550, 150);
                break;
            case(2):
                draw_xpm(date2_sprite, 550, 150);
                break;
            case(3):
                draw_xpm(date3_sprite, 550, 150);
                break;
            case(4):
                draw_xpm(date4_sprite, 550, 150);
                break;
            case(5):
                draw_xpm(date5_sprite, 550, 150);
                break;
            case(6):
                draw_xpm(date6_sprite, 550, 150);
                break;
            case(7):
                draw_xpm(date7_sprite, 550, 150);
                break;
            case(8):
                draw_xpm(date8_sprite, 550, 150);
                break;
            case(9):
                draw_xpm(date9_sprite, 550, 150);
                break;
        }


        //TODO DO CASE IF HIGHSCORE IS >= 10





        fclose(fptr);
    }
    free(fptr);





    rtc_start(); //get date and time
    //TODO IT TAKES A SECOND FOR TIME TO APPEAR

    int day_digit = bcd_to_dec(realDate[0]);
    int month_digit = bcd_to_dec(realDate[1]);
    char day[3];
    char month[3];

    //GETTING DAY!
    sprintf(day, "%d", day_digit); //Converting int to string, so we can have a general way of printing the date

    if (day_digit >= 10) {
        //2 digits will be needed
        switch (day[0]) {
            case ('1'):
                draw_xpm(date1_sprite, 0, 550);
                break;
            case ('2'):
                draw_xpm(date2_sprite, 0, 550);
                break;
            case ('3'):
                draw_xpm(date3_sprite, 0, 550);
                break;

        }
        switch (day[1]) {
            case ('0'):
                draw_xpm(date0_sprite, 30, 550);
                break;
            case ('1'):
                draw_xpm(date1_sprite, 30, 550);
                break;
            case ('2'):
                draw_xpm(date2_sprite, 30, 550);
                break;
            case ('3'):
                draw_xpm(date3_sprite, 30, 550);
                break;
            case ('4'):
                draw_xpm(date4_sprite, 30, 550);
                break;
            case ('5'):
                draw_xpm(date5_sprite, 30, 550);
                break;
            case ('6'):
                draw_xpm(date6_sprite,  30, 550);
                break;
            case ('7'):
                draw_xpm(date7_sprite, 30, 550);
                break;
            case ('8'):
                draw_xpm(date8_sprite, 30, 550);
                break;
            case ('9'):
                draw_xpm(date9_sprite, 30, 550);
                break;
        }
    }
    else{
        //Only 1 digit will be needed
        draw_xpm(date0_sprite, 0, 550);
        switch (day[0]) {
            case ('0'):
                draw_xpm(date0_sprite, 30, 550);
                break;
            case ('1'):
                draw_xpm(date1_sprite, 30, 550);
                break;
            case ('2'):
                draw_xpm(date2_sprite, 30, 550);
                break;
            case ('3'):
                draw_xpm(date3_sprite, 30, 550);
                break;
            case ('4'):
                draw_xpm(date4_sprite, 30, 550);
                break;
            case ('5'):
                draw_xpm(date5_sprite, 30, 550);
                break;
            case ('6'):
                draw_xpm(date6_sprite,  30, 550);
                break;
            case ('7'):
                draw_xpm(date7_sprite, 30, 550);
                break;
            case ('8'):
                draw_xpm(date8_sprite, 30, 550);
                break;
            case ('9'):
                draw_xpm(date9_sprite, 30, 550);
                break;
        }
    }


    draw_xpm(dateslash_sprite,60, 550);
    //Day finished
//------------------------------------------------------------


    //Converting int to string, so we can have a general way to draw the date
    sprintf(month, "%d", month_digit);

    //DRAW_MONTH
    if (month_digit >= 10) {
        draw_xpm(date1_sprite, 90, 550);

        switch (month[1]) {
            case ('0'):
                draw_xpm(date0_sprite, 120, 550);
                break;
            case ('1'):
                draw_xpm(date1_sprite, 120, 550);
                break;
            case ('2'):
                draw_xpm(date2_sprite, 120, 550);
                break;

        }
    }
    else{
        draw_xpm(date0_sprite, 90, 550);
        switch (month[0]) {
            case ('0'):
                draw_xpm(date0_sprite, 120, 550);
                break;
            case ('1'):
                draw_xpm(date1_sprite,  120, 550);
                break;
            case ('2'):
                draw_xpm(date2_sprite,  120, 550);
                break;
            case ('3'):
                draw_xpm(date3_sprite, 120, 550);
                break;
            case ('4'):
                draw_xpm(date4_sprite,  120, 550);
                break;
            case ('5'):
                draw_xpm(date5_sprite, 120, 550);
                break;
            case ('6'):
                draw_xpm(date6_sprite,  120, 550);
                break;
            case ('7'):
                draw_xpm(date7_sprite, 120, 550);
                break;
            case ('8'):
                draw_xpm(date8_sprite,  120, 550);
                break;
            case ('9'):
                draw_xpm(date9_sprite,  120, 550);
                break;
        }
    }

    draw_xpm(dateslash_sprite, 150, 550);

    //Month finished
    //------------------------------------------------------------
    //Drawing year
    draw_xpm(date2_sprite, 180, 550);
    draw_xpm(date0_sprite,210, 550);
    draw_xpm(date2_sprite, 240, 550);
    draw_xpm(date0_sprite,270, 550);

    draw_xpm(datedot_sprite, 560, 550);
    draw_xpm(datedot_sprite, 650, 550);


    if (write_cmd(enable_date_report)!= 0){
        printf("Error enabling mouse!");
        return 1;
    }

    uint8_t bit_no_timer = 0;
    timer_subscribe_int(&bit_no_timer);
    bit_no_timer = BIT(0);


    uint8_t bit_no_kbd = 1;
    keyboard_subscribe_int(&bit_no_kbd);
    bit_no_kbd= BIT(1);


    uint8_t bit_no_mouse = 2;
    mouse_subscribe_int(&bit_no_mouse);
    bit_no_mouse = BIT(2);



    int ipc_status; unsigned long r; message msg;
    int interrupt_count = 0, hour_digit, minute_digit, second_digit;
    char hour[3];
    char minute[3];
    char second[3];
    while (scancode != 0x81) {
        // Get a request message.
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with\n");
            continue;
        }
        if (is_ipc_notify(ipc_status)) { // received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: //hardware interrupt notification

                    if (msg.m_notify.interrupts &
                        bit_no_mouse) {  //if it gets an interruption, must read the scancode from the OUT_BUF

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
                                //mouse_print_packet(&pp);
                                counter_mouse = 0;
                                if (pp.lb) {
                                    free(sprite);
                                    free(sprite_play);
                                    free(sprite_exit);
                                    free(sprite_date0);
                                    free(sprite_date1);
                                    free(sprite_date2);
                                    free(sprite_date3);
                                    free(sprite_date4);
                                    free(sprite_date5);
                                    free(sprite_date6);
                                    free(sprite_date7);
                                    free(sprite_date8);
                                    free(sprite_date9);
                                    free(sprite_dateslash);
                                    free(sprite_datedot);
                                    free(sprite_highscore);
                                    free(sprite_left);
                                    free(sprite_escape);

                                    keyboard_unsubscribe_int();
                                    mouse_unsubscribe_int();
                                    timer_unsubscribe_int();

                                    if(write_cmd(disable_data_report)!=0){
                                        printf("Error disabling mouse data reporting!");
                                        return 1;

                                    }

                                    if (gameFlow(getRandomWord()) == 0){
                                        return 0;
                                    }
                                    else{
                                        printf("Error occurred during the game!\n");
                                        return 1;
                                    }
                                }


                            }

                        }
                    }
                    if (msg.m_notify.interrupts &
                        bit_no_kbd) {  //if it gets an interruption from the keyboard

                        kbc_ih();

                    }
                    if (msg.m_notify.interrupts & bit_no_timer){
                        interrupt_count++;
                        if (interrupt_count % 60 == 0){ //Update time every second

                            rtc_get_hours();
                            rtc_get_minutes();
                            rtc_get_seconds();
                            hour_digit = bcd_to_dec(realTime[0]);
                            minute_digit = bcd_to_dec(realTime[1]);
                            second_digit = bcd_to_dec(realTime[2]);

                            sprintf(hour, "%d", hour_digit);
                            sprintf(minute, "%d", minute_digit);
                            sprintf(second, "%d", second_digit);

                            //Drawing hours
                            if (hour_digit >= 10) {
                                switch (hour[0]) {
                                    case ('1'):
                                        draw_xpm(date1_sprite, 500, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 500, 550);
                                        break;


                                }
                                switch (hour[1]) {
                                    case ('0'):
                                        draw_xpm(date0_sprite, 530, 550);
                                        break;
                                    case ('1'):
                                        draw_xpm(date1_sprite, 530, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 530, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite, 530, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite, 530, 550);
                                        break;
                                    case ('5'):
                                        draw_xpm(date5_sprite, 530, 550);
                                        break;
                                    case ('6'):
                                        draw_xpm(date6_sprite, 530, 550);
                                        break;
                                    case ('7'):
                                        draw_xpm(date7_sprite,  530, 550);
                                        break;
                                    case ('8'):
                                        draw_xpm(date8_sprite, 530, 550);
                                        break;
                                    case ('9'):
                                        draw_xpm(date9_sprite, 530, 550);
                                        break;
                                }
                            }
                            else{
                                draw_xpm(date0_sprite, 500, 550);
                                switch (hour[0]) {
                                    case ('0'):
                                        draw_xpm(date0_sprite, 530, 550);
                                        break;
                                    case ('1'):
                                        draw_xpm(date1_sprite, 530, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 530, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite, 530, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite, 530, 550);
                                        break;
                                    case ('5'):
                                        draw_xpm(date5_sprite,530, 550);
                                        break;
                                    case ('6'):
                                        draw_xpm(date6_sprite,  530, 550);
                                        break;
                                    case ('7'):
                                        draw_xpm(date7_sprite, 530, 550);
                                        break;
                                    case ('8'):
                                        draw_xpm(date8_sprite, 530, 550);
                                        break;
                                    case ('9'):
                                        draw_xpm(date9_sprite, 530, 550);
                                        break;
                                }
                            }
                            //------------------------------------------------------------------------------------------
                            //Drawing minutes
                            if (minute_digit >= 10) {
                                switch (minute[0]) {
                                    case ('1'):
                                        draw_xpm(date1_sprite, 590, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 590, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite, 590, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite, 590, 550);
                                        break;
                                    case('5'):
                                        draw_xpm(date5_sprite, 590, 550);
                                        break;
                                }
                                switch (minute[1]) {
                                    case ('0'):
                                        draw_xpm(date0_sprite, 620, 550);
                                        break;
                                    case ('1'):
                                        draw_xpm(date1_sprite, 620, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 620, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite, 620, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite, 620, 550);
                                        break;
                                    case ('5'):
                                        draw_xpm(date5_sprite, 620, 550);
                                        break;
                                    case ('6'):
                                        draw_xpm(date6_sprite, 620, 550);
                                        break;
                                    case ('7'):
                                        draw_xpm(date7_sprite, 620, 550);
                                        break;
                                    case ('8'):
                                        draw_xpm(date8_sprite, 620, 550);
                                        break;
                                    case ('9'):
                                        draw_xpm(date9_sprite, 620, 550);
                                        break;
                                }
                            }
                            else{
                                draw_xpm(date0_sprite, 590, 550);
                                switch (minute[0]) {
                                    case ('0'):
                                        draw_xpm(date0_sprite,620, 550);
                                        break;
                                    case ('1'):
                                        draw_xpm(date1_sprite, 620, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 620, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite,  620, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite,  620, 550);
                                        break;
                                    case ('5'):
                                        draw_xpm(date5_sprite, 620, 550);
                                        break;
                                    case ('6'):
                                        draw_xpm(date6_sprite, 620, 550);
                                        break;
                                    case ('7'):
                                        draw_xpm(date7_sprite,  620, 550);
                                        break;
                                    case ('8'):
                                        draw_xpm(date8_sprite, 620, 550);
                                        break;
                                    case ('9'):
                                        draw_xpm(date9_sprite, 620, 550);
                                        break;
                                }
                            }
                            //------------------------------------------------------------------------------------------
                            //Drawing seconds
                            if (second_digit >= 10) {
                                switch (second[0]) {
                                    case ('1'):
                                        draw_xpm(date1_sprite, 680, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 680, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite, 680, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite, 680, 550);
                                        break;
                                    case('5'):
                                        draw_xpm(date5_sprite, 680, 550);
                                        break;
                                }
                                switch (second[1]) {
                                    case ('0'):
                                        draw_xpm(date0_sprite, 710, 550);
                                        break;
                                    case ('1'):
                                        draw_xpm(date1_sprite, 710, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 710, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite, 710, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite,710, 550);
                                        break;
                                    case ('5'):
                                        draw_xpm(date5_sprite, 710, 550);
                                        break;
                                    case ('6'):
                                        draw_xpm(date6_sprite, 710, 550);
                                        break;
                                    case ('7'):
                                        draw_xpm(date7_sprite, 710, 550);
                                        break;
                                    case ('8'):
                                        draw_xpm(date8_sprite, 710, 550);
                                        break;
                                    case ('9'):
                                        draw_xpm(date9_sprite, 710, 550);
                                        break;
                                }
                            }
                            else{
                                draw_xpm(date0_sprite, 680, 550);
                                switch (second[0]) {
                                    case ('0'):
                                        draw_xpm(date0_sprite, 710, 550);
                                        break;
                                    case ('1'):
                                        draw_xpm(date1_sprite, 710, 550);
                                        break;
                                    case ('2'):
                                        draw_xpm(date2_sprite, 710, 550);
                                        break;
                                    case ('3'):
                                        draw_xpm(date3_sprite,  710, 550);
                                        break;
                                    case ('4'):
                                        draw_xpm(date4_sprite,  710, 550);
                                        break;
                                    case ('5'):
                                        draw_xpm(date5_sprite,710, 550);
                                        break;
                                    case ('6'):
                                        draw_xpm(date6_sprite, 710, 550);
                                        break;
                                    case ('7'):
                                        draw_xpm(date7_sprite,  710, 550);
                                        break;
                                    case ('8'):
                                        draw_xpm(date8_sprite, 710, 550);
                                        break;
                                    case ('9'):
                                        draw_xpm(date9_sprite, 710, 550);
                                        break;
                                }
                            }
                        }
                    }







                    tickdelay(micros_to_ticks(20000));

                    break;
                default:
                    break; // no other notifications expected: do nothing
            }
        } else { // received a standard message, not a notification
            // no standard messages expected: do nothing
        }

    }


    free(sprite);
    free(sprite_play);
    free(sprite_exit);
    free(sprite_date0);
    free(sprite_date1);
    free(sprite_date2);
    free(sprite_date3);
    free(sprite_date4);
    free(sprite_date5);
    free(sprite_date6);
    free(sprite_date7);
    free(sprite_date8);
    free(sprite_date9);
    free(sprite_dateslash);
    free(sprite_datedot);
    free(sprite_highscore);
    free(sprite_left);
    free(sprite_escape);


    if(vg_exit()!=0){
        printf("Error exiting video test mode");
        return 1;
    }



    mouse_unsubscribe_int();

    keyboard_unsubscribe_int();

    timer_unsubscribe_int();

    if(write_cmd(disable_data_report)!=0){
        printf("Error disabling mouse data reporting!");
        return 1;

    }

    return 0;
}


/**
 * Generates a random word from the available list of words
 * @return The word randomly generated
 */
char* getRandomWord(){
    int random_number;

    for (int i = 0; i < rand() %20 + 5 ;i++){
        random_number = rand() %1511 + 1;
    }

    switch(random_number){
        case(1):
            return "lcom";
        case(2):
            return "pool";
        case(3):
            return "feup";
        case(4):
            return "sprite";
        case(5):
            return "porto";
        case(6):
            return "hotel";
        case(7):
            return "pixels";
        case(8):
            return "april";
        case(9):
            return "star";
        case(10):
            return "color";
        case(11):
            return "house";
        case(12):
            return "mouse";
        case(13):
            return "video";
        case(14):
            return "fabio";
        case(15):
            return "timer";
        case(16):
            return "luis";
        case(17):
            return "acid";
        case(18):
            return "aged";
        case(19):
            return "also";
        case(20):
            return "area";
        case(21):
            return "army";
        case(22):
            return "away";
        case(23):
            return "baby";
        case(24):
            return "back";
        case(25):
            return "ball";
        case(26):
            return "band";
        case(27):
            return "bank";
        case(28):
            return "base";
        case(29):
            return "bath";
        case(30):
            return "bear";
        case(31):
            return "beat";
        case(32):
            return "been";
        case(33):
            return "beer";
        case(34):
            return "bell";
        case(35):
            return "belt";
        case(36):
            return "best";
        case(37):
            return "bill";
        case(38):
            return "bird";
        case(39):
            return "blow";
        case(40):
            return "blue";
        case(41):
            return "boat";
        case(42):
            return "body";
        case(43):
            return "bomb";
        case(44):
            return "bond";
        case(45):
            return "bone";
        case(46):
            return "book";
        case(47):
            return "boom";
        case(48):
            return "born";
        case(49):
            return "boss";
        case(50):
            return "both";
        case(51):
            return "bowl";
        case(52):
            return "bulk";
        case(53):
            return "burn";
        case(54):
            return "bush";
        case(55):
            return "busy";
        case(56):
            return "call";
        case(57):
            return "calm";
        case(58):
            return "came";
        case(59):
            return "camp";
        case(60):
            return "card";
        case(61):
            return "care";
        case(62):
            return "case";
        case(63):
            return "cash";
        case(64):
            return "cast";
        case(65):
            return "cell";
        case(66):
            return "chat";
        case(67):
            return "chip";
        case(68):
            return "city";
        case(69):
            return "club";
        case(70):
            return "coal";
        case(71):
            return "coat";
        case(72):
            return "code";
        case(73):
            return "cold";
        case(74):
            return "come";
        case(75):
            return "cook";
        case(76):
            return "cool";
        case(77):
            return "cope";
        case(78):
            return "copy";
        case(79):
            return "CORE";
        case(80):
            return "cost";
        case(81):
            return "crew";
        case(82):
            return "crop";
        case(83):
            return "dark";
        case(84):
            return "data";
        case(85):
            return "date";
        case(86):
            return "dawn";
        case(87):
            return "days";
        case(88):
            return "dead";
        case(89):
            return "deal";
        case(90):
            return "dean";
        case(91):
            return "dear";
        case(92):
            return "debt";
        case(93):
            return "deep";
        case(94):
            return "deny";
        case(95):
            return "desk";
        case(96):
            return "dial";
        case(97):
            return "dick";
        case(98):
            return "diet";
        case(99):
            return "disc";
        case(100):
            return "disk";
        case(101):
            return "does";
        case(102):
            return "done";
        case(103):
            return "door";
        case(104):
            return "dose";
        case(105):
            return "down";
        case(106):
            return "draw";
        case(107):
            return "drew";
        case(108):
            return "drop";
        case(109):
            return "drug";
        case(110):
            return "dual";
        case(111):
            return "duke";
        case(112):
            return "dust";
        case(113):
            return "duty";
        case(114):
            return "each";
        case(115):
            return "earn";
        case(116):
            return "ease";
        case(117):
            return "east";
        case(118):
            return "easy";
        case(119):
            return "edge";
        case(120):
            return "else";
        case(121):
            return "even";
        case(122):
            return "ever";
        case(123):
            return "evil";
        case(124):
            return "exit";
        case(125):
            return "face";
        case(126):
            return "fact";
        case(127):
            return "fail";
        case(128):
            return "fair";
        case(129):
            return "fall";
        case(130):
            return "farm";
        case(131):
            return "fast";
        case(132):
            return "fate";
        case(133):
            return "fear";
        case(134):
            return "feed";
        case(135):
            return "feel";
        case(136):
            return "feet";
        case(137):
            return "fell";
        case(138):
            return "felt";
        case(139):
            return "file";
        case(140):
            return "fill";
        case(141):
            return "film";
        case(142):
            return "find";
        case(143):
            return "fine";
        case(144):
            return "fire";
        case(145):
            return "firm";
        case(146):
            return "fish";
        case(147):
            return "five";
        case(148):
            return "flat";
        case(149):
            return "flow";
        case(150):
            return "food";
        case(151):
            return "foot";
        case(152):
            return "ford";
        case(153):
            return "form";
        case(154):
            return "fort";
        case(155):
            return "four";
        case(156):
            return "free";
        case(157):
            return "from";
        case(158):
            return "fuel";
        case(159):
            return "full";
        case(160):
            return "fund";
        case(161):
            return "gain";
        case(162):
            return "game";
        case(163):
            return "gate";
        case(164):
            return "gave";
        case(165):
            return "gear";
        case(166):
            return "gene";
        case(167):
            return "gift";
        case(168):
            return "girl";
        case(169):
            return "give";
        case(170):
            return "glad";
        case(171):
            return "goal";
        case(172):
            return "goes";
        case(173):
            return "gold";
        case(174):
            return "Golf";
        case(175):
            return "gone";
        case(176):
            return "good";
        case(177):
            return "gray";
        case(178):
            return "grew";
        case(179):
            return "grey";
        case(180):
            return "grow";
        case(181):
            return "gulf";
        case(182):
            return "hair";
        case(183):
            return "half";
        case(184):
            return "hall";
        case(185):
            return "hand";
        case(186):
            return "hang";
        case(187):
            return "hard";
        case(188):
            return "harm";
        case(189):
            return "hate";
        case(190):
            return "have";
        case(191):
            return "head";
        case(192):
            return "hear";
        case(193):
            return "heat";
        case(194):
            return "held";
        case(195):
            return "hell";
        case(196):
            return "help";
        case(197):
            return "here";
        case(198):
            return "hero";
        case(199):
            return "high";
        case(200):
            return "hill";
        case(201):
            return "hire";
        case(202):
            return "hold";
        case(203):
            return "hole";
        case(204):
            return "holy";
        case(205):
            return "home";
        case(206):
            return "hope";
        case(207):
            return "host";
        case(208):
            return "hour";
        case(209):
            return "huge";
        case(210):
            return "hung";
        case(211):
            return "hunt";
        case(212):
            return "hurt";
        case(213):
            return "idea";
        case(214):
            return "inch";
        case(215):
            return "into";
        case(216):
            return "iron";
        case(217):
            return "item";
        case(218):
            return "jack";
        case(219):
            return "jane";
        case(220):
            return "jean";
        case(221):
            return "john";
        case(222):
            return "join";
        case(223):
            return "jump";
        case(224):
            return "jury";
        case(225):
            return "just";
        case(226):
            return "keen";
        case(227):
            return "keep";
        case(228):
            return "kent";
        case(229):
            return "kept";
        case(230):
            return "kick";
        case(231):
            return "kill";
        case(232):
            return "kind";
        case(233):
            return "king";
        case(234):
            return "knee";
        case(235):
            return "knew";
        case(236):
            return "know";
        case(237):
            return "lack";
        case(238):
            return "lady";
        case(239):
            return "laid";
        case(240):
            return "lake";
        case(241):
            return "land";
        case(242):
            return "lane";
        case(243):
            return "last";
        case(244):
            return "late";
        case(245):
            return "lead";
        case(246):
            return "left";
        case(247):
            return "less";
        case(248):
            return "life";
        case(249):
            return "lift";
        case(250):
            return "like";
        case(251):
            return "line";
        case(252):
            return "link";
        case(253):
            return "list";
        case(254):
            return "live";
        case(255):
            return "load";
        case(256):
            return "loan";
        case(257):
            return "lock";
        case(258):
            return "logo";
        case(259):
            return "long";
        case(260):
            return "look";
        case(261):
            return "lord";
        case(262):
            return "lose";
        case(263):
            return "loss";
        case(264):
            return "lost";
        case(265):
            return "love";
        case(266):
            return "luck";
        case(267):
            return "made";
        case(268):
            return "mail";
        case(269):
            return "main";
        case(270):
            return "make";
        case(271):
            return "male";
        case(272):
            return "many";
        case(273):
            return "Mark";
        case(274):
            return "mass";
        case(275):
            return "meal";
        case(276):
            return "mean";
        case(277):
            return "meat";
        case(278):
            return "meet";
        case(279):
            return "menu";
        case(280):
            return "mere";
        case(281):
            return "mike";
        case(282):
            return "mile";
        case(283):
            return "milk";
        case(284):
            return "mill";
        case(285):
            return "mind";
        case(286):
            return "mine";
        case(287):
            return "miss";
        case(288):
            return "mode";
        case(289):
            return "mood";
        case(290):
            return "moon";
        case(291):
            return "more";
        case(292):
            return "most";
        case(293):
            return "move";
        case(294):
            return "much";
        case(295):
            return "must";
        case(296):
            return "name";
        case(297):
            return "navy";
        case(298):
            return "near";
        case(299):
            return "neck";
        case(300):
            return "need";
        case(301):
            return "news";
        case(302):
            return "next";
        case(303):
            return "nice";
        case(304):
            return "nick";
        case(305):
            return "nine";
        case(306):
            return "none";
        case(307):
            return "nose";
        case(308):
            return "note";
        case(309):
            return "okay";
        case(310):
            return "once";
        case(311):
            return "only";
        case(312):
            return "onto";
        case(313):
            return "open";
        case(314):
            return "oral";
        case(315):
            return "over";
        case(316):
            return "pace";
        case(317):
            return "pack";
        case(318):
            return "page";
        case(319):
            return "paid";
        case(320):
            return "pain";
        case(321):
            return "pair";
        case(322):
            return "palm";
        case(323):
            return "park";
        case(324):
            return "part";
        case(325):
            return "pass";
        case(326):
            return "past";
        case(327):
            return "path";
        case(328):
            return "peak";
        case(329):
            return "pick";
        case(330):
            return "pink";
        case(331):
            return "pipe";
        case(332):
            return "plan";
        case(333):
            return "play";
        case(334):
            return "plot";
        case(335):
            return "plug";
        case(336):
            return "plus";
        case(337):
            return "poll";
        case(338):
            return "pool";
        case(339):
            return "poor";
        case(340):
            return "port";
        case(341):
            return "post";
        case(342):
            return "pull";
        case(343):
            return "pure";
        case(344):
            return "push";
        case(345):
            return "race";
        case(346):
            return "rail";
        case(347):
            return "rain";
        case(348):
            return "rank";
        case(349):
            return "rare";
        case(350):
            return "rate";
        case(351):
            return "read";
        case(352):
            return "real";
        case(353):
            return "rear";
        case(354):
            return "rely";
        case(355):
            return "rent";
        case(356):
            return "rest";
        case(357):
            return "rice";
        case(358):
            return "rich";
        case(359):
            return "ride";
        case(360):
            return "ring";
        case(361):
            return "rise";
        case(362):
            return "risk";
        case(363):
            return "road";
        case(364):
            return "rock";
        case(365):
            return "role";
        case(366):
            return "roll";
        case(367):
            return "roof";
        case(368):
            return "room";
        case(369):
            return "root";
        case(370):
            return "rose";
        case(371):
            return "rule";
        case(372):
            return "rush";
        case(373):
            return "ruth";
        case(374):
            return "safe";
        case(375):
            return "said";
        case(376):
            return "sake";
        case(377):
            return "sale";
        case(378):
            return "salt";
        case(379):
            return "same";
        case(380):
            return "sand";
        case(381):
            return "save";
        case(382):
            return "seat";
        case(383):
            return "seed";
        case(384):
            return "seek";
        case(385):
            return "seem";
        case(386):
            return "seen";
        case(387):
            return "self";
        case(388):
            return "sell";
        case(389):
            return "send";
        case(390):
            return "sent";
        case(391):
            return "sept";
        case(392):
            return "ship";
        case(393):
            return "shop";
        case(394):
            return "shot";
        case(395):
            return "show";
        case(396):
            return "shut";
        case(397):
            return "sick";
        case(398):
            return "side";
        case(399):
            return "sign";
        case(400):
            return "site";
        case(401):
            return "size";
        case(402):
            return "skin";
        case(403):
            return "slip";
        case(404):
            return "slow";
        case(405):
            return "snow";
        case(406):
            return "soft";
        case(407):
            return "soil";
        case(408):
            return "sold";
        case(409):
            return "sole";
        case(410):
            return "some";
        case(411):
            return "song";
        case(412):
            return "soon";
        case(413):
            return "sort";
        case(414):
            return "soul";
        case(415):
            return "spot";
        case(416):
            return "star";
        case(417):
            return "stay";
        case(418):
            return "step";
        case(419):
            return "stop";
        case(420):
            return "such";
        case(421):
            return "suit";
        case(422):
            return "sure";
        case(423):
            return "take";
        case(424):
            return "tale";
        case(425):
            return "talk";
        case(426):
            return "tall";
        case(427):
            return "tank";
        case(428):
            return "tape";
        case(429):
            return "task";
        case(430):
            return "team";
        case(431):
            return "tech";
        case(432):
            return "tell";
        case(433):
            return "tend";
        case(434):
            return "term";
        case(435):
            return "test";
        case(436):
            return "text";
        case(437):
            return "than";
        case(438):
            return "that";
        case(439):
            return "them";
        case(440):
            return "then";
        case(441):
            return "they";
        case(442):
            return "thin";
        case(443):
            return "this";
        case(444):
            return "thus";
        case(445):
            return "till";
        case(446):
            return "time";
        case(447):
            return "tiny";
        case(448):
            return "told";
        case(449):
            return "toll";
        case(450):
            return "tone";
        case(451):
            return "tony";
        case(452):
            return "took";
        case(453):
            return "tool";
        case(454):
            return "tour";
        case(455):
            return "town";
        case(456):
            return "tree";
        case(457):
            return "trip";
        case(458):
            return "true";
        case(459):
            return "tune";
        case(460):
            return "turn";
        case(461):
            return "twin";
        case(462):
            return "type";
        case(463):
            return "unit";
        case(464):
            return "upon";
        case(465):
            return "used";
        case(466):
            return "user";
        case(467):
            return "vary";
        case(468):
            return "vast";
        case(469):
            return "very";
        case(470):
            return "vice";
        case(471):
            return "view";
        case(472):
            return "vote";
        case(473):
            return "wage";
        case(474):
            return "wait";
        case(475):
            return "wake";
        case(476):
            return "walk";
        case(477):
            return "wall";
        case(478):
            return "want";
        case(479):
            return "ward";
        case(480):
            return "warm";
        case(481):
            return "wash";
        case(482):
            return "wave";
        case(483):
            return "ways";
        case(484):
            return "weak";
        case(485):
            return "wear";
        case(486):
            return "week";
        case(487):
            return "well";
        case(488):
            return "went";
        case(489):
            return "were";
        case(490):
            return "west";
        case(491):
            return "what";
        case(492):
            return "when";
        case(493):
            return "whom";
        case(494):
            return "wide";
        case(495):
            return "wife";
        case(496):
            return "wild";
        case(497):
            return "will";
        case(498):
            return "wind";
        case(499):
            return "wine";
        case(500):
            return "wing";
        case(501):
            return "wire";
        case(502):
            return "wise";
        case(503):
            return "wish";
        case(504):
            return "with";
        case(505):
            return "wood";
        case(506):
            return "word";
        case(507):
            return "wore";
        case(508):
            return "work";
        case(509):
            return "yard";
        case(510):
            return "yeah";
        case(511):
            return "year";
        case(512):
            return "your";
        case(513):
            return "zero";
        case(514):
            return "zone";
        case(515):
            return "about";
        case(516):
            return "above";
        case(517):
            return "abuse";
        case(518):
            return "actor";
        case(519):
            return "acute";
        case(520):
            return "admit";
        case(521):
            return "adopt";
        case(522):
            return "adult";
        case(523):
            return "after";
        case(524):
            return "again";
        case(525):
            return "agent";
        case(526):
            return "agree";
        case(527):
            return "ahead";
        case(528):
            return "alarm";
        case(529):
            return "album";
        case(530):
            return "alert";
        case(531):
            return "alike";
        case(532):
            return "alive";
        case(533):
            return "allow";
        case(534):
            return "alone";
        case(535):
            return "along";
        case(536):
            return "alter";
        case(537):
            return "among";
        case(538):
            return "anger";
        case(539):
            return "Angle";
        case(540):
            return "angry";
        case(541):
            return "apart";
        case(542):
            return "apple";
        case(543):
            return "apply";
        case(544):
            return "arena";
        case(545):
            return "argue";
        case(546):
            return "arise";
        case(547):
            return "array";
        case(548):
            return "aside";
        case(549):
            return "asset";
        case(550):
            return "audio";
        case(551):
            return "audit";
        case(552):
            return "avoid";
        case(553):
            return "award";
        case(554):
            return "aware";
        case(555):
            return "badly";
        case(556):
            return "baker";
        case(557):
            return "bases";
        case(558):
            return "basic";
        case(559):
            return "basis";
        case(560):
            return "beach";
        case(561):
            return "began";
        case(562):
            return "begin";
        case(563):
            return "begun";
        case(564):
            return "being";
        case(565):
            return "below";
        case(566):
            return "bench";
        case(567):
            return "billy";
        case(568):
            return "birth";
        case(569):
            return "black";
        case(570):
            return "blame";
        case(571):
            return "blind";
        case(572):
            return "block";
        case(573):
            return "blood";
        case(574):
            return "board";
        case(575):
            return "boost";
        case(576):
            return "booth";
        case(577):
            return "bound";
        case(578):
            return "brain";
        case(579):
            return "brand";
        case(580):
            return "bread";
        case(581):
            return "break";
        case(582):
            return "breed";
        case(583):
            return "brief";
        case(584):
            return "bring";
        case(585):
            return "broad";
        case(586):
            return "broke";
        case(587):
            return "brown";
        case(588):
            return "build";
        case(589):
            return "built";
        case(590):
            return "buyer";
        case(591):
            return "cable";
        case(592):
            return "calif";
        case(593):
            return "carry";
        case(594):
            return "catch";
        case(595):
            return "cause";
        case(596):
            return "chain";
        case(597):
            return "chair";
        case(598):
            return "chart";
        case(599):
            return "chase";
        case(600):
            return "cheap";
        case(601):
            return "check";
        case(602):
            return "chest";
        case(603):
            return "chief";
        case(604):
            return "child";
        case(605):
            return "china";
        case(606):
            return "chose";
        case(607):
            return "civil";
        case(608):
            return "claim";
        case(609):
            return "class";
        case(610):
            return "clean";
        case(611):
            return "clear";
        case(612):
            return "click";
        case(613):
            return "clock";
        case(614):
            return "close";
        case(615):
            return "coach";
        case(616):
            return "coast";
        case(617):
            return "could";
        case(618):
            return "count";
        case(619):
            return "court";
        case(620):
            return "cover";
        case(621):
            return "craft";
        case(622):
            return "crash";
        case(623):
            return "cream";
        case(624):
            return "crime";
        case(625):
            return "cross";
        case(626):
            return "crowd";
        case(627):
            return "crown";
        case(628):
            return "curve";
        case(629):
            return "cycle";
        case(630):
            return "daily";
        case(631):
            return "dance";
        case(632):
            return "dated";
        case(633):
            return "dealt";
        case(634):
            return "death";
        case(635):
            return "debut";
        case(636):
            return "delay";
        case(637):
            return "depth";
        case(638):
            return "doing";
        case(639):
            return "doubt";
        case(640):
            return "dozen";
        case(641):
            return "draft";
        case(642):
            return "drama";
        case(643):
            return "drawn";
        case(644):
            return "dream";
        case(645):
            return "dress";
        case(646):
            return "drill";
        case(647):
            return "drink";
        case(648):
            return "drive";
        case(649):
            return "drove";
        case(650):
            return "dying";
        case(651):
            return "eager";
        case(652):
            return "early";
        case(653):
            return "earth";
        case(654):
            return "eight";
        case(655):
            return "elite";
        case(656):
            return "empty";
        case(657):
            return "enemy";
        case(658):
            return "enjoy";
        case(659):
            return "enter";
        case(660):
            return "entry";
        case(661):
            return "equal";
        case(662):
            return "error";
        case(663):
            return "event";
        case(664):
            return "every";
        case(665):
            return "exact";
        case(666):
            return "exist";
        case(667):
            return "extra";
        case(668):
            return "faith";
        case(669):
            return "false";
        case(670):
            return "fault";
        case(671):
            return "fiber";
        case(672):
            return "field";
        case(673):
            return "fifth";
        case(674):
            return "fifty";
        case(675):
            return "fight";
        case(676):
            return "final";
        case(677):
            return "first";
        case(678):
            return "fixed";
        case(679):
            return "flash";
        case(680):
            return "fleet";
        case(681):
            return "floor";
        case(682):
            return "fluid";
        case(683):
            return "focus";
        case(684):
            return "force";
        case(685):
            return "forth";
        case(686):
            return "forty";
        case(687):
            return "forum";
        case(688):
            return "found";
        case(689):
            return "frame";
        case(690):
            return "frank";
        case(691):
            return "fraud";
        case(692):
            return "fresh";
        case(693):
            return "front";
        case(694):
            return "fruit";
        case(695):
            return "fully";
        case(696):
            return "funny";
        case(697):
            return "giant";
        case(698):
            return "given";
        case(699):
            return "glass";
        case(700):
            return "globe";
        case(701):
            return "going";
        case(702):
            return "grace";
        case(703):
            return "grade";
        case(704):
            return "grand";
        case(705):
            return "grant";
        case(706):
            return "grass";
        case(707):
            return "great";
        case(708):
            return "green";
        case(709):
            return "gross";
        case(710):
            return "group";
        case(711):
            return "grown";
        case(712):
            return "guard";
        case(713):
            return "guess";
        case(714):
            return "guest";
        case(715):
            return "guide";
        case(716):
            return "happy";
        case(717):
            return "harry";
        case(718):
            return "heart";
        case(719):
            return "heavy";
        case(720):
            return "hence";
        case(721):
            return "henry";
        case(722):
            return "horse";
        case(723):
            return "hotel";
        case(724):
            return "house";
        case(725):
            return "human";
        case(726):
            return "ideal";
        case(727):
            return "image";
        case(728):
            return "index";
        case(729):
            return "inner";
        case(730):
            return "input";
        case(731):
            return "issue";
        case(732):
            return "japan</a";
        case(733):
            return "joint";
        case(734):
            return "jones";
        case(735):
            return "judge";
        case(736):
            return "known";
        case(737):
            return "label";
        case(738):
            return "large";
        case(739):
            return "laser";
        case(740):
            return "later";
        case(741):
            return "laugh";
        case(742):
            return "layer";
        case(743):
            return "learn";
        case(744):
            return "lease";
        case(745):
            return "least";
        case(746):
            return "leave";
        case(747):
            return "legal";
        case(748):
            return "level";
        case(749):
            return "lewis";
        case(750):
            return "light";
        case(751):
            return "limit";
        case(752):
            return "links";
        case(753):
            return "lives";
        case(754):
            return "local";
        case(755):
            return "logic";
        case(756):
            return "loose";
        case(757):
            return "lower";
        case(758):
            return "lucky";
        case(759):
            return "lunch";
        case(760):
            return "lying";
        case(761):
            return "magic";
        case(762):
            return "major";
        case(763):
            return "maker";
        case(764):
            return "march";
        case(765):
            return "maria";
        case(766):
            return "match";
        case(767):
            return "maybe";
        case(768):
            return "mayor";
        case(769):
            return "meant";
        case(770):
            return "media";
        case(771):
            return "metal";
        case(772):
            return "might";
        case(773):
            return "minor";
        case(774):
            return "minus";
        case(775):
            return "mixed";
        case(776):
            return "model";
        case(777):
            return "money";
        case(778):
            return "month";
        case(779):
            return "moral";
        case(780):
            return "motor";
        case(781):
            return "mount";
        case(782):
            return "mouse";
        case(783):
            return "mouth";
        case(784):
            return "movie";
        case(785):
            return "music";
        case(786):
            return "needs";
        case(787):
            return "never";
        case(788):
            return "newly";
        case(789):
            return "night";
        case(790):
            return "noise";
        case(791):
            return "north";
        case(792):
            return "noted";
        case(793):
            return "novel";
        case(794):
            return "nurse";
        case(795):
            return "occur";
        case(796):
            return "ocean";
        case(797):
            return "offer";
        case(798):
            return "often";
        case(799):
            return "order";
        case(800):
            return "other";
        case(801):
            return "ought";
        case(802):
            return "paint";
        case(803):
            return "panel";
        case(804):
            return "paper";
        case(805):
            return "party";
        case(806):
            return "peace";
        case(807):
            return "peter";
        case(808):
            return "phase";
        case(809):
            return "phone";
        case(810):
            return "photo";
        case(811):
            return "piece";
        case(812):
            return "pilot";
        case(813):
            return "pitch";
        case(814):
            return "place";
        case(815):
            return "plain";
        case(816):
            return "plane";
        case(817):
            return "plant";
        case(818):
            return "plate";
        case(819):
            return "point";
        case(820):
            return "pound";
        case(821):
            return "power";
        case(822):
            return "press";
        case(823):
            return "price";
        case(824):
            return "pride";
        case(825):
            return "prime";
        case(826):
            return "print";
        case(827):
            return "prior";
        case(828):
            return "prize";
        case(829):
            return "proof";
        case(830):
            return "proud";
        case(831):
            return "prove";
        case(832):
            return "queen";
        case(833):
            return "quick";
        case(834):
            return "quiet";
        case(835):
            return "quite";
        case(836):
            return "radio";
        case(837):
            return "raise";
        case(838):
            return "range";
        case(839):
            return "rapid";
        case(840):
            return "ratio";
        case(841):
            return "reach";
        case(842):
            return "ready";
        case(843):
            return "refer";
        case(844):
            return "right";
        case(845):
            return "rival";
        case(846):
            return "river";
        case(847):
            return "robin";
        case(848):
            return "roger";
        case(849):
            return "roman";
        case(850):
            return "rough";
        case(851):
            return "round";
        case(852):
            return "route";
        case(853):
            return "royal";
        case(854):
            return "rural";
        case(855):
            return "scale";
        case(856):
            return "scene";
        case(857):
            return "scope";
        case(858):
            return "score";
        case(859):
            return "sense";
        case(860):
            return "serve";
        case(861):
            return "seven";
        case(862):
            return "shall";
        case(863):
            return "shape";
        case(864):
            return "share";
        case(865):
            return "sharp";
        case(866):
            return "sheet";
        case(867):
            return "shelf";
        case(868):
            return "shell";
        case(869):
            return "shift";
        case(870):
            return "shirt";
        case(871):
            return "shock";
        case(872):
            return "shoot";
        case(873):
            return "short";
        case(874):
            return "shown";
        case(875):
            return "sight";
        case(876):
            return "since";
        case(877):
            return "sixth";
        case(878):
            return "sixty";
        case(879):
            return "sized";
        case(880):
            return "skill";
        case(881):
            return "sleep";
        case(882):
            return "slide";
        case(883):
            return "small";
        case(884):
            return "smart";
        case(885):
            return "smile";
        case(886):
            return "smith";
        case(887):
            return "smoke";
        case(888):
            return "solid";
        case(889):
            return "solve";
        case(890):
            return "sorry";
        case(891):
            return "sound";
        case(892):
            return "south";
        case(893):
            return "space";
        case(894):
            return "spare";
        case(895):
            return "speak";
        case(896):
            return "speed";
        case(897):
            return "spend";
        case(898):
            return "spent";
        case(899):
            return "split";
        case(900):
            return "spoke";
        case(901):
            return "sport";
        case(902):
            return "staff";
        case(903):
            return "stage";
        case(904):
            return "stake";
        case(905):
            return "stand";
        case(906):
            return "start";
        case(907):
            return "state";
        case(908):
            return "steam";
        case(909):
            return "steel";
        case(910):
            return "stick";
        case(911):
            return "still";
        case(912):
            return "stock";
        case(913):
            return "stone";
        case(914):
            return "stood";
        case(915):
            return "store";
        case(916):
            return "storm";
        case(917):
            return "story";
        case(918):
            return "strip";
        case(919):
            return "stuck";
        case(920):
            return "study";
        case(921):
            return "stuff";
        case(922):
            return "style";
        case(923):
            return "sugar";
        case(924):
            return "suite";
        case(925):
            return "super";
        case(926):
            return "sweet";
        case(927):
            return "table";
        case(928):
            return "taken";
        case(929):
            return "taste";
        case(930):
            return "taxes";
        case(931):
            return "teach";
        case(932):
            return "teeth";
        case(933):
            return "terry";
        case(934):
            return "texas";
        case(935):
            return "thank";
        case(936):
            return "theft";
        case(937):
            return "their";
        case(938):
            return "theme";
        case(939):
            return "there";
        case(940):
            return "these";
        case(941):
            return "thick";
        case(942):
            return "thing";
        case(943):
            return "think";
        case(944):
            return "third";
        case(945):
            return "those";
        case(946):
            return "three";
        case(947):
            return "threw";
        case(948):
            return "throw";
        case(949):
            return "tight";
        case(950):
            return "times";
        case(951):
            return "tired";
        case(952):
            return "title";
        case(953):
            return "today";
        case(954):
            return "topic";
        case(955):
            return "total";
        case(956):
            return "touch";
        case(957):
            return "tough";
        case(958):
            return "tower";
        case(959):
            return "track";
        case(960):
            return "trade";
        case(961):
            return "train";
        case(962):
            return "treat";
        case(963):
            return "trend";
        case(964):
            return "trial";
        case(965):
            return "tried";
        case(966):
            return "tries";
        case(967):
            return "truck";
        case(968):
            return "truly";
        case(969):
            return "trust";
        case(970):
            return "truth";
        case(971):
            return "twice";
        case(972):
            return "under";
        case(973):
            return "undue";
        case(974):
            return "union";
        case(975):
            return "unity";
        case(976):
            return "until";
        case(977):
            return "upper";
        case(978):
            return "upset";
        case(979):
            return "urban";
        case(980):
            return "usage";
        case(981):
            return "usual";
        case(982):
            return "valid";
        case(983):
            return "value";
        case(984):
            return "video";
        case(985):
            return "virus";
        case(986):
            return "visit";
        case(987):
            return "vital";
        case(988):
            return "voice";
        case(989):
            return "waste";
        case(990):
            return "watch";
        case(991):
            return "water";
        case(992):
            return "wheel";
        case(993):
            return "where";
        case(994):
            return "which";
        case(995):
            return "while";
        case(996):
            return "white";
        case(997):
            return "whole";
        case(998):
            return "whose";
        case(999):
            return "woman";
        case(1000):
            return "women";
        case(1001):
            return "world";
        case(1002):
            return "worry";
        case(1003):
            return "worse";
        case(1004):
            return "worst";
        case(1005):
            return "worth";
        case(1006):
            return "would";
        case(1007):
            return "wound";
        case(1008):
            return "write";
        case(1009):
            return "wrong";
        case(1010):
            return "wrote";
        case(1011):
            return "yield";
        case(1012):
            return "young";
        case(1013):
            return "youth";
        case(1014):
            return "abroad";
        case(1015):
            return "accept";
        case(1016):
            return "access";
        case(1017):
            return "across";
        case(1018):
            return "acting";
        case(1019):
            return "action";
        case(1020):
            return "active";
        case(1021):
            return "actual";
        case(1022):
            return "advice";
        case(1023):
            return "advise";
        case(1024):
            return "affect";
        case(1025):
            return "afford";
        case(1026):
            return "afraid";
        case(1027):
            return "agency";
        case(1028):
            return "agenda";
        case(1029):
            return "almost";
        case(1030):
            return "always";
        case(1031):
            return "amount";
        case(1032):
            return "animal";
        case(1033):
            return "annual";
        case(1034):
            return "answer";
        case(1035):
            return "anyone";
        case(1036):
            return "anyway";
        case(1037):
            return "appeal";
        case(1038):
            return "appear";
        case(1039):
            return "around";
        case(1040):
            return "arrive";
        case(1041):
            return "artist";
        case(1042):
            return "aspect";
        case(1043):
            return "assess";
        case(1044):
            return "assist";
        case(1045):
            return "assume";
        case(1046):
            return "attack";
        case(1047):
            return "attend";
        case(1048):
            return "author";
        case(1049):
            return "avenue";
        case(1050):
            return "backed";
        case(1051):
            return "barely";
        case(1052):
            return "battle";
        case(1053):
            return "beauty";
        case(1054):
            return "became";
        case(1055):
            return "become";
        case(1056):
            return "before";
        case(1057):
            return "behalf";
        case(1058):
            return "behind";
        case(1059):
            return "belief";
        case(1060):
            return "belong";
        case(1061):
            return "berlin";
        case(1062):
            return "better";
        case(1063):
            return "beyond";
        case(1064):
            return "bishop";
        case(1065):
            return "border";
        case(1066):
            return "bottle";
        case(1067):
            return "bottom";
        case(1068):
            return "bought";
        case(1069):
            return "branch";
        case(1070):
            return "breath";
        case(1071):
            return "bridge";
        case(1072):
            return "bright";
        case(1073):
            return "broken";
        case(1074):
            return "budget";
        case(1075):
            return "burden";
        case(1076):
            return "bureau";
        case(1077):
            return "button";
        case(1078):
            return "camera";
        case(1079):
            return "cancer";
        case(1080):
            return "cannot";
        case(1081):
            return "carbon";
        case(1082):
            return "career";
        case(1083):
            return "castle";
        case(1084):
            return "casual";
        case(1085):
            return "caught";
        case(1086):
            return "center";
        case(1087):
            return "centre";
        case(1088):
            return "chance";
        case(1089):
            return "change";
        case(1090):
            return "charge";
        case(1091):
            return "choice";
        case(1092):
            return "choose";
        case(1093):
            return "chosen";
        case(1094):
            return "church";
        case(1095):
            return "circle";
        case(1096):
            return "client";
        case(1097):
            return "closed";
        case(1098):
            return "closer";
        case(1099):
            return "coffee";
        case(1100):
            return "column";
        case(1101):
            return "combat";
        case(1102):
            return "coming";
        case(1103):
            return "common";
        case(1104):
            return "comply";
        case(1105):
            return "copper";
        case(1106):
            return "corner";
        case(1107):
            return "costly";
        case(1108):
            return "county";
        case(1109):
            return "couple";
        case(1110):
            return "course";
        case(1111):
            return "covers";
        case(1112):
            return "create";
        case(1113):
            return "credit";
        case(1114):
            return "crisis";
        case(1115):
            return "custom";
        case(1116):
            return "damage";
        case(1117):
            return "danger";
        case(1118):
            return "dealer";
        case(1119):
            return "debate";
        case(1120):
            return "decade";
        case(1121):
            return "decide";
        case(1122):
            return "defeat";
        case(1123):
            return "defend";
        case(1124):
            return "define";
        case(1125):
            return "degree";
        case(1126):
            return "demand";
        case(1127):
            return "depend";
        case(1128):
            return "deputy";
        case(1129):
            return "desert";
        case(1130):
            return "design";
        case(1131):
            return "desire";
        case(1132):
            return "detail";
        case(1133):
            return "detect";
        case(1134):
            return "device";
        case(1135):
            return "differ";
        case(1136):
            return "dinner";
        case(1137):
            return "direct";
        case(1138):
            return "doctor";
        case(1139):
            return "dollar";
        case(1140):
            return "domain";
        case(1141):
            return "double";
        case(1142):
            return "driven";
        case(1143):
            return "driver";
        case(1144):
            return "during";
        case(1145):
            return "easily";
        case(1146):
            return "eating";
        case(1147):
            return "editor";
        case(1148):
            return "effect";
        case(1149):
            return "effort";
        case(1150):
            return "eighth";
        case(1151):
            return "either";
        case(1152):
            return "eleven";
        case(1153):
            return "emerge";
        case(1154):
            return "empire";
        case(1155):
            return "employ";
        case(1156):
            return "enable";
        case(1157):
            return "ending";
        case(1158):
            return "energy";
        case(1159):
            return "engage";
        case(1160):
            return "engine";
        case(1161):
            return "enough";
        case(1162):
            return "ensure";
        case(1163):
            return "entire";
        case(1164):
            return "entity";
        case(1165):
            return "equity";
        case(1166):
            return "escape";
        case(1167):
            return "estate";
        case(1168):
            return "ethnic";
        case(1169):
            return "exceed";
        case(1170):
            return "except";
        case(1171):
            return "excess";
        case(1172):
            return "expand";
        case(1173):
            return "expect";
        case(1174):
            return "expert";
        case(1175):
            return "export";
        case(1176):
            return "extend";
        case(1177):
            return "extent";
        case(1178):
            return "fabric";
        case(1179):
            return "facing";
        case(1180):
            return "factor";
        case(1181):
            return "failed";
        case(1182):
            return "fairly";
        case(1183):
            return "fallen";
        case(1184):
            return "family";
        case(1185):
            return "famous";
        case(1186):
            return "father";
        case(1187):
            return "fellow";
        case(1188):
            return "female";
        case(1189):
            return "figure";
        case(1190):
            return "filing";
        case(1191):
            return "finger";
        case(1192):
            return "finish";
        case(1193):
            return "fiscal";
        case(1194):
            return "flight";
        case(1195):
            return "flying";
        case(1196):
            return "follow";
        case(1197):
            return "forced";
        case(1198):
            return "forest";
        case(1199):
            return "forget";
        case(1200):
            return "formal";
        case(1201):
            return "format";
        case(1202):
            return "former";
        case(1203):
            return "foster";
        case(1204):
            return "fought";
        case(1205):
            return "fourth";
        case(1206):
            return "French";
        case(1207):
            return "friend";
        case(1208):
            return "future";
        case(1209):
            return "garden";
        case(1210):
            return "gather";
        case(1211):
            return "gender";
        case(1212):
            return "german";
        case(1213):
            return "global";
        case(1214):
            return "golden";
        case(1215):
            return "ground";
        case(1216):
            return "growth";
        case(1217):
            return "guilty";
        case(1218):
            return "handed";
        case(1219):
            return "handle";
        case(1220):
            return "happen";
        case(1221):
            return "hardly";
        case(1222):
            return "headed";
        case(1223):
            return "health";
        case(1224):
            return "height";
        case(1225):
            return "hidden";
        case(1226):
            return "holder";
        case(1227):
            return "honest";
        case(1228):
            return "impact";
        case(1229):
            return "import";
        case(1230):
            return "income";
        case(1231):
            return "indeed";
        case(1232):
            return "injury";
        case(1233):
            return "inside";
        case(1234):
            return "intend";
        case(1235):
            return "intent";
        case(1236):
            return "invest";
        case(1237):
            return "island";
        case(1238):
            return "itself";
        case(1239):
            return "jersey";
        case(1240):
            return "joseph";
        case(1241):
            return "junior";
        case(1242):
            return "killed";
        case(1243):
            return "labour";
        case(1244):
            return "latest";
        case(1245):
            return "latter";
        case(1246):
            return "launch";
        case(1247):
            return "lawyer";
        case(1248):
            return "leader";
        case(1249):
            return "league";
        case(1250):
            return "leaves";
        case(1251):
            return "legacy";
        case(1252):
            return "length";
        case(1253):
            return "lesson";
        case(1254):
            return "letter";
        case(1255):
            return "lights";
        case(1256):
            return "likely";
        case(1257):
            return "linked";
        case(1258):
            return "liquid";
        case(1259):
            return "listen";
        case(1260):
            return "little";
        case(1261):
            return "living";
        case(1262):
            return "losing";
        case(1263):
            return "lucent";
        case(1264):
            return "luxury";
        case(1265):
            return "mainly";
        case(1266):
            return "making";
        case(1267):
            return "manage";
        case(1268):
            return "manner";
        case(1269):
            return "manual";
        case(1270):
            return "margin";
        case(1271):
            return "marine";
        case(1272):
            return "marked";
        case(1273):
            return "market";
        case(1274):
            return "martin";
        case(1275):
            return "master";
        case(1276):
            return "matter";
        case(1277):
            return "mature";
        case(1278):
            return "medium";
        case(1279):
            return "member";
        case(1280):
            return "memory";
        case(1281):
            return "mental";
        case(1282):
            return "merely";
        case(1283):
            return "merger";
        case(1284):
            return "method";
        case(1285):
            return "middle";
        case(1286):
            return "miller";
        case(1287):
            return "mining";
        case(1288):
            return "minute";
        case(1289):
            return "mirror";
        case(1290):
            return "mobile";
        case(1291):
            return "modern";
        case(1292):
            return "modest";
        case(1293):
            return "module";
        case(1294):
            return "moment";
        case(1295):
            return "morris";
        case(1296):
            return "mostly";
        case(1297):
            return "mother";
        case(1298):
            return "motion";
        case(1299):
            return "moving";
        case(1300):
            return "murder";
        case(1301):
            return "museum";
        case(1302):
            return "mutual";
        case(1303):
            return "myself";
        case(1304):
            return "narrow";
        case(1305):
            return "nation";
        case(1306):
            return "native";
        case(1307):
            return "nature";
        case(1308):
            return "nearby";
        case(1309):
            return "nearly";
        case(1310):
            return "nights";
        case(1311):
            return "nobody";
        case(1312):
            return "normal";
        case(1313):
            return "notice";
        case(1314):
            return "notion";
        case(1315):
            return "number";
        case(1316):
            return "object";
        case(1317):
            return "obtain";
        case(1318):
            return "office";
        case(1319):
            return "offset";
        case(1320):
            return "online";
        case(1321):
            return "option";
        case(1322):
            return "orange";
        case(1323):
            return "origin";
        case(1324):
            return "output";
        case(1325):
            return "packed";
        case(1326):
            return "palace";
        case(1327):
            return "parent";
        case(1328):
            return "partly";
        case(1329):
            return "patent";
        case(1330):
            return "people";
        case(1331):
            return "period";
        case(1332):
            return "permit";
        case(1333):
            return "person";
        case(1334):
            return "phrase";
        case(1335):
            return "picked";
        case(1336):
            return "planet";
        case(1337):
            return "player";
        case(1338):
            return "please";
        case(1339):
            return "plenty";
        case(1340):
            return "pocket";
        case(1341):
            return "police";
        case(1342):
            return "policy";
        case(1343):
            return "prefer";
        case(1344):
            return "pretty";
        case(1345):
            return "prince";
        case(1346):
            return "prison";
        case(1347):
            return "profit";
        case(1348):
            return "proper";
        case(1349):
            return "proven";
        case(1350):
            return "public";
        case(1351):
            return "pursue";
        case(1352):
            return "raised";
        case(1353):
            return "random";
        case(1354):
            return "rarely";
        case(1355):
            return "rather";
        case(1356):
            return "rating";
        case(1357):
            return "reader";
        case(1358):
            return "really";
        case(1359):
            return "reason";
        case(1360):
            return "recall";
        case(1361):
            return "recent";
        case(1362):
            return "record";
        case(1363):
            return "reduce";
        case(1364):
            return "reform";
        case(1365):
            return "regard";
        case(1366):
            return "regime";
        case(1367):
            return "region";
        case(1368):
            return "relate";
        case(1369):
            return "relief";
        case(1370):
            return "remain";
        case(1371):
            return "remote";
        case(1372):
            return "remove";
        case(1373):
            return "repair";
        case(1374):
            return "repeat";
        case(1375):
            return "replay";
        case(1376):
            return "report";
        case(1377):
            return "rescue";
        case(1378):
            return "resort";
        case(1379):
            return "result";
        case(1380):
            return "retail";
        case(1381):
            return "retain";
        case(1382):
            return "return";
        case(1383):
            return "reveal";
        case(1384):
            return "review";
        case(1385):
            return "reward";
        case(1386):
            return "riding";
        case(1387):
            return "rising";
        case(1388):
            return "robust";
        case(1389):
            return "ruling";
        case(1390):
            return "safety";
        case(1391):
            return "salary";
        case(1392):
            return "sample";
        case(1393):
            return "saving";
        case(1394):
            return "saying";
        case(1395):
            return "scheme";
        case(1396):
            return "school";
        case(1397):
            return "screen";
        case(1398):
            return "search";
        case(1399):
            return "season";
        case(1400):
            return "second";
        case(1401):
            return "secret";
        case(1402):
            return "sector";
        case(1403):
            return "secure";
        case(1404):
            return "seeing";
        case(1405):
            return "select";
        case(1406):
            return "seller";
        case(1407):
            return "senior";
        case(1408):
            return "series";
        case(1409):
            return "server";
        case(1410):
            return "settle";
        case(1411):
            return "severe";
        case(1412):
            return "sexual";
        case(1413):
            return "should";
        case(1414):
            return "signal";
        case(1415):
            return "signed";
        case(1416):
            return "silent";
        case(1417):
            return "silver";
        case(1418):
            return "simple";
        case(1419):
            return "simply";
        case(1420):
            return "single";
        case(1421):
            return "sister";
        case(1422):
            return "slight";
        case(1423):
            return "smooth";
        case(1424):
            return "social";
        case(1425):
            return "solely";
        case(1426):
            return "sought";
        case(1427):
            return "source";
        case(1428):
            return "soviet";
        case(1429):
            return "speech";
        case(1430):
            return "spirit";
        case(1431):
            return "spoken";
        case(1432):
            return "spread";
        case(1433):
            return "spring";
        case(1434):
            return "square";
        case(1435):
            return "stable";
        case(1436):
            return "status";
        case(1437):
            return "steady";
        case(1438):
            return "stolen";
        case(1439):
            return "strain";
        case(1440):
            return "stream";
        case(1441):
            return "street";
        case(1442):
            return "stress";
        case(1443):
            return "strict";
        case(1444):
            return "strike";
        case(1445):
            return "string";
        case(1446):
            return "strong";
        case(1447):
            return "struck";
        case(1448):
            return "studio";
        case(1449):
            return "submit";
        case(1450):
            return "sudden";
        case(1451):
            return "suffer";
        case(1452):
            return "summer";
        case(1453):
            return "summit";
        case(1454):
            return "supply";
        case(1455):
            return "surely";
        case(1456):
            return "survey";
        case(1457):
            return "switch";
        case(1458):
            return "symbol";
        case(1459):
            return "system";
        case(1460):
            return "taking";
        case(1461):
            return "talent";
        case(1462):
            return "target";
        case(1463):
            return "taught";
        case(1464):
            return "tenant";
        case(1465):
            return "tender";
        case(1466):
            return "tennis";
        case(1467):
            return "thanks";
        case(1468):
            return "theory";
        case(1469):
            return "thirty";
        case(1470):
            return "though";
        case(1471):
            return "threat";
        case(1472):
            return "thrown";
        case(1473):
            return "ticket";
        case(1474):
            return "timely";
        case(1475):
            return "timing";
        case(1476):
            return "tissue";
        case(1477):
            return "toward";
        case(1478):
            return "travel";
        case(1479):
            return "treaty";
        case(1480):
            return "trying";
        case(1481):
            return "twelve";
        case(1482):
            return "twenty";
        case(1483):
            return "unable";
        case(1484):
            return "unique";
        case(1485):
            return "united";
        case(1486):
            return "unless";
        case(1487):
            return "unlike";
        case(1488):
            return "update";
        case(1489):
            return "useful";
        case(1490):
            return "valley";
        case(1491):
            return "varied";
        case(1492):
            return "vendor";
        case(1493):
            return "versus";
        case(1494):
            return "victim";
        case(1495):
            return "vision";
        case(1496):
            return "visual";
        case(1497):
            return "volume";
        case(1498):
            return "walker";
        case(1499):
            return "wealth";
        case(1500):
            return "weekly";
        case(1501):
            return "weight";
        case(1502):
            return "wholly";
        case(1503):
            return "window";
        case(1504):
            return "winner";
        case(1505):
            return "winter";
        case(1506):
            return "within";
        case(1507):
            return "wonder";
        case(1508):
            return "worker";
        case(1509):
            return "wright";
        case(1510):
            return "writer";
        case(1511):
            return "yellow";



        default:
            return "";
    }
}


/**
 * Main function to handle the game.
 * It will control the remaining time as well as the attempts remaining.
 * If the player loses (runs out of lives or time), it will call the function gameOver.
 * If the player wins (gets all the letters right), it will call the function victoryScreen.
 * As the player tries the different letters, it will draw them in the adequate position.
 * @param string -> word to be guessed
 * @return
 */
int gameFlow(char* string){

    int attempts = 0;
    int letters_to_guess = (int)strlen(string); //Get the number of letters in the string, to know how many ___ we will
    //have to draw

    xpm_image_t game,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
    xpm_image_t n0, n1, n2, n3, n4, n5, n6, n7, n8, n9;
    xpm_image_t nslash;

    uint8_t* sprite_game = NULL;
    //Getting the game ready
    if (strlen(string) == 4)
        sprite_game = xpm_load(gameimage_xpm, XPM_5_6_5,&game );
    else if (strlen(string) == 5)
        sprite_game = xpm_load(gameimage_5_xpm, XPM_5_6_5, &game);
    else if (strlen(string) == 6)
        sprite_game = xpm_load(gameimage_6_xpm, XPM_5_6_5, &game);
    else if (strlen(string) == 7)
        sprite_game = xpm_load(gameimage_7_xpm, XPM_5_6_5, &game);

    uint8_t* sprite_a = xpm_load(letterA_xpm, XPM_5_6_5, &a);
    uint8_t* sprite_b = xpm_load(letterB_xpm, XPM_5_6_5, &b);
    uint8_t* sprite_c = xpm_load(letterC_xpm, XPM_5_6_5, &c);
    uint8_t* sprite_d = xpm_load(letterD_xpm, XPM_5_6_5, &d);
    uint8_t* sprite_e = xpm_load(letterE_xpm, XPM_5_6_5, &e);
    uint8_t* sprite_f = xpm_load(letterF_xpm, XPM_5_6_5, &f);
    uint8_t* sprite_g = xpm_load(letterG_xpm, XPM_5_6_5, &g);
    uint8_t* sprite_h = xpm_load(letterH_xpm, XPM_5_6_5, &h);
    uint8_t* sprite_i = xpm_load(letterI_xpm, XPM_5_6_5, &i);
    uint8_t* sprite_j = xpm_load(letterJ_xpm, XPM_5_6_5, &j);
    uint8_t* sprite_k = xpm_load(letterK_xpm, XPM_5_6_5, &k);
    uint8_t* sprite_l = xpm_load(letterL_xpm, XPM_5_6_5, &l);
    uint8_t* sprite_m = xpm_load(letterM_xpm, XPM_5_6_5, &m);
    uint8_t* sprite_n = xpm_load(letterN_xpm, XPM_5_6_5, &n);
    uint8_t* sprite_o = xpm_load(letterO_xpm, XPM_5_6_5, &o);
    uint8_t* sprite_p = xpm_load(letterP_xpm, XPM_5_6_5, &p);
    uint8_t* sprite_q = xpm_load(letterQ_xpm, XPM_5_6_5, &q);
    uint8_t* sprite_r = xpm_load(letterR_xpm, XPM_5_6_5, &r);
    uint8_t* sprite_s = xpm_load(letterS_xpm, XPM_5_6_5, &s);
    uint8_t* sprite_t = xpm_load(letterT_xpm, XPM_5_6_5, &t);
    uint8_t* sprite_u = xpm_load(letterU_xpm, XPM_5_6_5, &u);
    uint8_t* sprite_v = xpm_load(letterV_xpm, XPM_5_6_5, &v);
    uint8_t* sprite_w = xpm_load(letterW_xpm, XPM_5_6_5, &w);
    uint8_t* sprite_x = xpm_load(letterX_xpm, XPM_5_6_5, &x);
    uint8_t* sprite_y = xpm_load(letterY_xpm, XPM_5_6_5, &y);
    uint8_t* sprite_z = xpm_load(letterZ_xpm, XPM_5_6_5, &z);

    uint8_t* sprite_0 = xpm_load(number0_xpm, XPM_5_6_5, &n0);
    uint8_t* sprite_1 = xpm_load(number1_xpm, XPM_5_6_5, &n1);
    uint8_t* sprite_2 = xpm_load(number2_xpm, XPM_5_6_5, &n2);
    uint8_t* sprite_3 = xpm_load(number3_xpm, XPM_5_6_5, &n3);
    uint8_t* sprite_4 = xpm_load(number4_xpm, XPM_5_6_5, &n4);
    uint8_t* sprite_5 = xpm_load(number5_xpm, XPM_5_6_5, &n5);
    uint8_t* sprite_6 = xpm_load(number6_xpm, XPM_5_6_5, &n6);
    uint8_t* sprite_7 = xpm_load(number7_xpm, XPM_5_6_5, &n7);
    uint8_t* sprite_8 = xpm_load(number8_xpm, XPM_5_6_5, &n8);
    uint8_t* sprite_9 = xpm_load(number9_xpm, XPM_5_6_5, &n9);

    uint8_t* sprite_slash = xpm_load(numberslash_xpm, XPM_5_6_5, &nslash);

    draw_xpm(game, 0,0);


    int time_digits = 60;
    //If we want to change the round time, all we have to do is change the value of the previous line:
    //For example, time_digits = 90
    //And then the next line, like: draw_xpm(n9, 5, 165); draw_xpm(n0, 75, 165);
    draw_xpm(n6, 5, 165); draw_xpm(n0, 75, 165);
    char time[3];


    uint8_t bit_no_timer = 0;
    timer_subscribe_int(&bit_no_timer);
    bit_no_timer = BIT(0);

    uint8_t bit_no_kbd = 1;
    keyboard_subscribe_int(&bit_no_kbd);
    bit_no_kbd = BIT(1);

    //Drawing the attempts number
    draw_xpm(n0, 540, 180);
    draw_xpm(nslash, 620, 180);
    draw_xpm(n5, 700, 180);

    int ipc_status; unsigned long rm; message msg;
    int timer_interrupt_count = 0;

    //Bool variables to control wether a certain letter was already guessed or not.
    //Optimized version would be having a map<char, int>, such as {a,0}, {b,1}, where 1 means guessed and 0 not
    bool guessed_a = false;    bool guessed_b = false;    bool guessed_c = false;
    bool guessed_d = false;    bool guessed_e = false;    bool guessed_f = false;
    bool guessed_g = false;    bool guessed_h = false;    bool guessed_i = false;
    bool guessed_j = false;    bool guessed_k = false;    bool guessed_l = false;
    bool guessed_m = false;    bool guessed_n = false;    bool guessed_o = false;
    bool guessed_p = false;    bool guessed_q = false;    bool guessed_r = false;
    bool guessed_s = false;    bool guessed_t = false;    bool guessed_u = false;
    bool guessed_v = false;    bool guessed_w = false;    bool guessed_x = false;
    bool guessed_y = false;    bool guessed_z = false;
    bool draw = false;
    int x_pos_wrong = 20;

    while (true) {
        // Get a request message.
        if ((rm = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with\n");
            continue;
        }
        if (is_ipc_notify(ipc_status)) { // received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: //hardware interrupt notification

                    if (msg.m_notify.interrupts & bit_no_timer){
                        timer_interrupt_count++;
                        if (timer_interrupt_count % 60 == 0){
                            time_digits--;

                            sprintf(time, "%d", time_digits);

                            //print first digit of time:
                            if (time_digits >= 10) {
                                switch (time[0]) {
                                    case ('0'):
                                        draw_xpm(n0, 5, 165);
                                        break;
                                    case ('1'):
                                        draw_xpm(n1, 5, 165);
                                        break;
                                    case ('2'):
                                        draw_xpm(n2, 5, 165);
                                        break;
                                    case ('3'):
                                        draw_xpm(n3, 5, 165);
                                        break;
                                    case ('4'):
                                        draw_xpm(n4, 5, 165);
                                        break;
                                    case ('5'):
                                        draw_xpm(n5, 5, 165);
                                        break;
                                    case ('6'):
                                        draw_xpm(n6, 5, 165);
                                        break;
                                    case ('7'):
                                        draw_xpm(n7, 5, 165);
                                        break;
                                    case ('8'):
                                        draw_xpm(n8, 5, 165);
                                        break;
                                    case ('9'):
                                        draw_xpm(n9, 5, 165);
                                        break;
                                }
                            }
                            else{
                                draw_xpm(n0, 5, 165);
                                switch (time[0]) {
                                    case ('0'):
                                        draw_xpm(n0, 75, 165);
                                        break;
                                    case ('1'):
                                        draw_xpm(n1, 75, 165);
                                        break;
                                    case ('2'):
                                        draw_xpm(n2, 75, 165);
                                        break;
                                    case ('3'):
                                        draw_xpm(n3, 75, 165);
                                        break;
                                    case ('4'):
                                        draw_xpm(n4, 75, 165);
                                        break;
                                    case ('5'):
                                        draw_xpm(n5, 75, 165);
                                        break;
                                    case ('6'):
                                        draw_xpm(n6, 75, 165);
                                        break;
                                    case ('7'):
                                        draw_xpm(n7, 75, 165);
                                        break;
                                    case ('8'):
                                        draw_xpm(n8, 75, 165);
                                        break;
                                    case ('9'):
                                        draw_xpm(n9, 75, 165);
                                        break;
                                }
                            }
                            if (time_digits == 0){

                                free(sprite_game);
                                free(sprite_slash);
                                free(sprite_a);
                                free(sprite_b);
                                free(sprite_c);
                                free(sprite_d);
                                free(sprite_e);
                                free(sprite_f);
                                free(sprite_g);
                                free(sprite_h);
                                free(sprite_i);
                                free(sprite_j);
                                free(sprite_k);
                                free(sprite_l);
                                free(sprite_m);
                                free(sprite_n);
                                free(sprite_o);
                                free(sprite_p);
                                free(sprite_q);
                                free(sprite_r);
                                free(sprite_s);
                                free(sprite_t);
                                free(sprite_u);
                                free(sprite_v);
                                free(sprite_w);
                                free(sprite_x);
                                free(sprite_y);
                                free(sprite_z);

                                free(sprite_0);
                                free(sprite_1);
                                free(sprite_2);
                                free(sprite_3);
                                free(sprite_4);
                                free(sprite_5);
                                free(sprite_6);
                                free(sprite_7);
                                free(sprite_8);
                                free(sprite_9);
                                timer_unsubscribe_int();
                                keyboard_unsubscribe_int();
                                if (gameOver(string) == 0){
                                    return 0;
                                }
                                else{
                                    printf("Error occurred during game over screen!\n");
                                    return 1;
                                }
                            }
                            if (time_digits >= 10) {
                                switch (time[1]) {
                                    case ('0'):
                                        draw_xpm(n0, 75, 165);
                                        break;
                                    case ('1'):
                                        draw_xpm(n1, 75, 165);
                                        break;
                                    case ('2'):
                                        draw_xpm(n2, 75, 165);
                                        break;
                                    case ('3'):
                                        draw_xpm(n3, 75, 165);
                                        break;
                                    case ('4'):
                                        draw_xpm(n4, 75, 165);
                                        break;
                                    case ('5'):
                                        draw_xpm(n5, 75, 165);
                                        break;
                                    case ('6'):
                                        draw_xpm(n6, 75, 165);
                                        break;
                                    case ('7'):
                                        draw_xpm(n7, 75, 165);
                                        break;
                                    case ('8'):
                                        draw_xpm(n8, 75, 165);
                                        break;
                                    case ('9'):
                                        draw_xpm(n9, 75, 165);
                                        break;
                                }
                            }

                        }
                    }


                    if (msg.m_notify.interrupts &
                        bit_no_kbd) {  //if it gets an interruption from the keyboard

                        kbc_ih();


                        draw = false;
                        if (scancode == A && !guessed_a){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'a'){
                                    if (i == 0) draw_xpm(a, x1, y1);
                                    else if (i==1) draw_xpm(a, x2, y1);
                                    else if (i==2) draw_xpm (a, x3, y1);
                                    else if (i==3) draw_xpm (a, x4, y1);
                                    else if (i==4) draw_xpm (a, x5, y1);
                                    else if (i==5) draw_xpm (a, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_a = true;
                            if (!draw) {
                                draw_xpm(a, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }

                        }
                        else if (scancode == B && !guessed_b){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'b'){
                                    if (i == 0) draw_xpm(b, x1, y1);
                                    else if (i==1) draw_xpm(b, x2, y1);
                                    else if (i==2) draw_xpm (b, x3, y1);
                                    else if (i==3) draw_xpm (b, x4, y1);
                                    else if (i==4) draw_xpm (b, x5, y1);
                                    else if (i==5) draw_xpm (b, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_b = true;
                            if (!draw) {
                                draw_xpm(b, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == C && !guessed_c){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'c'){
                                    if (i == 0) draw_xpm(c, x1, y1);
                                    else if (i==1) draw_xpm(c, x2, y1);
                                    else if (i==2) draw_xpm (c, x3, y1);
                                    else if (i==3) draw_xpm (c, x4, y1);
                                    else if (i==4) draw_xpm (c, x5, y1);
                                    else if (i==5) draw_xpm (c, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_c = true;
                            if (!draw) {
                                draw_xpm(c, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }

                        }
                        else if (scancode == D && !guessed_d){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'd'){
                                    if (i == 0) draw_xpm(d, x1, y1);
                                    else if (i==1) draw_xpm(d, x2, y1);
                                    else if (i==2) draw_xpm (d, x3, y1);
                                    else if (i==3) draw_xpm (d, x4, y1);
                                    else if (i==4) draw_xpm (d, x5, y1);
                                    else if (i==5) draw_xpm (d, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_d = true;
                            if (!draw) {
                                draw_xpm(d, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == E && !guessed_e){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'e'){
                                    if (i == 0) draw_xpm(e, x1, y1);
                                    else if (i==1) draw_xpm(e, x2, y1);
                                    else if (i==2) draw_xpm (e, x3, y1);
                                    else if (i==3) draw_xpm (e, x4, y1);
                                    else if (i==4) draw_xpm (e, x5, y1);
                                    else if (i==5) draw_xpm (e, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_e = true;
                            if (!draw) {
                                draw_xpm(e, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == F && !guessed_f){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'f'){
                                    if (i == 0) draw_xpm(f, x1, y1);
                                    else if (i==1) draw_xpm(f, x2, y1);
                                    else if (i==2) draw_xpm (f, x3, y1);
                                    else if (i==3) draw_xpm (f, x4, y1);
                                    else if (i==4) draw_xpm (f, x5, y1);
                                    else if (i==5) draw_xpm (f, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_f = true;
                            if (!draw) {
                                draw_xpm(f, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == G && !guessed_g){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'g'){
                                    if (i == 0) draw_xpm(g, x1, y1);
                                    else if (i==1) draw_xpm(g, x2, y1);
                                    else if (i==2) draw_xpm (g, x3, y1);
                                    else if (i==3) draw_xpm (g, x4, y1);
                                    else if (i==4) draw_xpm (g, x5, y1);
                                    else if (i==5) draw_xpm (g, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_g = true;
                            if (!draw) {
                                draw_xpm(g, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == H && !guessed_h){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'h'){
                                    if (i == 0) draw_xpm(h, x1, y1);
                                    else if (i==1) draw_xpm(h, x2, y1);
                                    else if (i==2) draw_xpm (h, x3, y1);
                                    else if (i==3) draw_xpm (h, x4, y1);
                                    else if (i==4) draw_xpm (h, x5, y1);
                                    else if (i==5) draw_xpm (h, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_h = true;
                            if (!draw) {
                                draw_xpm(h, x_pos_wrong,y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == I && !guessed_i){
                            for (size_t it = 0; it<strlen(string); it++ ){
                                if (string[it] == 'i'){
                                    if (it == 0) draw_xpm(i, x1, y1);
                                    else if (it==1) draw_xpm(i, x2, y1);
                                    else if (it==2) draw_xpm (i, x3, y1);
                                    else if (it==3) draw_xpm (i, x4, y1);
                                    else if (it==4) draw_xpm (i, x5, y1);
                                    else if (it==5) draw_xpm (i, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_i = true;
                            if (!draw) {
                                draw_xpm(i, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == J && !guessed_j){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'j'){
                                    if (i == 0) draw_xpm(j, x1, y1);
                                    else if (i==1) draw_xpm(j, x2, y1);
                                    else if (i==2) draw_xpm (j, x3, y1);
                                    else if (i==3) draw_xpm (j, x4, y1);
                                    else if (i==4) draw_xpm (j, x5, y1);
                                    else if (i==5) draw_xpm (j, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_j = true;
                            if (!draw) {
                                draw_xpm(j, x_pos_wrong,y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == K && !guessed_k){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'k'){
                                    if (i == 0) draw_xpm(k, x1, y1);
                                    else if (i==1) draw_xpm(k, x2, y1);
                                    else if (i==2) draw_xpm (k, x3, y1);
                                    else if (i==3) draw_xpm (k, x4, y1);
                                    else if (i==4) draw_xpm (k, x5, y1);
                                    else if (i==5) draw_xpm (k, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_k = true;
                            if (!draw) {
                                draw_xpm(k, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == L && !guessed_l){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'l'){
                                    if (i == 0) draw_xpm(l, x1, y1);
                                    else if (i==1) draw_xpm(l, x2, y1);
                                    else if (i==2) draw_xpm (l, x3, y1);
                                    else if (i==3) draw_xpm (l, x4, y1);
                                    else if (i==4) draw_xpm (l, x5, y1);
                                    else if (i==5) draw_xpm (l, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_l = true;
                            if (!draw) {
                                draw_xpm(l, x_pos_wrong,y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode== M && !guessed_m){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'm'){
                                    if (i == 0) draw_xpm(m, x1, y1);
                                    else if (i==1) draw_xpm(m, x2, y1);
                                    else if (i==2) draw_xpm (m, x3, y1);
                                    else if (i==3) draw_xpm (m, x4, y1);
                                    else if (i==4) draw_xpm (m, x5, y1);
                                    else if (i==5) draw_xpm (m, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_m = true;
                            if (!draw) {
                                draw_xpm(m, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == N && !guessed_n){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'n'){
                                    if (i == 0) draw_xpm(n, x1, y1);
                                    else if (i==1) draw_xpm(n, x2, y1);
                                    else if (i==2) draw_xpm (n, x3, y1);
                                    else if (i==3) draw_xpm (n, x4, y1);
                                    else if (i==4) draw_xpm (n, x5, y1);
                                    else if (i==5) draw_xpm (n, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_n = true;
                            if (!draw) {
                                draw_xpm(n, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == O && !guessed_o){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'o'){
                                    if (i == 0) draw_xpm(o, x1, y1);
                                    else if (i==1) draw_xpm(o, x2, y1);
                                    else if (i==2) draw_xpm (o, x3, y1);
                                    else if (i==3) draw_xpm (o, x4, y1);
                                    else if (i==4) draw_xpm (o, x5, y1);
                                    else if (i==5) draw_xpm (o, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_o = true;
                            if (!draw) {
                                draw_xpm(o, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == P && !guessed_p){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'p'){
                                    if (i == 0) draw_xpm(p, x1, y1);
                                    else if (i==1) draw_xpm(p, x2, y1);
                                    else if (i==2) draw_xpm (p, x3, y1);
                                    else if (i==3) draw_xpm (p, x4, y1);
                                    else if (i==4) draw_xpm (p, x5, y1);
                                    else if (i==5) draw_xpm (p, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_p = true;
                            if (!draw) {
                                draw_xpm(p, x_pos_wrong,y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode== Q && !guessed_q){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'q'){
                                    if (i == 0) draw_xpm(q, x1, y1);
                                    else if (i==1) draw_xpm(q, x2, y1);
                                    else if (i==2) draw_xpm (q, x3, y1);
                                    else if (i==3) draw_xpm (q, x4, y1);
                                    else if (i==4) draw_xpm (q, x5, y1);
                                    else if (i==5) draw_xpm (q, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_q = true;
                            if (!draw) {
                                draw_xpm(q, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == R && !guessed_r){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'r'){
                                    if (i == 0) draw_xpm(r, x1, y1);
                                    else if (i==1) draw_xpm(r, x2, y1);
                                    else if (i==2) draw_xpm (r, x3, y1);
                                    else if (i==3) draw_xpm (r, x4, y1);
                                    else if (i==4) draw_xpm (r, x5, y1);
                                    else if (i==5) draw_xpm (r, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_r = true;
                            if (!draw) {
                                draw_xpm(r, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == S && !guessed_s){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 's'){
                                    if (i == 0) draw_xpm(s, x1, y1);
                                    else if (i==1) draw_xpm(s, x2, y1);
                                    else if (i==2) draw_xpm (s, x3, y1);
                                    else if (i==3) draw_xpm (s, x4, y1);
                                    else if (i==4) draw_xpm (s, x5, y1);
                                    else if (i==5) draw_xpm (s, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_s = true;
                            if (!draw) {
                                draw_xpm(s, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == T && !guessed_t){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 't'){
                                    if (i == 0) draw_xpm(t, x1, y1);
                                    else if (i==1) draw_xpm(t, x2, y1);
                                    else if (i==2) draw_xpm (t, x3, y1);
                                    else if (i==3) draw_xpm (t, x4, y1);
                                    else if (i==4) draw_xpm (t, x5, y1);
                                    else if (i==5) draw_xpm (t, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_t = true;
                            if (!draw) {
                                draw_xpm(t, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode== U && !guessed_u){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'u'){
                                    if (i == 0) draw_xpm(u, x1, y1);
                                    else if (i==1) draw_xpm(u, x2, y1);
                                    else if (i==2) draw_xpm (u, x3, y1);
                                    else if (i==3) draw_xpm (u, x4, y1);
                                    else if (i==4) draw_xpm (u, x5, y1);
                                    else if (i==5) draw_xpm (u, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_u = true;
                            if (!draw) {
                                draw_xpm(u, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == V && !guessed_v){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'v'){
                                    if (i == 0) draw_xpm(v, x1, y1);
                                    else if (i==1) draw_xpm(v, x2, y1);
                                    else if (i==2) draw_xpm (v, x3, y1);
                                    else if (i==3) draw_xpm (v, x4, y1);
                                    else if (i==4) draw_xpm (v, x5, y1);
                                    else if (i==5) draw_xpm (v, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_v = true;
                            if (!draw) {
                                draw_xpm(v, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == W && !guessed_w){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'w'){
                                    if (i == 0) draw_xpm(w, x1, y1);
                                    else if (i==1) draw_xpm(w, x2, y1);
                                    else if (i==2) draw_xpm (w, x3, y1);
                                    else if (i==3) draw_xpm (w, x4, y1);
                                    else if (i==4) draw_xpm (w, x5, y1);
                                    else if (i==5) draw_xpm (w, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_w = true;
                            if (!draw) {
                                draw_xpm(w, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == X && !guessed_x){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'x'){
                                    if (i == 0) draw_xpm(x, x1, y1);
                                    else if (i==1) draw_xpm(x, x2, y1);
                                    else if (i==2) draw_xpm (x, x3, y1);
                                    else if (i==3) draw_xpm (x, x4, y1);
                                    else if (i==4) draw_xpm (x, x5, y1);
                                    else if (i==5) draw_xpm (x, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_x = true;
                            if (!draw) {
                                draw_xpm(x, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode== Y && !guessed_y){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'y'){
                                    if (i == 0) draw_xpm(y, x1, y1);
                                    else if (i==1) draw_xpm(y, x2, y1);
                                    else if (i==2) draw_xpm (y, x3, y1);
                                    else if (i==3) draw_xpm (y, x4, y1);
                                    else if (i==4) draw_xpm (y, x5, y1);
                                    else if (i==5) draw_xpm (y, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_y = true;
                            if (!draw) {
                                draw_xpm(y, x_pos_wrong, y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        else if (scancode == Z && !guessed_z){
                            for (size_t i = 0; i<strlen(string); i++ ){
                                if (string[i] == 'z'){
                                    if (i == 0) draw_xpm(z, x1, y1);
                                    else if (i==1) draw_xpm(z, x2, y1);
                                    else if (i==2) draw_xpm (z, x3, y1);
                                    else if (i==3) draw_xpm (z, x4, y1);
                                    else if (i==4) draw_xpm (z, x5, y1);
                                    else if (i==5) draw_xpm (z, x6, y1);
                                    draw = true;
                                    letters_to_guess--;
                                }
                            }
                            guessed_a = true;
                            if (!draw) {
                                draw_xpm(z, x_pos_wrong,y_pos_wrong);
                                x_pos_wrong+=115;
                                attempts++;
                            }
                        }
                        if (letters_to_guess == 0){ //Player got the word correctly
                            free(sprite_game);
                            free(sprite_slash);
                            free(sprite_a);
                            free(sprite_b);
                            free(sprite_c);
                            free(sprite_d);
                            free(sprite_e);
                            free(sprite_f);
                            free(sprite_g);
                            free(sprite_h);
                            free(sprite_i);
                            free(sprite_j);
                            free(sprite_k);
                            free(sprite_l);
                            free(sprite_m);
                            free(sprite_n);
                            free(sprite_o);
                            free(sprite_p);
                            free(sprite_q);
                            free(sprite_r);
                            free(sprite_s);
                            free(sprite_t);
                            free(sprite_u);
                            free(sprite_v);
                            free(sprite_w);
                            free(sprite_x);
                            free(sprite_y);
                            free(sprite_z);

                            free(sprite_0);
                            free(sprite_1);
                            free(sprite_2);
                            free(sprite_3);
                            free(sprite_4);
                            free(sprite_5);
                            free(sprite_6);
                            free(sprite_7);
                            free(sprite_8);
                            free(sprite_9);
                            timer_unsubscribe_int();
                            keyboard_unsubscribe_int();

                            if (victoryScreen() == 0) {
                                return 0;
                            }
                            else {
                                return 1;
                                printf("Something wrong in victory screen!\n");
                            }
                        }


                        //Drawing the attempts
                        switch(attempts){
                            case(1):
                                draw_xpm(n1,540, 180);
                                break;
                            case(2):
                                draw_xpm(n2,540, 180);
                                break;
                            case(3):
                                draw_xpm(n3,540, 180);
                                break;
                            case(4):
                                draw_xpm(n4,540, 180);
                                break;
                            case(5):
                                draw_xpm(n5,540, 180);

                                free(sprite_game);
                                free(sprite_slash);
                                free(sprite_a);
                                free(sprite_b);
                                free(sprite_c);
                                free(sprite_d);
                                free(sprite_e);
                                free(sprite_f);
                                free(sprite_g);
                                free(sprite_h);
                                free(sprite_i);
                                free(sprite_j);
                                free(sprite_k);
                                free(sprite_l);
                                free(sprite_m);
                                free(sprite_n);
                                free(sprite_o);
                                free(sprite_p);
                                free(sprite_q);
                                free(sprite_r);
                                free(sprite_s);
                                free(sprite_t);
                                free(sprite_u);
                                free(sprite_v);
                                free(sprite_w);
                                free(sprite_x);
                                free(sprite_y);
                                free(sprite_z);

                                free(sprite_0);
                                free(sprite_1);
                                free(sprite_2);
                                free(sprite_3);
                                free(sprite_4);
                                free(sprite_5);
                                free(sprite_6);
                                free(sprite_7);
                                free(sprite_8);
                                free(sprite_9);
                                timer_unsubscribe_int();
                                keyboard_unsubscribe_int();

                                if (gameOver(string) == 0){
                                    return 0;
                                }
                                else{
                                    printf("Error occurred during game over screen!\n");
                                    return 1;
                                }
                                break;
                            default:
                                break;
                        }


                    }

                    break;
                default:
                    break; // no other notifications expected: do nothing
            }
        } else { // received a standard message, not a notification
            // no standard messages expected: do nothing
        }

    }


    return 0;

}


/**
 * If the player ran out of times or lives, they will be presented with this screen.
 * It will present the score they had before they lost (unless it was 0), and it will show what was the right word.
 * It will also present the options to restart the game or quit
 * @param string -> word that was supposed to have been guessed
 * @return 0 if everything went OK, 1 otherwise
 */
int gameOver(char* string){
    xpm_image_t game_over;
    uint8_t* sprite;

    if (number_of_wins !=0)
        sprite = xpm_load(gameover_xpm, XPM_5_6_5, &game_over);
    else sprite = xpm_load (gameover_no0_xpm, XPM_5_6_5, &game_over);

    draw_xpm(game_over, 0,0);

    xpm_image_t a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;
    uint8_t* sprite_a = xpm_load(letterA_xpm, XPM_5_6_5, &a);
    uint8_t* sprite_b = xpm_load(letterB_xpm, XPM_5_6_5, &b);
    uint8_t* sprite_c = xpm_load(letterC_xpm, XPM_5_6_5, &c);
    uint8_t* sprite_d = xpm_load(letterD_xpm, XPM_5_6_5, &d);
    uint8_t* sprite_e = xpm_load(letterE_xpm, XPM_5_6_5, &e);
    uint8_t* sprite_f = xpm_load(letterF_xpm, XPM_5_6_5, &f);
    uint8_t* sprite_g = xpm_load(letterG_xpm, XPM_5_6_5, &g);
    uint8_t* sprite_h = xpm_load(letterH_xpm, XPM_5_6_5, &h);
    uint8_t* sprite_i = xpm_load(letterI_xpm, XPM_5_6_5, &i);
    uint8_t* sprite_j = xpm_load(letterJ_xpm, XPM_5_6_5, &j);
    uint8_t* sprite_k = xpm_load(letterK_xpm, XPM_5_6_5, &k);
    uint8_t* sprite_l = xpm_load(letterL_xpm, XPM_5_6_5, &l);
    uint8_t* sprite_m = xpm_load(letterM_xpm, XPM_5_6_5, &m);
    uint8_t* sprite_n = xpm_load(letterN_xpm, XPM_5_6_5, &n);
    uint8_t* sprite_o = xpm_load(letterO_xpm, XPM_5_6_5, &o);
    uint8_t* sprite_p = xpm_load(letterP_xpm, XPM_5_6_5, &p);
    uint8_t* sprite_q = xpm_load(letterQ_xpm, XPM_5_6_5, &q);
    uint8_t* sprite_r = xpm_load(letterR_xpm, XPM_5_6_5, &r);
    uint8_t* sprite_s = xpm_load(letterS_xpm, XPM_5_6_5, &s);
    uint8_t* sprite_t = xpm_load(letterT_xpm, XPM_5_6_5, &t);
    uint8_t* sprite_u = xpm_load(letterU_xpm, XPM_5_6_5, &u);
    uint8_t* sprite_v = xpm_load(letterV_xpm, XPM_5_6_5, &v);
    uint8_t* sprite_w = xpm_load(letterW_xpm, XPM_5_6_5, &w);
    uint8_t* sprite_x = xpm_load(letterX_xpm, XPM_5_6_5, &x);
    uint8_t* sprite_y = xpm_load(letterY_xpm, XPM_5_6_5, &y);
    uint8_t* sprite_z = xpm_load(letterZ_xpm, XPM_5_6_5, &z);


    xpm_image_t n0, n1, n2, n3, n4, n5, n6, n7, n8, n9;
    uint8_t* sprite_0 = xpm_load(number0_xpm, XPM_5_6_5, &n0);
    uint8_t* sprite_1 = xpm_load(number1_xpm, XPM_5_6_5, &n1);
    uint8_t* sprite_2 = xpm_load(number2_xpm, XPM_5_6_5, &n2);
    uint8_t* sprite_3 = xpm_load(number3_xpm, XPM_5_6_5, &n3);
    uint8_t* sprite_4 = xpm_load(number4_xpm, XPM_5_6_5, &n4);
    uint8_t* sprite_5 = xpm_load(number5_xpm, XPM_5_6_5, &n5);
    uint8_t* sprite_6 = xpm_load(number6_xpm, XPM_5_6_5, &n6);
    uint8_t* sprite_7 = xpm_load(number7_xpm, XPM_5_6_5, &n7);
    uint8_t* sprite_8 = xpm_load(number8_xpm, XPM_5_6_5, &n8);
    uint8_t* sprite_9 = xpm_load(number9_xpm, XPM_5_6_5, &n9);

    if (number_of_wins != 0) {
        switch (number_of_wins) {

            case (1):
                draw_xpm(n1, 460, 380);
                break;
            case (2):
                draw_xpm(n2, 460, 380);
                break;
            case (3):
                draw_xpm(n3, 460, 380);
                break;
            case (4):
                draw_xpm(n4, 460, 380);
                break;
            case (5):
                draw_xpm(n5, 460, 380);
                break;
            case (6):
                draw_xpm(n6, 460, 380);
                break;
            case (7):
                draw_xpm(n7, 460, 380);
                break;
            case (8):
                draw_xpm(n8, 460, 380);
                break;
            case (9):
                draw_xpm(n9, 460, 380);
                break;
                //TODO CASE FOR 10+ wins
        }
    }





    if (number_of_wins > current_highscore){
        //MUST UPDATE HIGHSCORE FILE!
        FILE* fptr = fopen(HIGHSCORE_PATH, "w");
        fprintf(fptr, "%d", number_of_wins);




        fclose(fptr);



    }
    number_of_wins = 0; //To allow players to restart game. Must reset the score since they lost





    //Give a bit of suspense before showing what the word was :)
    sleep(2);

    //PROCESS TO DRAW THE WORD
    int x_pos = 0;

    //Determining where to first start drawing the first letter. Aesthetic purposes only
    if (strlen(string) == 4) x_pos = 240; else if (strlen(string) == 5) x_pos = 215; else if (strlen(string) == 6) x_pos = 180;
    int y_pos = 300;

    //Parsing the word to draw the respective letter
    for (size_t it = 0; it < strlen(string); it++){
        if (string[it] == 'a') draw_xpm(a, x_pos, y_pos);
        else if (string[it] == 'b') draw_xpm(b, x_pos, y_pos);
        else if (string[it]== 'c') draw_xpm(c, x_pos, y_pos);
        else if (string[it] == 'd') draw_xpm(d, x_pos, y_pos);
        else if (string[it] == 'e') draw_xpm(e, x_pos, y_pos);
        else if (string[it] == 'f') draw_xpm(f, x_pos, y_pos);
        else if (string[it] == 'g') draw_xpm(g, x_pos, y_pos);
        else if (string[it] == 'h') draw_xpm(h, x_pos, y_pos);
        else if (string[it] == 'i') draw_xpm(i, x_pos, y_pos);
        else if (string[it] == 'j') draw_xpm(j, x_pos, y_pos);
        else if (string[it] == 'k') draw_xpm(k, x_pos, y_pos);
        else if (string[it] == 'l') draw_xpm(l, x_pos, y_pos);
        else if (string[it] == 'm') draw_xpm(m, x_pos, y_pos);
        else if (string[it] == 'n') draw_xpm(n, x_pos, y_pos);
        else if (string[it] == 'o') draw_xpm(o, x_pos, y_pos);
        else if (string[it] == 'p') draw_xpm(p, x_pos, y_pos);
        else if (string[it] == 'q') draw_xpm(q, x_pos, y_pos);
        else if (string[it] == 'r') draw_xpm(r, x_pos, y_pos);
        else if (string[it] == 's') draw_xpm(s, x_pos, y_pos);
        else if (string[it] == 't') draw_xpm(t, x_pos, y_pos);
        else if (string[it] == 'u') draw_xpm(u, x_pos, y_pos);
        else if (string[it] == 'v') draw_xpm(v, x_pos, y_pos);
        else if (string[it] == 'w') draw_xpm(w, x_pos, y_pos);
        else if (string[it] == 'x') draw_xpm(x, x_pos, y_pos);
        else if (string[it] == 'y') draw_xpm(y, x_pos, y_pos);
        else if (string[it] == 'z') draw_xpm(z, x_pos, y_pos);
        x_pos+=70; //Advancing the x_coordinate to draw the next letter


    }





    if (write_cmd(enable_date_report)!= 0){
        printf("Error enabling mouse!");
        return 1;
    }

    uint8_t bit_no_kbd = 1;
    keyboard_subscribe_int(&bit_no_kbd);
    bit_no_kbd= BIT(1);

    uint8_t bit_no_mouse = 2;
    mouse_subscribe_int(&bit_no_mouse);
    bit_no_mouse = BIT(2);


    int ipc_status; unsigned long rm; message msg;


    while (scancode != 0x81) { //if user doesn't press mouse within 5 seconds
        // Get a request message.
        if ((rm = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with\n");
            continue;
        }
        if (is_ipc_notify(ipc_status)) { // received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: //hardware interrupt notification

                    if (msg.m_notify.interrupts & bit_no_kbd) {
                        kbc_ih();
                    }

                    if (msg.m_notify.interrupts & bit_no_mouse) {
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
                                //mouse_print_packet(&pp);
                                counter_mouse = 0;
                                if (pp.lb) {
                                    free(sprite);
                                    free(sprite);
                                    free(sprite_a);
                                    free(sprite_b);
                                    free(sprite_c);
                                    free(sprite_d);
                                    free(sprite_e);
                                    free(sprite_f);
                                    free(sprite_g);
                                    free(sprite_h);
                                    free(sprite_i);
                                    free(sprite_j);
                                    free(sprite_k);
                                    free(sprite_l);
                                    free(sprite_m);
                                    free(sprite_n);
                                    free(sprite_o);
                                    free(sprite_p);
                                    free(sprite_q);
                                    free(sprite_r);
                                    free(sprite_s);
                                    free(sprite_t);
                                    free(sprite_u);
                                    free(sprite_v);
                                    free(sprite_w);
                                    free(sprite_x);
                                    free(sprite_y);
                                    free(sprite_z);
                                    free(sprite_0);
                                    free(sprite_1);
                                    free(sprite_2);
                                    free(sprite_3);
                                    free(sprite_4);
                                    free(sprite_5);
                                    free(sprite_6);
                                    free(sprite_7);
                                    free(sprite_8);
                                    free(sprite_9);


                                    mouse_unsubscribe_int();
                                    timer_unsubscribe_int();

                                    if (write_cmd(disable_data_report) != 0) {
                                        printf("Error disabling mouse data reporting!");
                                        return 1;

                                    }
                                    gameFlow(getRandomWord());
                                }
                            }

                        }


                        break;
                        default:
                            break; // no other notifications expected: do nothing
                    }
            }
        } else { // received a standard message, not a notification
            // no standard messages expected: do nothing
        }

    }











    vg_exit();
    free(sprite);
    free(sprite_a);
    free(sprite_b);
    free(sprite_c);
    free(sprite_d);
    free(sprite_e);
    free(sprite_f);
    free(sprite_g);
    free(sprite_h);
    free(sprite_i);
    free(sprite_j);
    free(sprite_k);
    free(sprite_l);
    free(sprite_m);
    free(sprite_n);
    free(sprite_o);
    free(sprite_p);
    free(sprite_q);
    free(sprite_r);
    free(sprite_s);
    free(sprite_t);
    free(sprite_u);
    free(sprite_v);
    free(sprite_w);
    free(sprite_x);
    free(sprite_y);
    free(sprite_z);
    free(sprite_0);
    free(sprite_1);
    free(sprite_2);
    free(sprite_3);
    free(sprite_4);
    free(sprite_5);
    free(sprite_6);
    free(sprite_7);
    free(sprite_8);
    free(sprite_9);
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();

    if (write_cmd(disable_data_report) != 0) {
        printf("Error disabling mouse data reporting!");
        return 1;

    }




    exit(0);

}


/**
 * If the player got all the letters right, they will be presented with this screen.
 * It will display the current score, and will let know the user if they beat the highscore.
 * It also presents the options to either keep playing or quit.
 * @return
 */
int victoryScreen(){
    xpm_image_t victory, hs_message;

    uint8_t* sprite = xpm_load(victoryscreenv2_xpm, XPM_5_6_5, &victory);
    uint8_t* sprite_message = xpm_load(highscore_message_xpm, XPM_5_6_5, &hs_message);

    draw_xpm(victory, 0,0);
    number_of_wins++;

    if (number_of_wins > current_highscore){
        draw_xpm(hs_message, 0, 325); //Write on the screen "You have a new highscore!"
    }

    xpm_image_t n0, n1, n2, n3, n4, n5, n6, n7, n8, n9;
    uint8_t* sprite_0 = xpm_load(number0_xpm, XPM_5_6_5, &n0);
    uint8_t* sprite_1 = xpm_load(number1_xpm, XPM_5_6_5, &n1);
    uint8_t* sprite_2 = xpm_load(number2_xpm, XPM_5_6_5, &n2);
    uint8_t* sprite_3 = xpm_load(number3_xpm, XPM_5_6_5, &n3);
    uint8_t* sprite_4 = xpm_load(number4_xpm, XPM_5_6_5, &n4);
    uint8_t* sprite_5 = xpm_load(number5_xpm, XPM_5_6_5, &n5);
    uint8_t* sprite_6 = xpm_load(number6_xpm, XPM_5_6_5, &n6);
    uint8_t* sprite_7 = xpm_load(number7_xpm, XPM_5_6_5, &n7);
    uint8_t* sprite_8 = xpm_load(number8_xpm, XPM_5_6_5, &n8);
    uint8_t* sprite_9 = xpm_load(number9_xpm, XPM_5_6_5, &n9);

    switch(number_of_wins){
        case(1):
            draw_xpm(n1, 545, 250);
            break;
        case(2):
            draw_xpm(n2, 545, 250);
            break;
        case(3):
            draw_xpm(n3, 545, 250);
            break;
        case(4):
            draw_xpm(n4, 545, 250);
            break;
        case(5):
            draw_xpm(n5, 545, 250);
            break;
        case(6):
            draw_xpm(n6, 545, 250);
            break;
        case(7):
            draw_xpm(n7, 545, 250);
            break;
        case(8):
            draw_xpm(n8, 545, 250);
            break;
        case(9):
            draw_xpm(n9, 545, 250);
            break;
    }




    if (write_cmd(enable_date_report)!= 0){
        printf("Error enabling mouse!");
        return 1;
    }

    uint8_t bit_no_kbd = 1;
    keyboard_subscribe_int(&bit_no_kbd);
    bit_no_kbd= BIT(1);

    uint8_t bit_no_mouse = 2;
    mouse_subscribe_int(&bit_no_mouse);
    bit_no_mouse = BIT(2);

    int ipc_status; unsigned long rm; message msg;


    while (scancode != 0x81) { //if user doesn't press mouse within 5 seconds
        // Get a request message.
        if ((rm = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with\n");
            continue;
        }
        if (is_ipc_notify(ipc_status)) { // received notification
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: //hardware interrupt notification

                    if (msg.m_notify.interrupts & bit_no_kbd) {
                        kbc_ih();
                    }

                    if (msg.m_notify.interrupts & bit_no_mouse) {
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
                                //mouse_print_packet(&pp);
                                counter_mouse = 0;
                                if (pp.lb) {
                                    free(sprite);
                                    free(sprite_0);
                                    free(sprite_1);
                                    free(sprite_2);
                                    free(sprite_3);
                                    free(sprite_4);
                                    free(sprite_5);
                                    free(sprite_6);
                                    free(sprite_7);
                                    free(sprite_8);
                                    free(sprite_9);
                                    free(sprite_message);

                                    mouse_unsubscribe_int();
                                    timer_unsubscribe_int();

                                    if (write_cmd(disable_data_report) != 0) {
                                        printf("Error disabling mouse data reporting!");
                                        return 1;

                                    }
                                    gameFlow(getRandomWord());
                                }
                            }

                        }


                        break;
                        default:
                            break; // no other notifications expected: do nothing
                    }
            }
        } else { // received a standard message, not a notification
                // no standard messages expected: do nothing
            }

    }






    vg_exit();
    free(sprite);
    free(sprite_0);
    free(sprite_1);
    free(sprite_2);
    free(sprite_3);
    free(sprite_4);
    free(sprite_5);
    free(sprite_6);
    free(sprite_7);
    free(sprite_8);
    free(sprite_9);
    free(sprite_message);
    mouse_unsubscribe_int();
    keyboard_unsubscribe_int();

    if (write_cmd(disable_data_report) != 0) {
        printf("Error disabling mouse data reporting!");
        return 1;

    }


    //UPDATE FILE WITH THE HIGHSCORE IF THE CURRENT SCORE IS GREATER THAN THE PREVIOUS ONE
    if (number_of_wins > current_highscore){
        //Opening file
        FILE* fptr = fopen(HIGHSCORE_PATH, "w");
        fprintf(fptr, "%d", number_of_wins);
        fclose(fptr);
    }
    exit(0);
}
