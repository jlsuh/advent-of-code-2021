#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define POSSIBLE_DUMBOS_SIZE 8
#define COMPONENTS_SIZE 2

typedef struct {
    int energyLevel;
    bool flash;
} t_dumbo;

t_dumbo** get_dumbos_matrix(FILE* input, int* rows, int* columns) {
    t_dumbo** dumbos = calloc(++(*rows), sizeof(t_dumbo*));
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        if(c == '\n') {
            dumbos = realloc(dumbos, ++(*rows) * sizeof(t_dumbo*));
            dumbos[*rows - 1] = NULL;
            *columns = 0;
        } else {
            dumbos[*rows - 1] = realloc(dumbos[*rows - 1], ++(*columns) * sizeof(t_dumbo));
            dumbos[*rows - 1][*columns - 1].energyLevel = c - '0';
            dumbos[*rows - 1][*columns - 1].flash = false;
        }
    }
    rewind(input);
    return dumbos;
}

void matrix_destroy(void** matrix, int dim) {
    for(int i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void dumbos_print(t_dumbo** dumbos, int rows, int columns) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            if(dumbos[i][j].energyLevel == 0) {
                printf("\e[1;93m%d\e[0m", dumbos[i][j].energyLevel);
            } else {
                printf("%d", dumbos[i][j].energyLevel);
            }
        }
        printf("\n");
    }
}

void increase_dumbo_energy(t_dumbo* dumbo) {
    dumbo->energyLevel += 1;
}

t_dumbo** increase_all_dumbos_energy(t_dumbo** dumbos, int rows, int columns) {
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            increase_dumbo_energy(&dumbos[i][j]);
        }
    }
    return dumbos;
}

int** possible_dumbos(int rows, int columns, int currX, int currY, int *size) {
    int** moves = NULL;
    int possibleMoves[POSSIBLE_DUMBOS_SIZE][COMPONENTS_SIZE] = {
        {currX - 1, currY},
        {currX - 1, currY + 1},
        {currX, currY + 1},
        {currX + 1, currY + 1},
        {currX + 1, currY},
        {currX + 1, currY - 1},
        {currX, currY - 1},
        {currX - 1, currY - 1},
    };
    for(int i = 0; i < POSSIBLE_DUMBOS_SIZE; i++) {
        if(possibleMoves[i][0] >= 0 && possibleMoves[i][0] < columns && possibleMoves[i][1] >= 0 && possibleMoves[i][1] < rows) {
            moves = realloc(moves, ++(*size) * sizeof(int*));
            moves[*size - 1] = calloc(COMPONENTS_SIZE, sizeof(int));
            moves[*size - 1][0] = possibleMoves[i][0];
            moves[*size - 1][1] = possibleMoves[i][1];
        }
    }
    return moves;
}

void make_surrounding_dumbos_flash(t_dumbo** dumbos, int rows, int columns, int currX, int currY) {
    int size = 0;
    int** possibleMoves = possible_dumbos(rows, columns, currX, currY, &size);
    for(int k = 0; k < size; k++) {
        increase_dumbo_energy(&dumbos[possibleMoves[k][0]][possibleMoves[k][1]]);
    }
    matrix_destroy((void**) possibleMoves, size);
}

void make_dumbos_flash(t_dumbo** dumbos, int rows, int columns, int* numberOfFlashes) {
    bool dumboFlash = false;
    for(int i = 0; i < rows;) {
        for(int j = 0; j < columns;) {
            t_dumbo* dumbo = &dumbos[i][j];
            if(dumbo->energyLevel > 9 && !dumbo->flash) {
                dumbo->flash = true;
                dumboFlash = true;
                make_surrounding_dumbos_flash(dumbos, rows, columns, i, j);
                j = 0;
            } else {
                j++;
            }
        }
        if(dumboFlash) {
            i = 0;
            dumboFlash = false;
        } else {
            i++;
        }
    }
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < columns; j++) {
            if(dumbos[i][j].energyLevel > 9) {
                dumbos[i][j].energyLevel = 0;
            }
            if(dumbos[i][j].flash) {
                (*numberOfFlashes)++;
                dumbos[i][j].flash = false;
            }
        }
    }
}

int simulate_steps(t_dumbo** dumbos, int rows, int columns, int numberOfSteps) {
    int numberOfFlashes = 0;
    int currStep = 1;
    printf("Before any steps:\n");
    dumbos_print(dumbos, rows, columns);
    while(currStep <= numberOfSteps) {
        dumbos = increase_all_dumbos_energy(dumbos, rows, columns);
        printf("\nAfter step %d:\n", currStep);
        make_dumbos_flash(dumbos, rows, columns, &numberOfFlashes);
        dumbos_print(dumbos, rows, columns);
        currStep++;
    }
    return numberOfFlashes;
}

int solution(FILE* input, int numberOfSteps) {
    int rows = 0;
    int columns = 0;
    t_dumbo** dumbos = get_dumbos_matrix(input, &rows, &columns);
    int numberOfFlashes = simulate_steps(dumbos, rows, columns, numberOfSteps);
    matrix_destroy((void**) dumbos, rows);
    return numberOfFlashes;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 100"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input, atoi(argv[2]));
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
