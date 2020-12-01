#include <stdio.h>
#include <string.h>
#include "s2e.h"

#define SIZE 1024

int main(){
	char str[SIZE] = "initial string";
	s2e_make_symbolic(&str, SIZE, "str");
	return strlen(str);	
}
