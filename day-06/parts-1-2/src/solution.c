#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FIRST_CYCLE 8
#define SECOND_CYCLE 6

void print_simulation(uint64_t* simulation, char* msg) {
    printf("%s:\t", msg);
    for(uint64_t i = 0; i <= FIRST_CYCLE; i++) {
        printf("%ld", simulation[i]);
        if(i == FIRST_CYCLE) {
            printf("\n");
        } else {
            printf(" | ");
        }
    }
}

void print_day(uint64_t* simulation, uint64_t day) {
    char* prefix = "After";
    char* dayStr = malloc(sizeof(char) * 4);
    sprintf(dayStr, "%ld", day);
    char* postFix = NULL;
    if(day == 1) {
        postFix = "day";
    } else {
        postFix = "days";
    }
    char* msg = malloc(strlen(prefix) + strlen(dayStr) + strlen(postFix) + 3);
    sprintf(msg, "%s %s %s", prefix, dayStr, postFix);
    print_simulation(simulation, msg);
    free(dayStr);
    free(msg);
}

uint64_t* set_initial_state(FILE* input) {
    uint64_t *simulation = calloc(FIRST_CYCLE + 1, sizeof(uint64_t));
    uint64_t i = 0;
    uint64_t fish = -1;
    while(fscanf(input, "%ld,", &fish) != EOF) {
        simulation[fish] += 1;
        i++;
    }
    return simulation;
}

uint64_t* next_day(uint64_t* simulation) {
    uint64_t reproductionFish = simulation[0];
    uint64_t* tempSimulation = calloc(FIRST_CYCLE + 1, sizeof(uint64_t));
    for(uint64_t i = FIRST_CYCLE; i > 0; i--) {
        tempSimulation[i - 1] = simulation[i];
    }
    tempSimulation[FIRST_CYCLE] += reproductionFish;
    tempSimulation[SECOND_CYCLE] += reproductionFish;
    memcpy(simulation, tempSimulation, (FIRST_CYCLE + 1) * sizeof(uint64_t));
    free(tempSimulation);
    return simulation;
}

uint64_t simulation_fish_sum(uint64_t* simulation) {
    uint64_t sum = 0;
    for(uint64_t i = 0; i <= FIRST_CYCLE; i++) {
        sum += simulation[i];
    }
    return sum;
}

uint64_t solution(FILE* input, uint64_t days) {
    uint64_t* simulation = set_initial_state(input);
    print_simulation(simulation, "Initial state");
    for(uint64_t i = 0; i < days; i++) {
        simulation = next_day(simulation);
        print_day(simulation, i + 1);
    }
    uint64_t totalFish = simulation_fish_sum(simulation);
    free(simulation);
    return totalFish;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 256"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        uint64_t answer = solution(input, atoi(argv[2]));
        printf("Answer: %ld\n", answer);
    }
    fclose(input);
    return 0;
}
