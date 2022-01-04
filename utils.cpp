#include <stdlib.h>
#include <stdint.h>
#include "utils.h"
#include "env.h"

uint8_t get_bit_arr(uint8_t *arr, size_t i, size_t n){
  if(i>=n*8) return 0;
  uint8_t block = arr[i/8], qtShifts=8-1-(i%8);
  block>>=qtShifts;
  return block&1;
}