#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define NO_SYNTAX_ERROR '-'

typedef struct t_node t_node;
struct t_node {
    char data;
    t_node *next;
};

typedef struct {
    t_node* head;
    uint64_t size;
} t_stack;

bool is_opening_char(char c) {
    return c == '(' || c == '[' || c == '{' || c == '<';
}

char get_enclosing_char(char c) {
    switch(c) {
        case '(':
            return ')';
        case '[':
            return ']';
        case '{':
            return '}';
        case '<':
            return '>';
        default:
            return '\0';
    }
}

uint64_t get_enclosing_char_score(char c) {
    switch(c) {
        case ')':
            return 1;
        case ']':
            return 2;
        case '}':
            return 3;
        case '>':
            return 4;
        default:
            return -1;
    }
}

t_stack* stack_create(void) {
    t_stack* self = malloc(sizeof(t_stack));
    self->head = NULL;
    self->size = 0;
    return self;
}

void stack_destroy(t_stack* stack) {
    t_node* node = stack->head;
    while(node != NULL) {
        t_node* next = node->next;
        free(node);
        node = next;
    }
    free(stack);
}

void stack_array_destroy(t_stack** stack, uint64_t size) {
    for(uint64_t i = 0; i < size; i++) {
        if(stack[i] != NULL) {
            stack_destroy(stack[i]);
        }
    }
    free(stack);
}

void stack_push(t_stack* stack, char data) {
    t_node* node = malloc(sizeof(t_node));
    node->data = data;
    node->next = stack->head;
    stack->head = node;
    stack->size++;
}

char stack_pop(t_stack* stack) {
    if(stack->head == NULL) {
        return '\0';
    }
    char data = stack->head->data;
    t_node* node = stack->head;
    stack->head = stack->head->next;
    free(node);
    stack->size--;
    return data;
}

t_stack* stack_reverse(t_stack* stack) {
    t_stack* reversed = stack_create();
    t_node* current = stack->head;
    while(current != NULL) {
        stack_push(reversed, current->data);
        current = current->next;
    }
    return reversed;
}

uint64_t* bubble_sort(uint64_t* array, uint64_t size) {
    uint64_t temp = 0;
    for(uint64_t i = 0; i < size; i++) {
        for(uint64_t j = 0; j < size - 1; j++) {
            if(array[j] > array[j + 1]) {
                temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
    return array;
}

t_stack** get_array_of_incomplete_syntax(FILE* input, uint64_t *currIndex) {
    char readChar = '\0';
    bool ignore = false;
    t_stack** syntaxSequences = calloc(++(*currIndex), sizeof(t_stack*));
    syntaxSequences[(*currIndex) - 1] = stack_create();
    while((readChar = fgetc(input)) != EOF) {
        if(!ignore) {
            t_stack* stack = syntaxSequences[*(currIndex) - 1];
            if(is_opening_char(readChar)) {
                stack_push(stack, readChar);
            } else if(readChar != '\n') {
                char popped = stack_pop(stack);
                if(popped != '\0') {
                    char enclosingChar = get_enclosing_char(popped);
                    if(enclosingChar != readChar) {
                        ignore = true;
                    }
                }
            }
        }
        if(readChar == '\n') {
            if(ignore) {
                ignore = false;
                stack_destroy(syntaxSequences[*(currIndex) - 1]);
            } else {
                syntaxSequences = realloc(syntaxSequences, ++(*currIndex) * sizeof(t_stack*));
            }
            syntaxSequences[(*currIndex) - 1] = stack_create();
        }
    }
    return syntaxSequences;
}

t_stack** get_array_of_complete_chunks(t_stack** incompleteSyntax, uint64_t size) {
    t_stack** completeChunks = calloc(size, sizeof(t_stack*));
    for(uint64_t i = 0; i < size; i++) {
        completeChunks[i] = stack_create();
        t_stack* destStack = completeChunks[i];
        t_stack* srcStack = incompleteSyntax[i];
        t_node* current = srcStack->head;
        while(current != NULL) {
            char enclosingChar = get_enclosing_char(current->data);
            if(enclosingChar != '\0') {
                stack_push(destStack, enclosingChar);
            }
            current = current->next;
        }
    }
    return completeChunks;
}

uint64_t* get_scores(t_stack** stacks, uint64_t size) {
    uint64_t* scores = calloc(size, sizeof(uint64_t));
    for(uint64_t i = 0; i < size; i++) {
        t_stack* stack = stack_reverse(stacks[i]);
        uint64_t score = 0;
        while(stack->size > 0) {
            char popped = stack_pop(stack);
            score = score * 5 + get_enclosing_char_score(popped);
        }
        scores[i] = score;
        stack_destroy(stack);
    }
    return scores;
}

uint64_t get_middle_score(uint64_t* scores, uint64_t size) {
    uint64_t index = size / 2;
    return scores[index];
}

uint64_t solution(FILE* input) {
    uint64_t size = 0;
    t_stack** incompleteSyntax = get_array_of_incomplete_syntax(input, &size);
    t_stack** completeSyntax = get_array_of_complete_chunks(incompleteSyntax, size);
    uint64_t* scores = get_scores(completeSyntax, size);
    scores = bubble_sort(scores, size);
    uint64_t middleScore = get_middle_score(scores, size);
    stack_array_destroy(completeSyntax, size);
    stack_array_destroy(incompleteSyntax, size);
    free(scores);
    return middleScore;
}

int main(int argc, char *argv[] /*ARGS="../input.txt"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        uint64_t answer = solution(input);
        printf("Answer: %ld\n", answer);
    }
    fclose(input);
    return 0;
}
