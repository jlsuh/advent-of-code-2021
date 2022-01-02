#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define FOLD_SIZE 18
#define DIMS_SIZE 2

typedef struct {
    int x;
    int y;
} t_point;

typedef struct {
    t_point* dots;
    int numberOfDots;
    char** folds;
    int numberOfFolds;
    t_point maxDims;
    bool** presence;
} t_instruction;

void matrix_destroy(void** matrix, int dim) {
    for(int i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

bool** presence_create(t_instruction* instruction) {
    bool** self = calloc(instruction->maxDims.y, sizeof(bool*));
    for(int i = 0; i < instruction->maxDims.y; i++) {
        self[i] = calloc(instruction->maxDims.x, sizeof(bool));
    }
    for(int i = 0; i < instruction->numberOfDots; i++) {
        t_point dot = instruction->dots[i];
        self[dot.y][dot.x] = true;
    }
    return self;
}

t_instruction* instruction_create(FILE* input, char dotDelimiter) {
    t_instruction* self = malloc(sizeof(t_instruction));
    self->numberOfDots = 0;
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        if(c == dotDelimiter) {
            self->numberOfDots++;
        }
    }
    rewind(input);
    t_point* dots = calloc(self->numberOfDots, sizeof(t_point));
    int xMax = -1;
    int yMax = -1;
    for(int i = 0; i < self->numberOfDots; i++) {
        int x = -1;
        int y = -1;
        fscanf(input, "%d,%d", &x, &y);
        dots[i].x = x;
        dots[i].y = y;
        if(x > xMax) {
            xMax = x;
        }
        if(y > yMax) {
            yMax = y;
        }
    }
    while((c = fgetc(input)) == '\n');
    ungetc(c, input);
    self->maxDims.x = xMax + 1;
    self->maxDims.y = yMax + 1;
    self->dots = dots;
    self->presence = presence_create(self);
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

void instruction_destroy(t_instruction* instruction) {
    matrix_destroy((void**) instruction->presence, instruction->maxDims.y);
    for(int i = 0; i < instruction->numberOfFolds; i++) {
        free(instruction->folds[i]);
    }
    free(instruction->folds);
    free(instruction->dots);
    free(instruction);
}

void print_instruction(t_instruction* instruction) {
    for(int i = 0; i < instruction->numberOfDots; i++) {
        printf("(%d,%d) \n", instruction->dots[i].x, instruction->dots[i].y);
    }
    for(int i = 0; i < instruction->numberOfFolds; i++) {
        printf("%s\n", instruction->folds[i]);
    }
    printf("MAXDIMS: (%d,%d) \n", instruction->maxDims.x, instruction->maxDims.y);
}

char** origami_create(t_instruction* instruction) {
    char** self = calloc(instruction->maxDims.y, sizeof(char*));
    for(int i = 0; i < instruction->maxDims.y; i++) {
        self[i] = calloc(instruction->maxDims.x, sizeof(char));
        memset(self[i], '.', instruction->maxDims.x);
    }
    for(int i = 0; i < instruction->numberOfDots; i++) {
        t_point dot = instruction->dots[i];
        self[dot.y][dot.x] = '#';
    }
    return self;
}

bool contains_point(t_instruction* instruction, int x, int y) {
    return instruction->presence[y][x];
}

void print_origami(t_instruction* instruction) {
    for(int j = 0; j < instruction->maxDims.y; j++) {
        for(int i = 0; i < instruction->maxDims.x; i++) {
            if(contains_point(instruction, i, j)) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

size_t set_main_dim_size(t_instruction* instruction, int axisValue, int upper, int dimLimit) {
    size_t dimSize = 0;
    int lower = 0;
    for(int i = axisValue + 1; i < dimLimit; i++) {
        lower++;
    }
    if(upper >= lower) {
        dimSize = upper;
    } else {
        dimSize = lower;
    }
    return dimSize;
}

t_instruction* fold_origami(t_instruction* instruction, int currFold) {
    char axis = '\0';
    int axisValue = -1;
    sscanf(instruction->folds[currFold], "%c=%d", &axis, &axisValue);
    size_t yDimSize = -1;
    size_t xDimSize = -1;
    int upper = axisValue;
    t_point* newDots = NULL;
    int newNumberOfDots = 0;
    int outerDimLimit = -1;
    int innerDimLimit = -1;
    if(axis == 'x') {
        xDimSize = set_main_dim_size(instruction, axisValue, upper, instruction->maxDims.x);
        yDimSize = instruction->maxDims.y;
        outerDimLimit = instruction->maxDims.y;
        innerDimLimit = instruction->maxDims.x - 1;
    } else if(axis == 'y') {
        yDimSize = set_main_dim_size(instruction, axisValue, upper, instruction->maxDims.y);
        xDimSize = instruction->maxDims.x;
        outerDimLimit = instruction->maxDims.x;
        innerDimLimit = instruction->maxDims.y - 1;
    }
    int destDim = 0;
    for(int i = 0; i < outerDimLimit; i++) {
        for(int j = innerDimLimit; j > axisValue; j--) {
            int xSrc = -1;
            int ySrc = -1;
            int xDest = -1;
            int yDest = -1;
            if(outerDimLimit == instruction->maxDims.y) {
                xSrc = j;
                ySrc = i;
                xDest = destDim;
                yDest = ySrc;
            } else if(outerDimLimit == instruction->maxDims.x) {
                xSrc = i;
                ySrc = j;
                xDest = xSrc;
                yDest = destDim;
            }
            if(contains_point(instruction, xSrc, ySrc)) {
                if(!contains_point(instruction, xDest, yDest)) {
                    instruction->dots = realloc(instruction->dots, ++(instruction->numberOfDots) * sizeof(t_point));
                    instruction->dots[instruction->numberOfDots - 1].x = xDest;
                    instruction->dots[instruction->numberOfDots - 1].y = yDest;
                }
            }
            destDim++;
        }
        destDim = 0;
    }
    for(int i = 0; i < instruction->numberOfDots; i++) {
        int limit = -1;
        if(innerDimLimit == instruction->maxDims.x - 1) {
            limit = instruction->dots[i].x;
        } else if(innerDimLimit == instruction->maxDims.y - 1) {
            limit = instruction->dots[i].y;
        }
        if(limit < axisValue) {
            newDots = realloc(newDots, ++newNumberOfDots * sizeof(t_point));
            newDots[newNumberOfDots - 1].x = instruction->dots[i].x;
            newDots[newNumberOfDots - 1].y = instruction->dots[i].y;
        }
    }
    free(instruction->dots);
    instruction->dots = newDots;
    instruction->numberOfDots = newNumberOfDots;
    matrix_destroy((void**) instruction->presence, instruction->maxDims.y);
    instruction->maxDims.x = xDimSize;
    instruction->maxDims.y = yDimSize;
    instruction->presence = presence_create(instruction);
//     if(++currFold <= instruction->numberOfFolds) {
//         origami = fold_origami(newOrigami, instruction, currFold);
//     }
//     print_origami(instruction);
    return instruction;
}

int solution(FILE* input) {
    t_instruction* instruction = instruction_create(input, ',');
    print_instruction(instruction);

//     for(int i = 0; i < instruction->numberOfFolds; i++) {
//         instruction = fold_origami(instruction, 0);
//     }

    instruction = fold_origami(instruction, 0);
    print_origami(instruction);
    instruction = fold_origami(instruction, 1);
    print_origami(instruction);
    instruction = fold_origami(instruction, 2);
    print_origami(instruction);
    instruction = fold_origami(instruction, 3);
    print_origami(instruction);
    instruction = fold_origami(instruction, 4);
    print_origami(instruction);
    instruction = fold_origami(instruction, 5);
    print_origami(instruction);
    instruction = fold_origami(instruction, 6);
    print_origami(instruction);
    instruction = fold_origami(instruction, 7);
    print_origami(instruction);
    instruction = fold_origami(instruction, 8);
    print_origami(instruction);
    instruction = fold_origami(instruction, 9);
    print_origami(instruction);
    printf("\n");
    instruction = fold_origami(instruction, 10);
    print_origami(instruction);
    printf("\n");
    instruction = fold_origami(instruction, 11);
    print_origami(instruction);
    
    int numberOfDots = instruction->numberOfDots;

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

/*
####.####.####.#..#.####.####...##.#....
####.####.####.#.#..#.##.####.#.##.#....
#.#..#....####.##.#.###..####..###..###.
####.###..##.#.#.##.##...####...##.#....
#....#..#.####.#.##.####.##.#.####.##...
#....#..#.####.####.####.####.####.####.
*/
