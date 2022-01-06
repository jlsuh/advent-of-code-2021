#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define PAIR_SIZE 2
#define ELEM_SIZE 1

typedef struct {
    char* pair;
    char elem;
    uint64_t occurrences;
} t_rule;

typedef struct {
    char* initialPolymer;
    size_t initialPolymerSize;
    t_rule* rules;
    size_t rulesSize;
} t_instruction;

void increment_occurrences(uint64_t* incrementOccurrences, t_instruction* instruction) {
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        instruction->rules[i].occurrences += incrementOccurrences[i];
    }
}

void increment_step_occurrences(uint64_t* incrementedOccurrences, t_instruction* instruction, char* pair, uint64_t newOccurrences) {
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        if(strcmp(instruction->rules[i].pair, pair) == 0) {
            incrementedOccurrences[i] += newOccurrences;
            break;
        }
    }
}

void set_initial_occurrences(t_instruction* instruction) {
    uint64_t* incrementedOccurrences = calloc(instruction->rulesSize, sizeof(uint64_t));
    char* subPair = calloc(PAIR_SIZE + 1, sizeof(char));
    for(size_t i = 0; i < instruction->initialPolymerSize - PAIR_SIZE; i++) {
        strncpy(subPair, instruction->initialPolymer + i, PAIR_SIZE);
        printf("subPair: %s\n", subPair);
        increment_step_occurrences(incrementedOccurrences, instruction, subPair, 1);
    }
    increment_occurrences(incrementedOccurrences, instruction);
    free(incrementedOccurrences);
    free(subPair);
}

void matrix_destroy(void** matrix, size_t dim) {
    for(size_t i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void rules_destroy(t_rule* self, size_t rulesSize) {
    for(size_t i = 0; i < rulesSize; i++) {
        free(self[i].pair);
    }
    free(self);
}

void instruction_destroy(t_instruction* self) {
    rules_destroy(self->rules, self->rulesSize);
    free(self->initialPolymer);
    free(self);
}

t_instruction* instruction_create(FILE* input) {
    t_instruction* self = malloc(sizeof(t_instruction));
    char c = '\0';
    size_t polymerSize = 1;
    while((c = fgetc(input)) != '\n') {
        polymerSize++;
    }
    rewind(input);
    char* initialPolymer = calloc(polymerSize, sizeof(char));
    fscanf(input, "%s", initialPolymer);
    while((c = fgetc(input)) == '\n');
    ungetc(c, input);
    char* pair = calloc(PAIR_SIZE + 1, sizeof(char));
    self->rulesSize = 0;
    self->rules = NULL;
    while(fscanf(input, "%s -> %c", pair, &c) != EOF) {
        self->rules = realloc(self->rules, ++(self->rulesSize) * sizeof(t_rule));
        self->rules[self->rulesSize - 1].pair = strdup(pair);
        self->rules[self->rulesSize - 1].elem = c;
        self->rules[self->rulesSize - 1].occurrences = 0;
    }
    self->initialPolymer = initialPolymer;
    self->initialPolymerSize = polymerSize;
    set_initial_occurrences(self);
    free(pair);
    rewind(input);
    return self;
}

char associated_elem(t_instruction* instruction, char* targetPair) {
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        if(strcmp(instruction->rules[i].pair, targetPair) == 0) {
            return instruction->rules[i].elem;
        }
    }
    return '\0';
}

bool contains_elem(char associatedElem, char* elements, size_t elementsSize) {
    for(size_t i = 0; i < elementsSize; i++) {
        if(elements[i] == associatedElem) {
            return true;
        }
    }
    return false;
}

char* elements_create(t_instruction* instruction, size_t* elementsSize) {
    char* elements = calloc(++(*elementsSize), sizeof(char));
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        char associatedElem = associated_elem(instruction, instruction->rules[i].pair);
        if(!contains_elem(associatedElem, elements, *elementsSize)) {
            elements[*elementsSize - 1] = associatedElem;
            elements = realloc(elements, ++(*elementsSize) * sizeof(char));
            elements[*elementsSize - 1] = '\0';
        }
    }
    return elements;
}

char** insertion_pairs(t_instruction* instruction, char* subPair) {
    char** pairs = calloc(PAIR_SIZE, sizeof(char*));
    for(size_t i = 0; i < PAIR_SIZE; i++) {
        pairs[i] = calloc(PAIR_SIZE + 1, sizeof(char));
    }
    char associatedElem = associated_elem(instruction, subPair);
    char* pair = calloc(PAIR_SIZE + 1, sizeof(char));
    size_t j = 0;
    pair[j] = subPair[j];
    pair[++j] = associatedElem;
    sprintf(pairs[j - 1], "%s", pair);
    pair[j - 1] = associatedElem;
    pair[j] = subPair[j];
    sprintf(pairs[j], "%s", pair);
    free(pair);
    return pairs;
}

void insert_polymer_pairs(t_instruction* instruction) {
    uint64_t* incrementedOccurrences = calloc(instruction->rulesSize, sizeof(uint64_t));
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        if(instruction->rules[i].occurrences > 0) {
            char** pairs = insertion_pairs(instruction, instruction->rules[i].pair);
            for(size_t j = 0; j < PAIR_SIZE; j++) {
                increment_step_occurrences(incrementedOccurrences, instruction, pairs[j], instruction->rules[i].occurrences);
            }
            instruction->rules[i].occurrences = 0;
            matrix_destroy((void**) pairs, PAIR_SIZE);
        }
    }
    increment_occurrences(incrementedOccurrences, instruction);
    free(incrementedOccurrences);
}

size_t elem_index(char elem, char* elements) {
    size_t i = 0;
    while(elements[i] != '\0') {
        if(elements[i] == elem) {
            return i;
        }
        i++;
    }
    return -1;
}

uint64_t* elements_occurrences(char* elements, size_t elementsSize, t_instruction* instruction) {
    uint64_t* occurrences = calloc(elementsSize - 1, sizeof(uint64_t));
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        char* pair = instruction->rules[i].pair;
        char left = pair[0];
        occurrences[elem_index(left, elements)] += instruction->rules[i].occurrences;
    }
    char lastElem = instruction->initialPolymer[instruction->initialPolymerSize - 2];
    occurrences[elem_index(lastElem, elements)] += 1;
    return occurrences;
}

void instruction_print(t_instruction* instruction) {
    printf("Rules:\n");
    for(size_t i = 0; i < instruction->rulesSize; i++) {
        printf("%s: %ld -> %c\n", instruction->rules[i].pair, instruction->rules[i].occurrences, instruction->rules[i].elem);
    }
}

uint64_t most_least_elems_diff(t_instruction* instruction) {
    size_t elementsSize = 0;
    char* elements = elements_create(instruction, &elementsSize);
    uint64_t* occurrences = elements_occurrences(elements, elementsSize, instruction);
    for(size_t i = 0; i < elementsSize - 1; i++) {
        printf("Element: %c | Occurrences: %ld\n", elements[i], occurrences[i]);
    }
    uint64_t max = occurrences[0];
    uint64_t min = occurrences[0];
    for(size_t i = 1; i < elementsSize - 1; i++) {
        uint64_t occurrence = occurrences[i];
        if(occurrence > max) {
            max = occurrence;
        }
        if(occurrence < min) {
            min = occurrence;
        }
    }
    free(elements);
    free(occurrences);
    return max - min;
}

uint64_t solution(FILE* input, size_t steps) {
    t_instruction* instruction = instruction_create(input);
    printf("Step: 0 out of %ld\n", steps);
    instruction_print(instruction);
    for(size_t i = 0; i < steps; i++) {
        printf("Step: %ld out of %ld\n", i + 1, steps);
        insert_polymer_pairs(instruction);
        instruction_print(instruction);
    }
    uint64_t diff = most_least_elems_diff(instruction);
    instruction_destroy(instruction);
    return diff;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 40"*/) {
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
