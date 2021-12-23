#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int sum_of_measurements_in_window(int* measurements, int pivot, int windowSize) {
    int sum = 0;
    for (int i = pivot; i < pivot + windowSize; i++) {
        sum += measurements[i];
    }
    return sum;
}

bool offset_from_pivot_is_less_than_limit(int pivot, int windowSize, int limit) {
    return pivot + windowSize < limit;
}

int get_window_sized_depth_measurement_increases(int* measurements, int numberOfLines, int windowSize) {
    int numberOfIncreases = 0;
    for(int i = 0; i + 1 < numberOfLines; i++) {
        int prevPivot = i;
        int nextPivot = i+1;
        if(offset_from_pivot_is_less_than_limit(nextPivot, windowSize - 1, numberOfLines)) {
            int prevSumWindow = sum_of_measurements_in_window(measurements, prevPivot, windowSize);
            int nextSumWindow = sum_of_measurements_in_window(measurements, nextPivot, windowSize);
            if(nextSumWindow > prevSumWindow) {
                numberOfIncreases++;
            }
        }
    }
    return numberOfIncreases;
}

int solution(FILE* input, int numberOfLines, int windowSize) {
    int* measurements = calloc(numberOfLines, sizeof(int));
    int i = 0;
    while (fscanf(input, "%d", &measurements[i]) != EOF) {
        i++;
    }
    int numberOfIncreases = get_window_sized_depth_measurement_increases(measurements, numberOfLines, windowSize);
    free(measurements);
    return numberOfIncreases;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 2000 3"*/) {
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
