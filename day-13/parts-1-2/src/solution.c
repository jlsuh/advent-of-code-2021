#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FOLD_SIZE 18
#define DOTS_DELIMITER ','

typedef struct {
    char** folds;
    int numberOfFolds;
    bool** matrix;
    int xMax;
    int yMax;
} t_instruction;

void matrix_destroy(void** matrix, int dim) {
    for(int i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void instruction_destroy(t_instruction* instruction) {
    matrix_destroy((void**) instruction->folds, instruction->numberOfFolds);
    matrix_destroy((void**) instruction->matrix, instruction->yMax);
    free(instruction);
}

t_instruction* instruction_create(FILE* input, char dotDelimiter) {
    t_instruction* self = malloc(sizeof(t_instruction));
    int numberOfDots = 0;
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        if(c == dotDelimiter) {
            numberOfDots++;
        }
    }
    rewind(input);
    int xMax = -1;
    int yMax = -1;
    for(int i = 0; i < numberOfDots; i++) {
        int x = -1;
        int y = -1;
        fscanf(input, "%d,%d", &x, &y);
        if(x > xMax) {
            xMax = x;
        }
        if(y > yMax) {
            yMax = y;
        }
    }
    rewind(input);
    self->xMax = xMax + 1;
    self->yMax = yMax + 1;
    self->matrix = calloc(self->yMax, sizeof(bool*));
    for(int i = 0; i < self->yMax; i++) {
        self->matrix[i] = calloc(self->xMax, sizeof(bool));
    }
    for(int i = 0; i < numberOfDots; i++) {
        int x = -1;
        int y = -1;
        fscanf(input, "%d,%d", &x, &y);
        self->matrix[y][x] = true;
    }
    while((c = fgetc(input)) == '\n');
    ungetc(c, input);
    self->numberOfFolds = 0;
    self->folds = NULL;
    char* fold = calloc(FOLD_SIZE, sizeof(char));
    while(fgets(fold, FOLD_SIZE, input) != NULL) {
        if(strcmp(fold, "\n") != 0) {
            strtok(fold, " ");
            strtok(NULL, " ");
            char* segment = strtok(NULL, " ");
            self->folds = realloc(self->folds, ++(self->numberOfFolds) * sizeof(char*));
            self->folds[self->numberOfFolds - 1] = strdup(segment);
        }
    }
    free(fold);
    rewind(input);
    return self;
}

void instruction_print(t_instruction* instruction) {
    for(int i = 0; i < instruction->yMax; i++) {
        for(int j = 0; j < instruction->xMax; j++) {
            if(instruction->matrix[i][j]) {
                printf("\e[1;33m#\e[0m");
                continue;
            }
            printf(".");
        }
        printf("\n");
    }
}

void instruction_transpose(t_instruction* instruction) {
    int tempYMax = instruction->yMax;
    instruction->yMax = instruction->xMax;
    instruction->xMax = tempYMax;
    bool** matrix = calloc(instruction->yMax, sizeof(bool*));
    for(int i = 0; i < instruction->yMax; i++) {
        matrix[i] = calloc(instruction->xMax, sizeof(bool));
    }
    for(int i = 0; i < instruction->xMax; i++) {
        for(int j = 0; j < instruction->yMax; j++) {
            matrix[j][i] = instruction->matrix[i][j];
        }
    }
    matrix_destroy((void**) instruction->matrix, tempYMax);
    instruction->matrix = matrix;
}

void instruction_fold(t_instruction* instruction, int currFold) {
    char foldAxis = '\0';
    int axisValue = -1;
    sscanf(instruction->folds[currFold], "%c=%d", &foldAxis, &axisValue);
    bool transposeAgain = false;
    if(foldAxis == 'x') {
        instruction_transpose(instruction);
        transposeAgain = true;
    }
    int upperIndex = axisValue - 1;
    for(int col = 0; col < instruction->xMax; col++) {
        for(int lowerIndex = axisValue + 1; lowerIndex < instruction->yMax; lowerIndex++) {
            if(upperIndex < 0) {
                instruction->yMax += 1;
                bool** matrix = calloc(instruction->yMax, sizeof(bool*));
                for(int newRowIndex = 0; newRowIndex < instruction->yMax; newRowIndex++) {
                    matrix[newRowIndex] = calloc(instruction->xMax, sizeof(bool));
                    if(newRowIndex > 0) {
                        memcpy(matrix[newRowIndex], instruction->matrix[newRowIndex - 1], instruction->xMax * sizeof(bool));
                    }
                }
                upperIndex = 0;
                lowerIndex++;
                axisValue++;
                matrix_destroy((void**) instruction->matrix, instruction->yMax - 1);
                instruction->matrix = matrix;
            }
            if(instruction->matrix[lowerIndex][col]) {
                instruction->matrix[upperIndex][col] = instruction->matrix[lowerIndex][col];
            }
            upperIndex--;
        }
        upperIndex = axisValue - 1;
    }
    for(int i = instruction->yMax - 1; i >= axisValue; i--) {
        free(instruction->matrix[i]);
    }
    instruction->yMax = axisValue;
    if(transposeAgain) {
        instruction_transpose(instruction);
    }
}

int number_of_dots(t_instruction* instruction) {
    int dots = 0;
    for(int i = 0; i < instruction->yMax; i++) {
        for(int j = 0; j < instruction->xMax; j++) {
            if(instruction->matrix[i][j]) {
                dots++;
            }
        }
    }
    return dots;
}

int solution(FILE* input) {
    t_instruction* instruction = instruction_create(input, DOTS_DELIMITER);
    for(int i = 0; i < instruction->numberOfFolds; i++) {
        instruction_fold(instruction, i);
    }
    instruction_print(instruction);
    int numberOfDots = number_of_dots(instruction);
    instruction_destroy(instruction);
    return numberOfDots;
}

int main(int argc, char *argv[] /*ARGS="../input.txt"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input);
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
