#include <stdio.h>
#include <stdlib.h>

int get_depth_measurement_increases(int* measurements, int numberOfLines) {
    int numberOfIncreases = 0;
    int prev = measurements[0];
    for(int i = 1; i < numberOfLines; i++) {
        int next = measurements[i];
        if(next > prev) {
            numberOfIncreases++;
        }
        prev = next;
    }
    return numberOfIncreases;
}

int solution(FILE* fp, int numberOfLines) {
    int* measurements = calloc(numberOfLines, sizeof(int));
    int i = 0;
    while (fscanf(fp, "%d", &measurements[i]) != EOF) {
        i++;
    }
    int numberOfIncreases = get_depth_measurement_increases(measurements, numberOfLines);
    free(measurements);
    return numberOfIncreases;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 2000"*/) {
    FILE* measurements = fopen(argv[1], "r");
    if(measurements == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(measurements, atoi(argv[2]));
        printf("Answer: %d\n", answer);
    }
    fclose(measurements);
    return 0;
}
