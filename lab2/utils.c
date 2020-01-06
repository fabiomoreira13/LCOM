#include <lcom/lcf.h>

#include <stdint.h>

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


