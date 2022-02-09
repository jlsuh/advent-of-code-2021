#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BASE10 10
#define SPLIT_MIN 10
#define EXPLODE_MIN 5
#define NULL_CHAR '\0'
#define PAIR_SIZE 2
#define LEFT 0
#define RIGHT 1

typedef enum {
    EXPLODE,
    SPLIT
} t_search;

typedef struct {
    uint32_t values[PAIR_SIZE];
    uint32_t sizes[PAIR_SIZE];
    uint32_t newLens[PAIR_SIZE];
    uint32_t newNumberLen;
    int32_t firstIndexes[PAIR_SIZE];
    int32_t firsts[PAIR_SIZE];
    int32_t news[PAIR_SIZE];
} t_explode;

void matrix_destroy(void** matrix, size_t dim) {
    for(size_t i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

size_t number_of_lines(FILE* const input) {
    size_t n = 0;
    char curr = NULL_CHAR;
    char prev = curr;
    for(;;) {
        prev = curr;
        curr = fgetc(input);
        if (curr == '\n') {
            n += 1;
            continue;
        }
        if (curr == EOF && prev == '\n') {
            break;
        }
        if (curr == EOF) {
            n += 1;
            break;
        }
    }
    rewind(input);
    return n;
}

char** extract_lines(FILE* const input, size_t* const size) {
    char** lines = calloc(*size, sizeof(*lines));
    size_t n = 0;
    size_t curr = 0;
    while(curr < *size - 1) {
        getline(&lines[curr], &n, input);
        n = strlen(lines[curr]);
        lines[curr][n - 1] = NULL_CHAR;
        curr += 1;
    }
    getline(&lines[curr], &n, input);       // store allocated size in n (last ']' included without null char)
    n += 1;                                 // include null char
    lines[curr] = realloc(lines[curr], n);  // realloc to store allocated size with null char
    lines[curr][n - 1] = NULL_CHAR;         // set last to null char
    return lines;
}

char** snailfish_numbers_extract(FILE* const input, size_t* const sfnSize) {
    *sfnSize = number_of_lines(input);
    return extract_lines(input, sfnSize);
}

uint32_t number_of_digits(uint32_t x) { return snprintf(0, 0, "%+d", x) - 1; }

bool is_split(uint32_t n) { return n >= SPLIT_MIN; }

bool is_explode(size_t n) { return n >= EXPLODE_MIN; }

bool is_null_char(char c) { return c == NULL_CHAR; }

int32_t first_occurrence_index(size_t initial, char direction, char* number) {
    int32_t curr = initial;
    if(direction == 'L') {
        while (!isdigit(number[curr]) || isdigit(number[curr - 1])) {
            curr -= 1;
            if (curr == -1) {
                break;
            } else if(curr == 0) {
                curr -= 1;
                break;
            }
        }
    } else if (direction == 'R') {
        while (!isdigit(number[curr])) {
            curr += 1;
            if (is_null_char(number[curr])) {
                curr = -1;
                break;
            }
        }
    }
    return curr;
}

t_explode explode_create(size_t const initial, char * const number) {
    uint32_t left = strtoul(number + initial + 1, NULL, BASE10);
    uint32_t right = strtoul(number + initial + 1 + number_of_digits(left) + 1, NULL, BASE10);
    uint32_t values[PAIR_SIZE] = {left, right};
    uint32_t sizes[PAIR_SIZE] = {
        number_of_digits(left),
        number_of_digits(right)
    };
    int32_t firstIndexes[PAIR_SIZE] = {
        first_occurrence_index(initial, 'L', number),
        first_occurrence_index(initial + sizes[LEFT] + 1 + sizes[RIGHT] + 1, 'R', number)
    };
    int32_t firsts[PAIR_SIZE] = {-1, -1};
    int32_t news[PAIR_SIZE] = {-1, -1};
    uint32_t newLens[PAIR_SIZE] = {0, 0};

    uint32_t newNumberLen = strlen(number) + 1;
    newNumberLen -= sizes[LEFT] + sizes[RIGHT];

    for(size_t i = 0; i < PAIR_SIZE; i++) {
        if (firstIndexes[i] > -1) {
            firsts[i] = strtoul(number + firstIndexes[i], NULL, BASE10);
            news[i] = firsts[i] + values[i];
            newLens[i] = number_of_digits(news[i]);
            newNumberLen += newLens[i] - sizes[i];
        }
    }
    t_explode e;
    memcpy(e.values, values, sizeof(values));
    memcpy(e.sizes, sizes, sizeof(sizes));
    memcpy(e.firstIndexes, firstIndexes, sizeof(firstIndexes));
    memcpy(e.firsts, firsts, sizeof(firsts));
    memcpy(e.news, news, sizeof(news));
    memcpy(e.newLens, newLens, sizeof(newLens));
    e.newNumberLen = newNumberLen;
    return e;
}

char* snailfish_number_explode(size_t initial, char * const number) {
    t_explode e = explode_create(initial, number);

    char* newNumber = calloc(e.newNumberLen, sizeof(*newNumber));
    size_t currNew = 0;
    size_t curr = 0;

    if(e.firsts[LEFT] > -1) {
        memcpy(newNumber, number, e.firstIndexes[LEFT]);
        currNew += e.firstIndexes[LEFT];
        curr += e.firstIndexes[LEFT];

        sprintf(newNumber + currNew, "%d", e.news[LEFT]);
        currNew += e.newLens[LEFT];
        curr += number_of_digits(e.firsts[LEFT]);
    }

    size_t currToInitial = initial - curr;
    memcpy(newNumber + currNew, number + curr, currToInitial);
    currNew += currToInitial;
    curr += currToInitial;

    sprintf(newNumber + currNew, "%d", 0);
    currNew += 1;
    curr += 1 + e.sizes[LEFT] + 1 + e.sizes[RIGHT] + 1;

    if (e.firsts[RIGHT] > -1) {
        size_t currToFirstRight = e.firstIndexes[RIGHT] - curr;
        memcpy(newNumber + currNew, number + curr, currToFirstRight);
        currNew += currToFirstRight;
        curr += currToFirstRight;

        sprintf(newNumber + currNew, "%d", e.news[RIGHT]);
        currNew += e.newLens[RIGHT];
        curr += number_of_digits(e.firsts[RIGHT]);
    }

    memcpy(newNumber + currNew, number + curr, strlen(number) - curr);

    return newNumber;
}

char* snailfish_number_split(int32_t initial, char* number) {
    uint32_t value = strtoul(number + initial, NULL, BASE10);

    double half = value / 2.0;
    uint32_t newLeft = (uint32_t) floor(half);
    uint32_t newRight = (uint32_t) round(half);

    size_t numberLen = strlen(number);
    size_t valueLen = number_of_digits(value);
    size_t pairSize = 1 + number_of_digits(newLeft) + 1 + number_of_digits(newRight) + 1;

    char* newNumber = calloc(numberLen + pairSize + 1 - valueLen, sizeof(*newNumber));
    size_t currNew = 0;
    size_t curr = 0;

    memcpy(newNumber, number, initial);
    currNew += initial;
    curr += initial;
    sprintf(newNumber + currNew, "[%d,%d]", newLeft, newRight);
    currNew += pairSize;
    curr += valueLen;
    memcpy(newNumber + currNew, number + curr, numberLen - curr);

    return newNumber;
}

char* snailfish_number_reduce(char* number) {
    size_t bracketsNet = 0;
    int32_t splitIndex = -1;
    uint8_t currSearch = EXPLODE;
    char* newNumber = NULL;
    for (int32_t i = 0; !is_null_char(number[i]); i += 1) {
        if(number[i] == '[') {
            bracketsNet += 1;
        } else if(number[i] == ']') {
            bracketsNet -= 1;
        } else if(splitIndex == -1 && isdigit(number[i])) {
            uint32_t n = strtoul(number + i, NULL, BASE10);
            if(is_split(n)) {
                splitIndex = i;
            }
        }
        bool reset = false;
        bool modified = false;
        if (currSearch == EXPLODE) {
            if (is_explode(bracketsNet)) {
                newNumber = snailfish_number_explode(i, number);
                modified = true;
                reset = true;
            } else if (is_null_char(number[i + 1])) {
                currSearch = SPLIT;
                reset = true;
            }
        } else if (currSearch == SPLIT) {
            if (splitIndex != -1) {
                newNumber = snailfish_number_split(i, number);
                currSearch = EXPLODE;
                modified = true;
                reset = true;
            }
        }
        if (modified) {
            free(number);
            number = newNumber;
        }
        if (reset) {
            bracketsNet = 0;
            i = -1;
            splitIndex = -1;
        }
    }
    return number;
}

char* snailfish_numbers_add(char* left, char* right) {
    size_t len = 1 + strlen(left) + 1 + strlen(right) + 1 + 1;
    char* newNumber = calloc(len, sizeof(*left));
    sprintf(newNumber, "[%s,%s]", left, right);
    return newNumber;
}

char* snailfish_numbers_sum(char** sfn, size_t sfnSize) {
    char* left = strdup(sfn[0]);
    left = snailfish_number_reduce(left);
    for (size_t i = 1; i < sfnSize; i += 1) {
        char* right = strdup(sfn[i]);
        right = snailfish_number_reduce(right);
        char* newNumber = snailfish_numbers_add(left, right);
        free(left);
        free(right);
        newNumber = snailfish_number_reduce(newNumber);
        left = newNumber;
    }
    return left;
}

uint64_t pair_magnitude(uint64_t left, uint64_t right) {
    return 3 * left + 2 * right;
}

uint64_t snailfish_number_magnitude(char const * const sfn) {
    char* number = strdup(sfn);
    int32_t openBracketIndex = -1;
    for (int32_t i = 0; number[i] != '\0'; i += 1) {
        if (number[i] == '[') {
            openBracketIndex = i;
        } else if (number[i] == ']') {
            uint64_t left = strtoull(number + openBracketIndex + 1, NULL, BASE10);
            uint64_t right = strtoull(number + openBracketIndex + 1 + number_of_digits(left) + 1, NULL, BASE10);
            uint64_t pairMagnitude = pair_magnitude(left, right);
            size_t prevPairSize = 1 + number_of_digits(left) + 1 + number_of_digits(right) + 1;
            size_t pairMagnitudeSize = number_of_digits(pairMagnitude);
            size_t newNumberSize = strlen(number) + 1 - prevPairSize + pairMagnitudeSize;

            char* newNumber = calloc(newNumberSize, sizeof(*newNumber));
            size_t currNew = 0;
            size_t curr = 0;

            memcpy(newNumber, number, openBracketIndex);
            currNew += openBracketIndex;
            curr += openBracketIndex;
            sprintf(newNumber + currNew, "%ld", pairMagnitude);
            currNew += pairMagnitudeSize;
            curr += prevPairSize;
            memcpy(newNumber + currNew, number + curr, strlen(number) - curr);

            free(number);
            number = newNumber;

            i = -1;
            openBracketIndex = -1;
        }
    }
    uint64_t m = atoll(number);
    free(number);
    return m;
}

uint64_t largest_magnitude(char * const * const sfnStrArr, size_t sfnSize) {
    uint64_t largest = 0;
    for (size_t i = 0; i < sfnSize; i += 1) {
        char* left = sfnStrArr[i];
        for (size_t j = 0; j < sfnSize; j += 1) {
            if(j == i) {
                continue;
            }
            char* right = sfnStrArr[j];
            char* addition = snailfish_number_reduce(snailfish_numbers_add(left, right));
            uint64_t m = snailfish_number_magnitude(addition);
            if(m > largest) {
                largest = m;
            }
            free(addition);
        }
    }
    return largest;
}

uint64_t solution(FILE* const input) {
    size_t sfnSize = 0;
    char** sfnStrArr = snailfish_numbers_extract(input, &sfnSize);
    char* finalSum = snailfish_numbers_sum(sfnStrArr, sfnSize);
    printf("Final Sum: %s\n", finalSum);
    uint64_t m = snailfish_number_magnitude(finalSum);
    printf("Final Sum Magnitude: %ld\n", m);
    uint64_t largestM = largest_magnitude(sfnStrArr, sfnSize);
    matrix_destroy((void**) sfnStrArr, sfnSize);
    free(finalSum);
    return largestM;
}

int main(int argc, char* argv[] /*ARGS="../input.txt"*/) {
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Failed");
        return -1;
    } else {
        uint64_t answer = solution(input);
        printf("Answer: %ld\n", answer);
    }
    fclose(input);
    return 0;
}
