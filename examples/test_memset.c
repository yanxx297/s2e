#include <stdio.h>
#include <string.h>
#include "s2e.h"

#define SIZE 1024

void *mymset(void *dst, int s, size_t count) {
      char *a = dst;
        while (count-- > 0)
	        *a++ = s;
	  return dst;
}


int main(){
	int arr[SIZE];
	int n;
	s2e_make_symbolic(&n, sizeof(n), "n");	
	n &= 0x1;
	n += 1023;

	if(n > SIZE || n <= 0 ) return 0;
	mymset(arr, 0, n);	
	return 0;
}
