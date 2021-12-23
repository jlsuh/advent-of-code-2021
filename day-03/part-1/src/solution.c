#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define ONE '1'
#define ZERO '0'

int binary_to_dec(char* binaryNumber) {
    int decimal = 0;
    for(int i = 0; i < strlen(binaryNumber); i++) {
        if(binaryNumber[i] == ONE) {
            decimal += pow(2, strlen(binaryNumber) - 1 - i);
        }
    }
    return decimal;
}

int solution(FILE* input, int numberOfEntries, int maxBinaryLen) {
    char* binaryNumber = calloc(maxBinaryLen, sizeof(char));
    char* gammaRate = calloc(maxBinaryLen, sizeof(char));
    char* epsilonRate = calloc(maxBinaryLen, sizeof(char));
    for(int i = 0; i < maxBinaryLen - 1; i++) {
        int zeros = 0;
        int ones = 0;
        while(fscanf(input, "%s", binaryNumber) != EOF) {
            if(binaryNumber[i] == ZERO) {
                zeros++;
            } else {
                ones++;
            }
        }
        if(zeros > ones) {
            gammaRate[i] = ZERO;
            epsilonRate[i] = ONE;
        } else {
            gammaRate[i] = ONE;
            epsilonRate[i] = ZERO;
        }
        rewind(input);
    }
    int gammaValue = binary_to_dec(gammaRate);
    int epsilonValue = binary_to_dec(epsilonRate);
    free(binaryNumber);
    free(gammaRate);
    free(epsilonRate);
    return gammaValue * epsilonValue;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 1000 13"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input, atoi(argv[2]), atoi(argv[3]));
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
