#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 9

typedef struct {
    char* rowName;
    char* colName;
    bool related;
} t_cave;

typedef struct {
    t_cave** caves;
    int squaredDim;
} t_matrix;

typedef struct {
    char* smallCaveName;
    int occurrences;
} t_traverse;

t_matrix* matrix_create(char** caveNames, int dim) {
    t_matrix* self = malloc(sizeof(t_matrix));
    self->squaredDim = dim;
    self->caves = calloc(dim, sizeof(t_cave*));
    for(int i = 0; i < dim; i++) {
        self->caves[i] = calloc(dim, sizeof(t_cave));
        for(int j = 0; j < dim; j++) {
            self->caves[i][j].rowName = calloc(MAX_LEN, sizeof(char));
            self->caves[i][j].colName = calloc(MAX_LEN, sizeof(char));
            self->caves[i][j].related = false;
            if(i == j) {
                strcpy(self->caves[i][j].rowName, caveNames[i]);
                strcpy(self->caves[i][j].colName, caveNames[i]);
            } else {
                strcpy(self->caves[i][j].rowName, caveNames[i]);
                strcpy(self->caves[i][j].colName, caveNames[j]);
            }
        }
    }
    return self;
}

void matrix_destroy(void** matrix, int dim) {
    for(int i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void traverse_destroy(t_traverse* traversedSmallCavesAux, int traversedSize) {
    for(int i = 0; i < traversedSize; i++) {
        free(traversedSmallCavesAux[i].smallCaveName);
    }
    free(traversedSmallCavesAux);
}

void row_column_destroy(void* matrixVoid) {
    t_matrix* matrix = (t_matrix*) matrixVoid;
    for(int i = 0; i < matrix->squaredDim; i++) {
        for(int j = 0; j < matrix->squaredDim; j++) {
            free(matrix->caves[i][j].rowName);
            free(matrix->caves[i][j].colName);
        }
        free(matrix->caves[i]);
    }
    free(matrix->caves);
    free(matrix);
}

void add_to_array_if_absent(char*** arr, char* str, int* size) {
    for(int i = 0; i < *size; i++) {
        if(strcmp((*arr)[i], str) == 0) {
            return;
        }
    }
    if(*size == 0) {
        *arr = calloc(++(*size), sizeof(char*));
    } else {
        *arr = realloc(*arr, ++(*size) * sizeof(char*));
    }
    (*arr)[*size - 1] = calloc(MAX_LEN, sizeof(char));
    strcpy((*arr)[*size - 1], str);
}

char** cave_names(FILE* input, int* caveSize) {
    char** caveNames = NULL;
    char* temp = calloc(MAX_LEN, sizeof(char));
    while(fscanf(input, "%s", temp) != EOF) {
        char* leftMember = strtok(temp, "-");
        char* rightMember = strtok(NULL, "-");
        add_to_array_if_absent(&caveNames, leftMember, caveSize);
        add_to_array_if_absent(&caveNames, rightMember, caveSize);
    }
    rewind(input);
    free(temp);
    return caveNames;
}

int occurences_greater_or_equal_than(int limit, t_traverse* traversedSmallCaves, int traversedSize) {
    int occurences = 0;
    for(int i = 0; i < traversedSize; i++) {
        if(traversedSmallCaves[i].occurrences >= limit) {
            occurences++;
        }
    }
    return occurences;
}

void print_caves(t_matrix* cavesMatrix) {
    for(int i = 0; i < cavesMatrix->squaredDim; i++) {
        for(int j = 0; j < cavesMatrix->squaredDim; j++) {
            printf(
                "[%s][%s]: %d ",
                cavesMatrix->caves[i][j].rowName,
                cavesMatrix->caves[i][j].colName,
                cavesMatrix->caves[i][j].related
            );
            if(j < cavesMatrix->squaredDim - 1) {
                printf("| ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void print_grammar(t_matrix* cavesMatrix) {
    for(int i = 0; i < cavesMatrix->squaredDim; i++) {
        t_cave* cave = cavesMatrix->caves[i];
        if(strcmp(cave->rowName, "end") != 0) {
            printf("%s -> ", cave->rowName);
            bool pipe = false;
            for(int j = 0; j < cavesMatrix->squaredDim; j++) {
                if(cave[j].related) {
                    if(pipe) {
                        printf("| ");
                        pipe = false;
                    }
                    printf("%s ", cave[j].colName);
                    pipe = true;
                }
                if(j == cavesMatrix->squaredDim - 1) {
                    printf("\n");
                }
            }
        }
    }
    printf("\n");
}

void _(void) {
    char* _[] = {
        "⠀     ⠀⠀⠀⠀⠀⣀⣠⣤⣤⣤⡄",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠛⣿⣟⣛⣃⡀",
        "⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⠿⠿⠟",
        "⠀⠀⠀⣀⣀⣀⣀⣀⣀⣀⣶⠿⢿⣆⣀⣀⣀⣀",
        " ⢠⡾⠋⠉⠉⠉⠉⠉⠉⠉  ⠉⠉⠉⠉⠉⠛⠛⢦⡀",
        " ⢸⡇      COPIUM         ⢸⡇",
        " ⢸⡇                      ⣷",
        " ⣸⡆                       ⣟",
        " ⣿⡇                        ⣿",
        " ⢻⡇                        ⢸⡏",
        " ⢸⡇                        ⣿",
        " ⢸⡇40Liters               ⣷",
        " ⠘⢷⣄⣀⣀⣀⣀⣀⣀⠀⠀⠀⠀⠀⠀⠀⣀⡀⡀⢀⣿",
        " ⠀⠀⠀⠀⠉⠉⠉⠉⠙⠛⠛⠛⠛⠛⠛⠛⠋⠉⠉⠉⠁"
    };
    for(int i = 0; i < 14; i++) {
        printf("%s\n", _[i]);
    }
}

bool is_end(char* str) {
    return strcmp(str, "end") == 0;
}

bool is_start(char* str) {
    return strcmp(str, "start") == 0;
}

bool is_small_cave(char* str) {
    return islower(str[0]);
}

bool is_valid_ocurrences(t_traverse* traversedSmallCaves, int traversedSize, char* name) {
    for(int i = 0; i < traversedSize; i++) {
        if(strcmp(traversedSmallCaves[i].smallCaveName, name) == 0) {
            int occurrences = traversedSmallCaves[i].occurrences;
            if(occurrences == 0) {
                return true;
            } else if(occurrences == 1) {
                if(occurences_greater_or_equal_than(2, traversedSmallCaves, traversedSize) == 0) {
                    return false;
                } else {
                    return true;
                }
            }
            return true;
        }
    }
    return false;
}

bool is_traversed_cave(t_traverse* traversedSmallCaves, char* caveName, int traversedSize) {
    for(int i = 0; i < traversedSize; i++) {
        char* name = traversedSmallCaves[i].smallCaveName;
        if(name != NULL && strcmp(name, caveName) == 0 && is_valid_ocurrences(traversedSmallCaves, traversedSize, name)) {
            return true;
        }
    }
    return false;
}

t_cave* find_cave(t_matrix* cavesMatrix, char* caveName) {
    t_cave* target = NULL;
    for(int i = 0; i < cavesMatrix->squaredDim; i++) {
        target = cavesMatrix->caves[i];
        if(strcmp(target->rowName, caveName) == 0) {
            break;
        }
    }
    return target;
}

void set_relations(FILE* input, t_matrix* cavesMatrix) {
    char* temp = calloc(MAX_LEN, sizeof(char));
    while(fscanf(input, "%s", temp) != EOF) {
        char* leftMember = strtok(temp, "-");
        char* rightMember = strtok(NULL, "-");
        for(int i = 0; i < cavesMatrix->squaredDim; i++) {
            if(strcmp(cavesMatrix->caves[i]->rowName, leftMember) == 0) {
                for(int j = 0; j < cavesMatrix->squaredDim; j++) {
                    if(strcmp(cavesMatrix->caves[i][j].colName, rightMember) == 0) {
                        if(!is_start(rightMember) && !is_end(leftMember)) {
                            cavesMatrix->caves[i][j].related = true;
                        }
                        if(!is_start(leftMember) && !is_end(rightMember)) {
                            cavesMatrix->caves[j][i].related = true;
                        }
                    }
                }
            }
        }
    }
    rewind(input);
    free(temp);
}

int get_number_of_small_caves(t_matrix* cavesMatrix) {
    int numberOfSmallCaves = 0;
    for(int i = 0; i < cavesMatrix->squaredDim; i++) {
        t_cave* cave = cavesMatrix->caves[i];
        if(!is_start(cave->rowName) && !is_end(cave->rowName) && is_small_cave(cave->rowName)) {
            numberOfSmallCaves++;
        }
    }
    return numberOfSmallCaves;
}

int get_index(t_traverse* traversedSmallCaves, int traversedSize, char* name) {
    for(int i = 0; i < traversedSize; i++) {
        if(traversedSmallCaves[i].smallCaveName == NULL || strcmp(traversedSmallCaves[i].smallCaveName, name) == 0) {
            return i;
        }
    }
    return -1;
}

int get_number_of_paths(t_cave* start, t_matrix* cavesMatrix, int numberOfPaths, t_traverse* traversedSmallCaves, int traversedSize) {
    for(int i = 0; i < cavesMatrix->squaredDim; i++) {
        t_traverse* traversedSmallCavesAux = calloc(traversedSize, sizeof(t_traverse));
        for(int j = 0; j < traversedSize; j++) {
            if(traversedSmallCaves[j].smallCaveName != NULL) {
                traversedSmallCavesAux[j].smallCaveName = strdup(traversedSmallCaves[j].smallCaveName);
            } else {
                traversedSmallCavesAux[j].smallCaveName = NULL;
            }
            traversedSmallCavesAux[j].occurrences = traversedSmallCaves[j].occurrences;
        }
        t_cave cave = start[i];
        if(cave.related) {
            if(is_end(cave.colName)) {
                numberOfPaths++;
                traverse_destroy(traversedSmallCavesAux, traversedSize);
                continue;
            } else if(is_small_cave(cave.colName)) {
                if(is_traversed_cave(traversedSmallCavesAux, cave.colName, traversedSize)) {
                    traverse_destroy(traversedSmallCavesAux, traversedSize);
                    continue;
                } else {
                    int nextIndex = get_index(traversedSmallCavesAux, traversedSize, cave.colName);
                    if(traversedSmallCavesAux[nextIndex].occurrences == 0) {
                        traversedSmallCavesAux[nextIndex].smallCaveName = strdup(cave.colName);
                    }
                    traversedSmallCavesAux[nextIndex].occurrences++;
                }
            }
            t_cave* nextCave = find_cave(cavesMatrix, cave.colName);
            numberOfPaths = get_number_of_paths(nextCave, cavesMatrix, numberOfPaths, traversedSmallCavesAux, traversedSize);
        }
        traverse_destroy(traversedSmallCavesAux, traversedSize);
    }
    return numberOfPaths;
}

int solution(FILE* input) {
    int caveSize = 0;
    char** caveNames = cave_names(input, &caveSize);
    printf("Size of matrix: %d\n", caveSize);

    t_matrix* cavesMatrix = matrix_create(caveNames, caveSize);
    print_caves(cavesMatrix);
    set_relations(input, cavesMatrix);
    print_caves(cavesMatrix);
    print_grammar(cavesMatrix);

    t_cave* start = find_cave(cavesMatrix, "start");
    int numberOfPaths = 0;
    int numberOfSmallCaves = get_number_of_small_caves(cavesMatrix);
    t_traverse* traversedSmallCaves = calloc(numberOfSmallCaves, sizeof(t_traverse));
    for(int i = 0; i < numberOfSmallCaves; i++) {
        traversedSmallCaves[i].smallCaveName = NULL;
        traversedSmallCaves[i].occurrences = 0;
    }
    numberOfPaths = get_number_of_paths(start, cavesMatrix, numberOfPaths, traversedSmallCaves, numberOfSmallCaves);

    row_column_destroy(cavesMatrix);
    traverse_destroy(traversedSmallCaves, numberOfSmallCaves);
    matrix_destroy((void**) caveNames, caveSize);
    return numberOfPaths;
}

int main(int argc, char *argv[] /*ARGS="../input.txt"*/) {
    time_t start;
    time(&start);
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input);
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    time_t end;
    time(&end);
    double diffT = difftime(end, start);
    printf("Time diff: %.2f\n", diffT);
    _();
    return 0;
}
