#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#define HEX_BIN_SIZE 4
#define HEADER_COMPONENT_SIZE 3
#define LITERAL_GROUP_SIZE 4
#define LITERAL_PACKET 4
#define PREFIX_SIZE 1
#define LENGTH_TYPE_ID_SIZE 1
#define SUBPACKETS_NUMBER_SIZE 11
#define SUBPACKETS_TOTAL_LENGTH_SIZE 15

void matrix_destroy(void** matrix, size_t dim) {
    for(size_t i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

char* get_msg(FILE* input, size_t* msgSize) {
    char* msg = NULL;
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        msg = realloc(msg, ++(*msgSize) * sizeof(char));
        msg[*msgSize - 1] = c;
    }
    msg = realloc(msg, ++(*msgSize) * sizeof(char));
    msg[*msgSize - 1] = '\0';
    rewind(input);
    return msg;
}

char* hex_to_bin(char hex) {
    switch(hex) {
        case '0': return "0000";
        case '1': return "0001";
        case '2': return "0010";
        case '3': return "0011";
        case '4': return "0100";
        case '5': return "0101";
        case '6': return "0110";
        case '7': return "0111";
        case '8': return "1000";
        case '9': return "1001";
        case 'A': return "1010";
        case 'B': return "1011";
        case 'C': return "1100";
        case 'D': return "1101";
        case 'E': return "1110";
        case 'F': return "1111";
    }
    return NULL;
}

char* hexSeq_to_binSeq(char* hexSeq, size_t hexSeqSize) {
    size_t binSeqSize = HEX_BIN_SIZE * (hexSeqSize - 1) + 1;
    char* binSeq = calloc(binSeqSize, sizeof(char));
    size_t i = 0;
    while(hexSeq[i]) {
        strcat(binSeq, hex_to_bin(hexSeq[i]));
        i++;
    }
    binSeq[binSeqSize - 1] = '\0';
    return binSeq;
}

uint64_t bin_to_dec(char* bin /* null terminated */) {
    uint64_t dec = 0;
    size_t i = 0;
    while(bin[i]) {
        if(bin[i] == '1') dec = dec * 2 + 1;
        else if(bin[i] == '0') dec *= 2;
        i++;
    }
    return dec;
}

bool is_last_packet(char* binSeq, uint64_t offset) {
    for(size_t i = offset; binSeq[i]; i++) {
        if(binSeq[i] == '1') return false;
    }
    return true;
}

uint64_t number_of_digits(uint64_t x) {
    return snprintf(0, 0, "%+ld", x) - 1;
}

char* associated_symbol(uint8_t typeID) {
    switch(typeID) {
        case 0: return "+";
        case 1: return "*";
        case 2: return "min";
        case 3: return "max";
        case 5: return ">";
        case 6: return "<";
        case 7: return "==";
    }
    return NULL;
}

char** preorder_gen(char* binSeq, size_t* preorderSize) {
    char* version = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(char));
    char* typeIDBin = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(char));
    version[HEADER_COMPONENT_SIZE] = '\0';
    typeIDBin[HEADER_COMPONENT_SIZE] = '\0';
    uint64_t offset = 0;

    char** preorder = NULL;
    char* preorderElem = NULL;

    do {
        preorder = realloc(preorder, ++(*preorderSize) * sizeof(char*));

        printf("\n");
        memcpy(version, binSeq + offset, HEADER_COMPONENT_SIZE);
        offset += HEADER_COMPONENT_SIZE;
        printf("Version: %ld\n", bin_to_dec(version));
        memcpy(typeIDBin, binSeq + offset, HEADER_COMPONENT_SIZE);
        offset += HEADER_COMPONENT_SIZE;

        uint8_t typeIDDec = bin_to_dec(typeIDBin);
        printf("Type ID: %d\n", typeIDDec);

        if(typeIDDec == LITERAL_PACKET) {
            uint64_t valueInBinSize = LITERAL_GROUP_SIZE + 1;
            char* valueInBin = calloc(valueInBinSize, sizeof(char));

            uint64_t literalValue = 0;
            bool literalsEnd = false;

            while(!literalsEnd) {
                char prefix = binSeq[offset];
                offset += PREFIX_SIZE;
                strncat(valueInBin, binSeq + offset, LITERAL_GROUP_SIZE);
                offset += LITERAL_GROUP_SIZE;
                if(prefix == '0') {
                    valueInBin[valueInBinSize - 1] = '\0';
                    literalValue = bin_to_dec(valueInBin);
                    printf("Literal value: %ld\n", literalValue);
                    uint64_t preorderElemSize = number_of_digits(literalValue) + 1;
                    printf("PreorderElemSize: %ld\n", preorderElemSize);
                    preorderElem = realloc(preorderElem, preorderElemSize);
                    sprintf(preorderElem, "%ld", literalValue);
                    preorderElem[preorderElemSize - 1] = '\0';
                    literalsEnd = true;
                } else if(prefix == '1') {
                    valueInBinSize += LITERAL_GROUP_SIZE;
                    valueInBin = realloc(valueInBin, valueInBinSize);
                }
            }
            free(valueInBin);
        } else {
            char lengthTypeID = binSeq[offset];
            offset += LENGTH_TYPE_ID_SIZE;
            printf("Length type ID: %c\n", lengthTypeID);

            uint64_t binSize = 0;
            if(lengthTypeID == '0') {
                binSize = SUBPACKETS_TOTAL_LENGTH_SIZE + 1;
            } else if(lengthTypeID == '1') {
                binSize = SUBPACKETS_NUMBER_SIZE + 1;
            }
            char* binAux = calloc(binSize, sizeof(char));
            memcpy(binAux, binSeq + offset, binSize - 1);
            binAux[binSize - 1] = '\0';
            offset += binSize - 1;
            uint64_t dec = bin_to_dec(binAux);
            printf("Value: %ld\n", dec);
            free(binAux);

            uint8_t typeIDDec = bin_to_dec(typeIDBin);
            char* associatedSymbol = associated_symbol(typeIDDec);
            printf("Associated symbol: %s | Size: %ld\n", associatedSymbol, strlen(associatedSymbol) + 1);
            preorderElem = strdup(associatedSymbol);
        }

        preorder[*preorderSize - 1] = preorderElem;
        preorderElem = NULL;

    } while(!is_last_packet(binSeq, offset));
    free(version);
    free(typeIDBin);
    return preorder;
}

void expression_print(char** expression, size_t size) {
    printf("[");
    for(size_t i = 0; i < size; i++) {
        if(i == size - 1) printf("%s", expression[i]);
        else printf("%s, ", expression[i]);
    }
    printf("]\n");
}

bool is_sum(char* str) { return strcmp(str, "+") == 0; }
bool is_prod(char* str) { return strcmp(str, "*") == 0; }
bool is_min(char* str) { return strcmp(str, "min") == 0; }
bool is_max(char* str) { return strcmp(str, "max") == 0; }
bool is_greater(char* str) { return strcmp(str, ">") == 0; }
bool is_less(char* str) { return strcmp(str, "<") == 0; }
bool is_equal(char* str) { return strcmp(str, "==") == 0; }
bool is_operator(char* str) {
    return is_sum(str) || is_prod(str) || is_min(str) || is_max(str) || is_greater(str) || is_less(str) || is_equal(str);
}

void push(char*** stack, int32_t* stackSize, char* elem) {
    *stack = realloc(*stack, ++(*stackSize) * sizeof(char*));
    (*stack)[*stackSize - 1] = strdup(elem);
}

char* pop(char*** stack, int32_t* stackSize) {
    char* elem = (*stack)[*stackSize - 1];
    *stack = realloc(*stack, --(*stackSize) * sizeof(char*));
    if(*stackSize == 0) {
        *stack = NULL;
    }
    return elem;
}

bool is_empty_stack(char** stack) {
    return stack == NULL;
}

uint64_t min(uint64_t a, uint64_t b) {
    return a < b ? a : b;
}

uint64_t max(uint64_t a, uint64_t b) {
    return a > b ? a : b;
}

char* currify(char* LVal, char* operator, char* RVal) {
    uint64_t newLVal = 0;
    uint64_t decLVal = atoll(LVal);
    uint64_t decRVal = atoll(RVal);
    if(is_sum(operator)) {
        newLVal = decLVal + decRVal;
    } else if(is_prod(operator)) {
        newLVal = decLVal * decRVal;
    } else if(is_min(operator)) {
        newLVal = min(decLVal, decRVal);
    } else if(is_max(operator)) {
        newLVal = max(decLVal, decRVal);
    } else if(is_greater(operator)) {
        newLVal = decLVal > decRVal;
    } else if(is_less(operator)) {
        newLVal = decLVal < decRVal;
    } else if(is_equal(operator)) {
        newLVal = decLVal == decRVal;
    }
    char* currified = calloc(number_of_digits(newLVal) + 1, sizeof(char));
    sprintf(currified, "%ld", newLVal);
    return currified;
}

uint64_t expression_eval(char** preorder, size_t preorderSize) {
    char** stack = NULL;
    int32_t stackSize = 0;
    int32_t i = preorderSize - 1;
    int32_t pushedLiterals = 0;
    expression_print(stack, stackSize);
    while(i >= 0) {
        if(is_operator(preorder[i])) {
            char* currified = NULL;
            char* LVal = NULL;
            char* RVal = NULL;
            if(pushedLiterals) {
                bool stop = false;
                while(!stop) {
                    LVal = pop(&stack, &stackSize);
                    pushedLiterals--;
                    if(pushedLiterals >= 1) {
                        RVal = pop(&stack, &stackSize);
                        pushedLiterals--;
                        currified = currify(LVal, preorder[i], RVal);
                        free(LVal);
                        free(RVal);
                        pushedLiterals++;
                    } else {
                        currified = LVal;
                        stop = true;
                    }
                    push(&stack, &stackSize, currified);
                    free(currified);
                    expression_print(stack, stackSize);
                }
                pushedLiterals = 1;
            } else if(i == 0) {
                while(stackSize > 1) {
                    LVal = pop(&stack, &stackSize);
                    RVal = pop(&stack, &stackSize);
                    currified = currify(LVal, preorder[i], RVal);
                    push(&stack, &stackSize, currified);
                    free(LVal);
                    free(RVal);
                    free(currified);
                    expression_print(stack, stackSize);
                }
            }
        } else {
            push(&stack, &stackSize, preorder[i]);
            pushedLiterals++;
            expression_print(stack, stackSize);
        }
        i--;
    }
    uint64_t eval = atoll(stack[0]);
    matrix_destroy((void**) stack, 1);
    return eval;
}

uint64_t solution(FILE* input) {
    size_t hexSeqSize = 0;
    char* hexSeq = get_msg(input, &hexSeqSize);
    printf("Hex: %s\n", hexSeq);
    char* binSeq = hexSeq_to_binSeq(hexSeq, hexSeqSize);
    printf("Binary: %s\n", binSeq);
    size_t preorderSize = 0;
    char** preorder = preorder_gen(binSeq, &preorderSize);
    printf("\n");
    expression_print(preorder, preorderSize);
    uint64_t eval = expression_eval(preorder, preorderSize);
    free(hexSeq);
    free(binSeq);
    matrix_destroy((void**) preorder, preorderSize);
    return eval;
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
