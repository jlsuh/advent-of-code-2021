#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
    int* content;
    int size;
} t_array;

void print_array(t_array* array) {
    for (int i = 0; i < array->size; i++) {
        printf("%d ", array->content[i]);
    }
    printf("\n");
}

t_array* array_create(void) {
    t_array* self = malloc(sizeof(t_array));
    self->content = NULL;
    self->size = -1;
    return self;
}

void array_destroy(t_array* array) {
    free(array->content);
    free(array);
}

t_array* set_initial_state(FILE* input, t_array* array) {
    int horizontalPosition = -1;
    int size = 0;
    while(fscanf(input, "%d,", &horizontalPosition) != EOF) {
        array->content = realloc(array->content, ++size * sizeof(int));
        array->content[size - 1] = horizontalPosition;
    }
    array->size = size;
    rewind(input);
    return array;
}

bool is_normal_growth(char* growthType) {
    return strcmp(growthType, "NORMAL") == 0;
}

bool is_gauss_growth(char* growthType) {
    return strcmp(growthType, "GAUSS") == 0;
}

int gauss_sum(int n) {
    return n * (n + 1) / 2;
}

t_array* calculate_fuel_cost(t_array* swarm, t_array* fuelCost, char* growthType) {
    for(int i = 0; i < fuelCost->size; i++) {
        for(int j = 0; j < swarm->size; j++) {
            int fuel = abs(i - swarm->content[j]);
            if(is_normal_growth(growthType)) {
                fuelCost->content[i] += fuel;
            } else if(is_gauss_growth(growthType)) {
                fuelCost->content[i] += gauss_sum(fuel);
            }
        }
    }
    return fuelCost;
}

int index_with_lowest_fuel_cost(t_array* fuelCost) {
    int lowestFuelCost = fuelCost->content[0];
    int lowestFuelCostIndex = 0;
    for(int i = 1; i < fuelCost->size; i++) {
        if(fuelCost->content[i] < lowestFuelCost) {
            lowestFuelCost = fuelCost->content[i];
            lowestFuelCostIndex = i;
        }
    }
    return lowestFuelCostIndex;
}

int max_position(FILE* input) {
    int max = 0;
    int position = -1;
    while(fscanf(input, "%d,", &position) != EOF) {
        if(position > max) {
            max = position;
        }
    }
    rewind(input);
    return max;
}

int solution(FILE* input, char* growthType) {
    int maxPosition = max_position(input);
    t_array* fuelCost = array_create();
    fuelCost->size = maxPosition + 1;
    fuelCost->content = calloc(fuelCost->size, sizeof(int));

    t_array* crabs = array_create();
    crabs = set_initial_state(input, crabs);
    fuelCost = calculate_fuel_cost(crabs, fuelCost, growthType);

    int lowestFuelCostIndex = index_with_lowest_fuel_cost(fuelCost);
    int lowestFuelCost = fuelCost->content[lowestFuelCostIndex];

    array_destroy(crabs);
    array_destroy(fuelCost);
    return lowestFuelCost;
}

int main(int argc, char *argv[] /*ARGS="../input.txt (NORMAL | GAUSS)"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input, argv[2]);
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
