#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t solution(FILE* const input) {
    return 0;
}

int main(int argc, char* argv[] /*ARGS="../input.txt"*/) {
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Failed");
        return -1;
    } else {
        uint64_t answer = solution(input);
        printf("Answer: %ld\n", answer);
    }
    fclose(input);
    return 0;
}
