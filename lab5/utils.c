#include <lcom/lcf.h>

#include <stdint.h>
int n_sys_inb_calls=0;

int(util_get_LSB)(uint16_t val, uint8_t *lsb) { //CHECK EXTRA FOR GET MSB
  //Returns the LSB of a 2 byte integer -> val
  //lsb is the address of memory location to be updated with Val's lsb
  //Returns 0 upon success and non zero otherwise
  if (NULL == lsb)
      return 1;
  *lsb = (uint8_t )val;
  return 0;  //Make it check if failed or not?? i.e. must return 0 or 1?
}


int(util_get_MSB)(uint16_t val, uint8_t *msb) {  //MAYBE ADD A CHECK TO SEE IF MSB == NULL RETURN 1?
    if (NULL == msb){
        return 1;
    }
   *msb = (uint8_t)(val >>8); //Right shift of 8 bits and then cast to a 8bit number
   return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t aux;
  if (sys_inb(port, &aux))
    return 1;
  #ifdef LAB3
  n_sys_inb_calls++;
  #endif
  *value=(uint8_t) aux;
  return 0;
}



/*
int KBC_write(uint8_t *cmd){
  uint16_t stat=IBF;
  while (stat & IBF) {
    delay(WAIT_KBC);
    if (util_sys_inb(STAT_REG, &stat))
      return 1
  }
  if (sys_outb(KBC_CMD_REG, cmd)) 
    return 1;

  return 0;
}

int KBC_read(uint16_t *read){
  uint16_t stat=!OBF, data;
  while (stat & !OBF) {
    delay(WAIT_KBC);
    if (sys_inb(STAT_REG, &stat))
      return 1;
  }
 //loop while 8042 output buffer is empty
  if (sys_inb(OUT_BUF, &data)) // assuming it returns OK
    return 1;
  if ((stat &(PAR_ERR | TO_ERR)) == 0)
    return data;
  else
    return 1;
}
delay(WAIT_KBC);
}
}*/
