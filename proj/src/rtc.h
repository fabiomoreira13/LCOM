//
// Created by Fabio on 12/12/2019.
//

#ifndef PROJ_RTC_H
#define PROJ_RTC_H

#include "rtc_macros.h"
/**
 * Subscribes the interruptions for the RTC
 * @param bit_no
 * @return 0 in case of success, 1 otherwise
 */
int (rtc_subscribe_int)(uint8_t *bit_no);


/**
 * Unsubscribes interrupts for RTC
 * @return 0 in case of success, 1 otherwise
 */
int (rtc_unsubscribe_int)();


/**
 * Disables interrupts for RTC
 * @return 0 in case of success, 1 otherwise
 */
int (rtc_disable)();


/**
 * Enables interrupts for RTC
 * @return 0 in case of success, 1 otherwise
 */
int (rtc_enable)();


/**
 * Gets the day and updates date array
 * @return day
 */
uint8_t rtc_get_day();


/**
 *  Gets the month and updates date array
 * @return month
 */
uint8_t rtc_get_month();


/**
 *  Gets the year and updates date array
 * @return year
 */
uint8_t rtc_get_year();


/**
 *  Gets the seconds and updates time array
 * @return seconds
 */
uint8_t rtc_get_seconds();


/**
 * Gets the minutes and updates time array
 * @return minutes
 */
uint8_t rtc_get_minutes();


/**
 * Gets the hours and updates time array
 * @return hours
 */
uint8_t rtc_get_hours();


/**
 * Interrupt Handler for the RTC
 */
void rtc_ih();


/**
 * Gets the initial information of time and date
 */
void rtc_start();

uint8_t bcd_to_dec(uint8_t bcd);

void printTime();

void printDate();


#endif //PROJ_RTC_H
