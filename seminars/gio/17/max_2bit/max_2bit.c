#include "max_2bit.h"

int FindMax(void* encoding) {
  unsigned char * bytes = encoding;
  int max = -1;
  int max_pos = 0;
  for(int i = 0; bytes[i] != 0; i++){
    for(int j = 0; j < 4; j++){
      int num = (bytes[i] >> (6 - 2 * j)) & 3;
      if(num > max){
        max = num;
        max_pos = 4 * i + j;
      }
    }
  }
  return max_pos;
}
