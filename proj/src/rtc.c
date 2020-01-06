//
// Created by Fabio on 12/12/2019.
//
#include <lcom/lcf.h>
#include "rtc.h"
int hook_id_rtc = 3;
uint8_t realDate[3]; //DAY, MONTH, YEAR
uint8_t realTime[3]; //HOURS MINUTES SECONDS

//TODO DISABLE NOT WORKING
//TODO BCD TO BINARY TO DECIMAL CONVERTER
int (rtc_subscribe_int)(uint8_t *bit_no) {

    uint8_t tmp;

    hook_id_rtc=*bit_no;
    //this is done so that the programmer can choose which bit to use, instead of having to stick to what is given, so that he can prevent compatibility issues with other libraries
    if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &hook_id_rtc) !=0){
        printf("Error subscribing interruption!");
        return 1;
    }

    if (sys_outb(ADDR_REG, REGISTER_B)){
        printf("Error accessing Addr Reg\n");
        return 1;
    }

    if (util_sys_inb(DATA_REG, &tmp)){
        printf("Error getting data from DATA_REG\n");
        return 1;
    }

    tmp = tmp | BIT(4); //To enable update interrupts

    if (sys_outb(ADDR_REG, REGISTER_B)){
        printf("Error writing register address in RTC_ADDR_REG\n");
        return 1;
    }
    if (sys_outb(DATA_REG, tmp)){
        printf("Error writing word to the register\n");
        return 1;
    }


    return 0;
}

int (rtc_unsubscribe_int)(){
    uint8_t tmp;
    if(sys_irqrmpolicy(&hook_id_rtc)!=0){
        printf("Error unsubscribing interruption!");
        return 1;
    }

    //Must get the word from the 0x71 register, clear bit(4) and write the word back to the register

    if (sys_outb(ADDR_REG, REGISTER_B)){
        printf("Error accessing ADDR_REG");
        return 1;
    }

    if (util_sys_inb(DATA_REG, &tmp)){
        printf("Error getting the word from the DATA_REG");
        return 1;
    }

    uint8_t mask = BIT(7) | BIT(6) | BIT(5) | BIT(3) | BIT(2) | BIT(1) | BIT(0);
    mask = mask & tmp; //mask extracted all the bits from the tmp word and bit(4) will be clear

    if (sys_outb(ADDR_REG, REGISTER_B)){
        printf("Error accessing ADDR_REG");
        return 1;
    }

    if (sys_outb(DATA_REG, &mask)){
        printf("Error writing word to the DATA_REG");
    }



    return 0;
}

int (rtc_disable)(){
    if (sys_irqdisable(&hook_id_rtc)) {
        printf("Error disabling rtc interrupts\n");
        return 1;
    }

    return 0;

}

int (rtc_enable)(){
    if (sys_irqenable(&hook_id_rtc)) {
        printf("Error enabling rtc interrupts\n");
        return 1;
    }

    return 0;
}



uint8_t rtc_get_day(){
    uint8_t regA = 0;
    uint8_t day;
    do {
        //rtc_disable();
        sys_outb(ADDR_REG, REGISTER_A);
        util_sys_inb(DATA_REG, &regA);
        //rtc_enable();
    } while ( regA & BIT(7)); //WHILE update is in progress

    if (sys_outb(ADDR_REG, DAY_REG)){
        printf("Error accessing ADDR_REG");
        return 1;
    }
    if (util_sys_inb(DATA_REG, &day)){
        printf("Error getting day");
        return 1;
    }

    realDate[0] = day;
    return day;

}

uint8_t rtc_get_month(){
    uint8_t regA = 0;
    uint8_t month;
    do {
        //rtc_disable();
        sys_outb(ADDR_REG, REGISTER_A);
        util_sys_inb(DATA_REG, &regA);
        //rtc_enable();
    } while ( regA & BIT(7)); //WHILE update is in progress

    if (sys_outb(ADDR_REG, MONTH_REG)){
        printf("Error accessing ADDR_REG");
        return 1;
    }
    if (util_sys_inb(DATA_REG, &month)){
        printf("Error getting day");
        return 1;
    }

    realDate[1] = month;
    return month;
}

uint8_t rtc_get_year(){
    uint8_t regA = 0;
    uint8_t year;
    do {
       // rtc_disable();
        sys_outb(ADDR_REG, REGISTER_A);
        util_sys_inb(DATA_REG, &regA);
        //rtc_enable();
    } while ( regA & BIT(7)); //WHILE update is in progress

    if (sys_outb(ADDR_REG, YEAR_REG)){
        printf("Error accessing ADDR_REG");
        return 1;
    }
    if (util_sys_inb(DATA_REG, &year)){
        printf("Error getting year");
        return 1;
    }

    realDate[2] = year ;
    return year;
}

uint8_t rtc_get_seconds(){
    uint8_t regA = 0;
    uint8_t seconds;
    do {
       // rtc_disable();
        sys_outb(ADDR_REG, REGISTER_A);
        util_sys_inb(DATA_REG, &regA);
       // rtc_enable();
    } while ( regA & BIT(7)); //WHILE update is in progress

    if (sys_outb(ADDR_REG, SECONDS_REG)){
        printf("Error accessing ADDR_REG");
        return 1;
    }
    if (util_sys_inb(DATA_REG, &seconds)){
        printf("Error getting seconds");
        return 1;
    }

    realTime[2] = seconds;
    return seconds;
}

uint8_t rtc_get_minutes(){
    uint8_t regA = 0;
    uint8_t minutes;
    do {
        //rtc_disable();
        sys_outb(ADDR_REG, REGISTER_A);
        util_sys_inb(DATA_REG, &regA);
        //rtc_enable();
    } while ( regA & BIT(7)); //WHILE update is in progress

    if (sys_outb(ADDR_REG, MINUTES_REG)){
        printf("Error accessing ADDR_REG");
        return 1;
    }
    if (util_sys_inb(DATA_REG, &minutes)){
        printf("Error getting minutes");
        return 1;
    }

    realTime[1] = minutes;
    return minutes;
}

uint8_t rtc_get_hours(){
    uint8_t regA = 0;
    uint8_t hours;
    do {
        //rtc_disable();
        sys_outb(ADDR_REG, REGISTER_A);
        util_sys_inb(DATA_REG, &regA);
        //rtc_enable();
    } while ( regA & BIT(7)); //WHILE update is in progress

    if (sys_outb(ADDR_REG, HOURS_REG)){
        printf("Error accessing ADDR_REG");
        return 1;
    }
    if (util_sys_inb(DATA_REG, &hours)){
        printf("Error getting hours");
        return 1;
    }

    realTime[0] = hours;
    return hours;

}

void rtc_ih(){
    uint8_t regC;

    if (sys_outb(ADDR_REG, REGISTER_C)){
        printf("Error accessing ADDR_REG\n");
        return;
    }
    if (util_sys_inb(DATA_REG, &regC)){
        printf("Error getting data from DATA_REG");
        return;
    }

    if (regC & BIT(4)){
        rtc_get_seconds();
        rtc_get_minutes();
        rtc_get_hours();
    }
}

void rtc_start(){
    rtc_get_day();
    rtc_get_month();
    rtc_get_year();

    rtc_get_hours();
    rtc_get_minutes();
    rtc_get_seconds();



}


uint8_t bcd_to_dec(uint8_t bcd){

    uint8_t mask = BIT(0) | BIT(1) | BIT(2) | BIT(3);
    mask = mask & bcd;
    if (bcd <= 9)
        return bcd;
    else{

        return ( bcd >> 4) * 10 + mask;
    }



}

void printDate(){
    printf("%d/%d/20%d\n", bcd_to_dec(realDate[0]), bcd_to_dec(realDate[1]), bcd_to_dec(realDate[2]));
}

void printTime(){
    printf("%d:%d:%d\n", bcd_to_dec(realTime[0]), bcd_to_dec(realTime[1]), bcd_to_dec(realTime[2]));
}
