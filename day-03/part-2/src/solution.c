#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>

#define ONE '1'
#define ZERO '0'

typedef enum {
    OXYGEN_GENERATOR_RATING,
    CO2_SCRUBBER_RATING
} t_bit_criteria;

int binary_to_dec(char* binaryNumber) {
    int decimal = 0;
    for(int i = 0; i < strlen(binaryNumber); i++) {
        if(binaryNumber[i] == ONE) {
            decimal += pow(2, strlen(binaryNumber) - 1 - i);
        }
    }
    return decimal;
}

void set_bit_criteria(t_bit_criteria criteria, char* wantedBit, int* nextNumberOfEntries, int zeros, int ones) {
    if(criteria == OXYGEN_GENERATOR_RATING) {
        if(zeros > ones) {
            *wantedBit = ZERO;
            *nextNumberOfEntries = zeros;
        } else if(ones > zeros || ones == zeros) {
            *wantedBit = ONE;
            *nextNumberOfEntries = ones;
        }
    } else if(criteria == CO2_SCRUBBER_RATING) {
        if(zeros > ones) {
            *wantedBit = ONE;
            *nextNumberOfEntries = ones;
        } else if(ones > zeros || ones == zeros) {
            *wantedBit = ZERO;
            *nextNumberOfEntries = zeros;
        }
    }
}

int get_rating(FILE* input, int numberOfEntries, int maxBinaryLen, t_bit_criteria criteria) {
    int currNumberOfEntries = numberOfEntries;
    char* binaryNumber = calloc(maxBinaryLen, sizeof(char));
    char** numbers = calloc(currNumberOfEntries, sizeof(char*));
    for(int i = 0; fscanf(input, "%s", binaryNumber) != EOF; i++) {
        numbers[i] = strdup(binaryNumber);
    }
    rewind(input);

    for(int bitIndex = 0; bitIndex < maxBinaryLen - 1; bitIndex++) {
        if(currNumberOfEntries == 1) {
            break;
        }

        int zeros = 0;
        int ones = 0;
        for(int i = 0; i < currNumberOfEntries; i++) {
            if(numbers[i][bitIndex] == ZERO) {
                zeros++;
            } else {
                ones++;
            }
        }
        // printf("ZEROS: %d | ONES: %d\n", zeros, ones);

        char** tempNumbers = NULL;
        char wantedBit = '\0';
        int nextNumberOfEntries = -1;
        set_bit_criteria(criteria, &wantedBit, &nextNumberOfEntries, zeros, ones);

        tempNumbers = calloc(nextNumberOfEntries, sizeof(char*));
        int j = 0;
        for(int i = 0; i < currNumberOfEntries; i++) {
            if(numbers[i][bitIndex] == wantedBit) {
                tempNumbers[j] = strdup(numbers[i]);
                j++;
            }
            if(j == nextNumberOfEntries) {
                break;
            }
        }

        for(int i = 0; i < currNumberOfEntries; i++) {
            // printf("NUMBER %d: %s\n", i, numbers[i]);
            free(numbers[i]);
        }
        free(numbers);

        currNumberOfEntries = nextNumberOfEntries;
        numbers = tempNumbers;
    }
    free(binaryNumber);
    int rating = binary_to_dec(numbers[0]);
    free(numbers[0]);
    free(numbers);
    return rating;
}

int solution(FILE* input, int numberOfEntries, int maxBinaryLen) {
    int oxygenGeneratorRating = get_rating(input, numberOfEntries, maxBinaryLen, OXYGEN_GENERATOR_RATING);
    int CO2ScrubberRating = get_rating(input, numberOfEntries, maxBinaryLen, CO2_SCRUBBER_RATING);
    printf("OXYGEN GENERATOR RATING: %d | CO2 SCRUBBER RATING: %d\n", oxygenGeneratorRating, CO2ScrubberRating);
    return oxygenGeneratorRating * CO2ScrubberRating;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 1000 13"*/) {
    FILE* measurements = fopen(argv[1], "r");
    if(measurements == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(measurements, atoi(argv[2]), atoi(argv[3]));
        printf("Answer: %d\n", answer);
    }
    fclose(measurements);
    return 0;
}
