#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define NO_SYNTAX_ERROR '-'

typedef struct t_node t_node;
struct t_node {
    char data;
    t_node *next;
};

typedef struct {
    t_node* head;
    int size;
} t_stack;

int number_of_entries(FILE* input) {
    int numberOfEntries = 0;
    char prev = '\0';
    char curr = fgetc(input);
    while (curr != EOF) {
        if(curr == '\n') {
            numberOfEntries++;
        }
        prev = curr;
        curr = fgetc(input);
        if(curr == EOF && prev != '\n') {
            numberOfEntries++;
        }
    }
    rewind(input);
    return numberOfEntries;
}

bool is_opening_char(char c) {
    return c == '(' || c == '[' || c == '{' || c == '<';
}

t_stack* stack_create(void) {
    t_stack* self = malloc(sizeof(t_stack));
    self->head = NULL;
    self->size = 0;
    return self;
}

void stack_destroy(t_stack* stack) {
    t_node* node = stack->head;
    while (node != NULL) {
        t_node* next = node->next;
        free(node);
        node = next;
    }
    free(stack);
}

void stack_push(t_stack* stack, char data) {
    t_node* node = malloc(sizeof(t_node));
    node->data = data;
    if(stack->head == NULL) {
        node->next = NULL;
        stack->head = node;
    } else {
        node->next = stack->head;
        stack->head = node;
    }
}

char stack_pop(t_stack* stack) {
    char popped = '\0';
    if(stack->head != NULL) {
        popped = stack->head->data;
        t_node* poppedNode = stack->head;
        stack->head = stack->head->next;
        free(poppedNode);
    }
    return popped;
}

void stack_print(t_stack* stack) {
    t_node* node = stack->head;
    while(node != NULL) {
        printf("%c", node->data);
        node = node->next;
    }
    printf("\n");
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

char* get_first_occurences_of_illegal_chars(FILE* input, char* firstIllegalChars, int numberOfEntries, int *currIndex) {
    char readChar = '\0';
    bool ignore = false;
    t_stack* stack = stack_create();
    while((readChar = fgetc(input)) != EOF) {
        if(!ignore) {
            if(is_opening_char(readChar)) {
                stack_push(stack, readChar);
            } else if(readChar != '\n') {
                char popped = stack_pop(stack);
                if(popped != '\0') {
                    char closingChar = get_enclosing_char(popped);
                    if(closingChar != readChar) {
                        firstIllegalChars[(*currIndex)++] = readChar;
                        ignore = true;
                    }
                }
            }
        }
        if(readChar == '\n') {
            if(ignore) {
                ignore = false;
            } else {
                firstIllegalChars[(*currIndex)++] = NO_SYNTAX_ERROR;
            }
            stack_destroy(stack);
            stack = stack_create();
        }
    }
    stack_destroy(stack);
    return firstIllegalChars;
}

int get_score(char c) {
    switch(c) {
        case ')':
            return 3;
        case ']':
            return 57;
        case '}':
            return 1197;
        case '>':
            return 25137;
        default:
            return -1;
    }
}

int syntax_error_score(char* firstIllegalChars, int size) {
    int score = 0;
    for(int i = 0; i < size; i++) {
        char illegalChar = firstIllegalChars[i];
        if(illegalChar != NO_SYNTAX_ERROR) {
            score += get_score(illegalChar);
        }
    }
    return score;
}

int solution(FILE* input) {
    int numberOfEntries = number_of_entries(input);
    char* firstIllegalChars = calloc(numberOfEntries, sizeof(char));
    int size = 0;
    firstIllegalChars = get_first_occurences_of_illegal_chars(input, firstIllegalChars, numberOfEntries, &size);
    /* printf("Illegal Chars: ");
    for(int i = 0; i < size; i++) {
        printf("%c ", firstIllegalChars[i]);
    }
    printf("\n%d\n", numberOfEntries); */
    int syntaxErrorScore = syntax_error_score(firstIllegalChars, size);
    free(firstIllegalChars);
    return syntaxErrorScore;
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
