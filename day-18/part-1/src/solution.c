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

typedef enum {
    EXPLODE,
    SPLIT
} t_search;

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
    while(curr < *size) {
        getline(&lines[curr], &n, input);
        n = strlen(lines[curr]);
        lines[curr][n - 1] = NULL_CHAR;
        curr += 1;
    }
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

char* snailfish_number_explode(size_t initial, char* number) {
    uint32_t left = strtoul(number + initial + 1, NULL, BASE10);
    uint32_t right = strtoul(number + initial + 1 + number_of_digits(left) + 1, NULL, BASE10);

    int32_t firstLeftIndex = first_occurrence_index(initial, 'L', number);
    size_t leftSize = number_of_digits(left);
    size_t rightSize = number_of_digits(right);
    int32_t firstRightIndex = first_occurrence_index(initial + leftSize + 1 + rightSize + 1, 'R', number);

    uint32_t firstLeft = 0;
    uint32_t newLeft = 0;
    size_t leftLen = 0;
    size_t newLeftLen = 0;

    uint32_t firstRight = 0;
    uint32_t newRight = 0;
    size_t rightLen = 0;
    size_t newRightLen = 0;

    bool firstLeftFound = false;
    bool firstRightFound = false;

    size_t newLen = strlen(number) + 1;
    newLen -= leftSize + 1 + rightSize + 1;   // "x,y]" => "0"

    if (firstLeftIndex > -1) {
        firstLeft = strtoul(number + firstLeftIndex, NULL, BASE10);
        newLeft = firstLeft + left;
        leftLen = leftSize;
        newLeftLen = number_of_digits(newLeft);
        newLen += newLeftLen - leftLen;
        firstLeftFound = true;
    }
    if (firstRightIndex > -1) {
        firstRight = strtoul(number + firstRightIndex, NULL, BASE10);
        newRight = firstRight + right;
        rightLen = rightSize;
        newRightLen = number_of_digits(newRight);
        newLen += newRightLen - rightLen;
        firstRightFound = true;
    }

    char* newNumber = calloc(newLen + 1 + 1, sizeof(*newNumber));
    size_t currNew = 0;
    size_t curr = 0;

    if(firstLeftFound) {
        memcpy(newNumber, number, firstLeftIndex);
        currNew += firstLeftIndex;
        curr += firstLeftIndex;

        sprintf(newNumber + currNew, "%d", newLeft);
        currNew += newLeftLen;
        curr += number_of_digits(firstLeft);
    }

    size_t currToInitial = initial - curr;
    memcpy(newNumber + currNew, number + curr, currToInitial);
    currNew += currToInitial;
    curr += currToInitial;

    sprintf(newNumber + currNew, "%d", 0);
    currNew += 1;
    curr += 1 + leftSize + 1 + rightSize + 1;   // "[x,y]"

    if (firstRightFound) {
        size_t currToFirstRight = firstRightIndex - curr;
        memcpy(newNumber + currNew, number + curr, currToFirstRight);
        currNew += currToFirstRight;
        curr += currToFirstRight;

        sprintf(newNumber + currNew, "%d", newRight);
        currNew += newRightLen;
        curr += number_of_digits(firstRight);
    }

    memcpy(newNumber + currNew, number + curr, strlen(number) - curr);

    return newNumber;
}

char* snailfish_number_split(int32_t initial, char* number) {
    uint32_t value = strtoul(number + initial, NULL, BASE10);

    double half = value / 2.0;
    uint32_t newLeft = (uint32_t) floor(half);
    uint32_t newRight = (uint32_t) round(half);

    size_t len = strlen(number);
    size_t valueLen = number_of_digits(value);
    size_t pairSize = 1 + number_of_digits(newLeft) + 1 + number_of_digits(newRight) + 1;
    size_t newSize = len + pairSize + 1 - valueLen /* => [x,y] */;

    char* newNumber = calloc(newSize, sizeof(*newNumber));

    size_t currNew = 0;
    size_t curr = 0;

    memcpy(newNumber, number, initial);

    currNew += initial;
    curr += initial;

    sprintf(newNumber + currNew, "[%d,%d]", newLeft, newRight);

    currNew += pairSize;
    curr += valueLen;

    memcpy(newNumber + currNew, number + curr, len - curr);

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
    printf("Left: %s\n", left);
    for (size_t i = 1; i < sfnSize; i += 1) {
        char* right = strdup(sfn[i]);
        right = snailfish_number_reduce(right);
        printf("Right: %s\n", right);
        char* newNumber = snailfish_numbers_add(left, right);
        free(left);
        free(right);
        newNumber = snailfish_number_reduce(newNumber);
        printf("New Number: %s\n", newNumber);
        left = newNumber;
    }
    return left;
}

uint64_t solution(FILE* const input) {
    size_t sfnSize = 0;
    char** sfnStrArr = snailfish_numbers_extract(input, &sfnSize);
    char* finalSum = snailfish_numbers_sum(sfnStrArr, sfnSize);
    printf("Final sum: %s\n", finalSum);
    // TODO: Calculate magnitude
    matrix_destroy((void**) sfnStrArr, sfnSize);
    free(finalSum);
    return 0;
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
