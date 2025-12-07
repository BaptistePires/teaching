#include <stdio.h>


char mem[12];

int main() {
        for(int i=0; i<10; i++) {
                printf("Address of mem[%d]: %p\n", i, &mem[i]);
        }
        return 0;
}