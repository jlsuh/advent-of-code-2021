#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <gmp.h>

#define HEX_BIN_SIZE 4
#define HEADER_COMPONENT_SIZE 3
#define LITERAL_GROUP_SIZE 4
#define LITERAL_PACKET 4
#define PREFIX_SIZE 1
#define LENGTH_TYPE_ID_SIZE 1
#define SUBPACKETS_NUMBER_SIZE 11
#define SUBPACKETS_TOTAL_LENGTH_SIZE 15
#define BASE10 10

typedef struct t_packet t_packet;
struct t_packet {
    uint8_t ver;
    char* tid;
    char* val;
    char ltid;
    uint64_t subPacketsSize;
    t_packet** subPackets;
};

void matrix_destroy(void** matrix, size_t dim) {
    for(size_t i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

char* get_hexSeq(FILE* input, size_t* hexSeqSize) {
    char* hex = calloc(++(*hexSeqSize), sizeof(char));
    char c = fgetc(input);
    while(c != EOF && c != '\n') {
        hex[*hexSeqSize - 1] = c;
        hex = realloc(hex, ++(*hexSeqSize) * sizeof(char));
        c = fgetc(input);
    }
    hex[*hexSeqSize - 1] = '\0';
    rewind(input);
    return hex;
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

char* hexSeq_to_binSeq(char* hexSeq, size_t binSeqSize) {
    char* binSeq = calloc(binSeqSize, sizeof(char));
    size_t i = 0;
    while(hexSeq[i]) {
        strcat(binSeq, hex_to_bin(hexSeq[i]));
        i++;
    }
    binSeq[binSeqSize - 1] = '\0';
    return binSeq;
}

uint64_t bin_to_dec(char* bin /* must be null terminated */) {
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
    int ret = snprintf(0, 0, "%+ld", x) - 1;
    return ret;
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

void push(char*** stack, uint32_t* stackSize, char* elem) {
    *stack = realloc(*stack, ++(*stackSize) * sizeof(char*));
    (*stack)[*stackSize - 1] = strdup(elem);
}

char* pop(char*** stack, uint32_t* stackSize) {
    char* elem = (*stack)[*stackSize - 1];
    *stack = realloc(*stack, --(*stackSize) * sizeof(char*));
    if(*stackSize == 0) {
        *stack = NULL;
    }
    return elem;
}

uint64_t min(uint64_t a, uint64_t b) {
    return a < b ? a : b;
}

uint64_t max(uint64_t a, uint64_t b) {
    return a > b ? a : b;
}

char* currify(char* LVal, char* operator, char* RVal) {
    mpz_t newLVal;
    mpz_t decLVal;
    mpz_t decRVal;
    mpz_init(newLVal);
    mpz_init_set_str(decLVal, LVal, BASE10);
    mpz_init_set_str(decRVal, RVal, BASE10);
    if(is_sum(operator)) {
        mpz_add(newLVal, decLVal, decRVal);
    } else if(is_prod(operator)) {
        mpz_mul(newLVal, decLVal, decRVal);
    } else {
        int cmpVal = mpz_cmp(decLVal, decRVal);
        if(is_min(operator)) {
            if(cmpVal < 0)  mpz_set(newLVal, decLVal);
            else            mpz_set(newLVal, decRVal);
        } else if(is_max(operator)) {
            if(cmpVal > 0)  mpz_set(newLVal, decLVal);
            else            mpz_set(newLVal, decRVal);
        } else if(is_greater(operator)) {
            if(cmpVal > 0)  mpz_set_str(newLVal, "1", BASE10);
            else            mpz_set_str(newLVal, "0", BASE10);
        } else if(is_less(operator)) {
            if(cmpVal < 0)  mpz_set_str(newLVal, "1", BASE10);
            else            mpz_set_str(newLVal, "0", BASE10);
        } else if(is_equal(operator)) {
            if(cmpVal == 0) mpz_set_str(newLVal, "1", BASE10);
            else            mpz_set_str(newLVal, "0", BASE10);
        }
    }
    char* currified = mpz_get_str(NULL, BASE10, newLVal);
    mpz_clear(newLVal);
    mpz_clear(decLVal);
    mpz_clear(decRVal);
    return currified;
}

char* associated_symbol(uint8_t typeID) {
    switch(typeID) {
        case 0: return "sum";
        case 1: return "prod";
        case 2: return "min";
        case 3: return "max";
        case 4: return "literal";
        case 5: return "greater";
        case 6: return "less";
        case 7: return "equal";
    }
    return NULL;
}

t_packet* packet_create(uint8_t ver, uint8_t tid, char* val, char ltid) {
    t_packet* self = malloc(sizeof(t_packet));
    self->ver = ver;
    self->tid = associated_symbol(tid); /* no need to free */
    self->val = val;
    self->ltid = ltid;
    self->subPacketsSize = 0;
    self->subPackets = NULL;
    return self;
}

t_packet* decode_packet(char* binSeq, size_t* offset) {
    char* versionStr = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(char));
    char* typeIDStr = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(char));
    versionStr[HEADER_COMPONENT_SIZE] = '\0';
    typeIDStr[HEADER_COMPONENT_SIZE] = '\0';

    memcpy(versionStr, binSeq + *offset, HEADER_COMPONENT_SIZE);
    *offset += HEADER_COMPONENT_SIZE;
    memcpy(typeIDStr, binSeq + *offset, HEADER_COMPONENT_SIZE);
    *offset += HEADER_COMPONENT_SIZE;

    uint64_t value = 0;
    uint8_t version = bin_to_dec(versionStr);
    uint8_t typeID = bin_to_dec(typeIDStr);
    char ltid = '\0';

    if(typeID == LITERAL_PACKET) {
        uint64_t valueInBinSize = LITERAL_GROUP_SIZE + 1;
        char* valueInBin = calloc(valueInBinSize, sizeof(char));
        bool literalsEnd = false;
        while(!literalsEnd) {
            char prefix = binSeq[*offset];
            *offset += PREFIX_SIZE;
            strncat(valueInBin, binSeq + *offset, LITERAL_GROUP_SIZE);
            *offset += LITERAL_GROUP_SIZE;
            if(prefix == '0') {
                valueInBin[valueInBinSize - 1] = '\0';
                value = bin_to_dec(valueInBin);
                ltid = '-';
                literalsEnd = true;
            } else if(prefix == '1') {
                valueInBinSize += LITERAL_GROUP_SIZE;
                valueInBin = realloc(valueInBin, valueInBinSize);
            }
        }
        free(valueInBin);
    } else {
        ltid = binSeq[*offset];
        *offset += LENGTH_TYPE_ID_SIZE;
        uint64_t binSize = 0;
        if(ltid == '0') {
            binSize = SUBPACKETS_TOTAL_LENGTH_SIZE + 1;
        } else if(ltid == '1') {
            binSize = SUBPACKETS_NUMBER_SIZE + 1;
        }
        char* binAux = calloc(binSize, sizeof(char));
        memcpy(binAux, binSeq + *offset, binSize - 1);
        binAux[binSize - 1] = '\0';
        *offset += binSize - 1;
        value = bin_to_dec(binAux);
        free(binAux);
    }
    char* val = calloc(number_of_digits(value) + 1, sizeof(char));
    sprintf(val, "%ld", value);

    free(versionStr);
    free(typeIDStr);

    return packet_create(version, typeID, val, ltid);
}

bool is_literal_packet(t_packet* packet) {
    return strcmp(packet->tid, "literal") == 0;
}

bool all_subpackets_decoded(t_packet* parent, uint64_t processedSubPackets, uint64_t processedLength) {
    bool val = false;
    if(!is_literal_packet(parent)) {                            /* is operator packet */
        if(parent->ltid == '0') {                               /* total length in bits */
            val = processedLength >= atoi(parent->val);
        } else if(parent->ltid == '1') {                        /* number of sub-packets immediately contained */
            val = processedSubPackets >= atoi(parent->val);
        }
    }
    return val;
}

t_packet* decode_subpackets(t_packet* parent, char* binSeq, size_t* offset, uint64_t processedSubPackets, uint64_t* processedLength) {
    while(!all_subpackets_decoded(parent, processedSubPackets, *processedLength)) {
        size_t initialOffset = *offset;
        t_packet* child = decode_packet(binSeq, offset);
        size_t finalOffset = *offset;
        size_t diffOffset = finalOffset - initialOffset;
        *processedLength += diffOffset;
        processedSubPackets++;
        if(!is_literal_packet(child)) {
            uint64_t childProcessedLength = 0;
            uint64_t childProcessedSubPackets = 0;
            child = decode_subpackets(child, binSeq, offset, childProcessedSubPackets, &childProcessedLength);
            *processedLength += childProcessedLength;
            processedSubPackets += childProcessedSubPackets;
        }
        parent->subPackets = realloc(parent->subPackets, ++(parent->subPacketsSize) * sizeof(t_packet*));
        (parent->subPackets)[parent->subPacketsSize - 1] = child;
    }
    return parent;
}

void ast_print(t_packet* packet, uint32_t depth) {
    for(size_t i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("<Packet ver: %d, tid: %s, ltid: %c, val: %s, subpsize: %ld>\n", packet->ver, packet->tid, packet->ltid, packet->val, packet->subPacketsSize);
    for(size_t i = 0; i < packet->subPacketsSize; i++) {
        ast_print(packet->subPackets[i], depth + 1);
    }
}

/*
struct t_packet {
    uint8_t ver;
    char* tid;
    char* val;
    char ltid;
    uint64_t subPacketsSize;
    t_packet* subPackets;
};
*/

void packet_destroy(t_packet* self) {
    free(self->subPackets);
    free(self->val);
    free(self);
}

void ast_destroy(t_packet* packet) {
    for(size_t i = 0; i < packet->subPacketsSize; i++) {
        ast_destroy(packet->subPackets[i]);
    }
    packet_destroy(packet);
}

uint64_t solution(FILE* input) {
    size_t hexSeqSize = 0;
    char* hexSeq = get_hexSeq(input, &hexSeqSize);
    size_t binSeqSize = HEX_BIN_SIZE * (hexSeqSize - 1) + 1;
    char* binSeq = hexSeq_to_binSeq(hexSeq, binSeqSize);
    printf("Hex: %s\n", hexSeq);
    printf("Binary: %s\n", binSeq);

    size_t offset = 0;
    t_packet* outer = decode_packet(binSeq, &offset);
    uint64_t processedSubPackets = 0;
    uint64_t processedLength = 0;
    outer = decode_subpackets(outer, binSeq, &offset, processedSubPackets, &processedLength);

    uint32_t depth = 0;
    ast_print(outer, depth);

    free(hexSeq);
    free(binSeq);
    ast_destroy(outer);
    return 0;
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
