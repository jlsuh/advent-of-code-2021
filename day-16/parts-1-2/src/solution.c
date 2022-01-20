#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <gmp.h>

void _(void) {
    char const * _[] = {
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
    for(int i = 0; i < 14; i += 1) {
        printf("%s\n", _[i]);
    }
}

#define LENGTH_TYPE_ID_SIZE 1
#define PREFIX_SIZE 1
#define HEADER_COMPONENT_SIZE 3
#define HEX_BIN_SIZE 4
#define LITERAL_GROUP_SIZE 4
#define BASE10 10
#define SUBPACKETS_NUMBER_SIZE 11
#define SUBPACKETS_TOTAL_LENGTH_SIZE 15

typedef enum {
    PACKET_TYPE_sum,
    PACKET_TYPE_product,
    PACKET_TYPE_minimum,
    PACKET_TYPE_maximum,
    PACKET_TYPE_literal,
    PACKET_TYPE_greater_than,
    PACKET_TYPE_less_than,
    PACKET_TYPE_equal_to
} t_packet_type;

typedef struct t_packet t_packet;
struct t_packet {
    uint8_t ver;
    uint8_t tid;
    char* currified;
    char* val;
    char ltid;
    uint64_t subPacketsSize;
    t_packet** subPackets;
};

bool is_sum(t_packet_type tid) { return tid == PACKET_TYPE_sum; }
bool is_prod(t_packet_type tid) { return tid == PACKET_TYPE_product; }
bool is_min(t_packet_type tid) { return tid == PACKET_TYPE_minimum; }
bool is_max(t_packet_type tid) { return tid == PACKET_TYPE_maximum; }
bool is_literal(t_packet_type tid) { return tid == PACKET_TYPE_literal; }
bool is_greater(t_packet_type tid) { return tid == PACKET_TYPE_greater_than; }
bool is_less(t_packet_type tid) { return tid == PACKET_TYPE_less_than; }
bool is_equal(t_packet_type tid) { return tid == PACKET_TYPE_equal_to; }
bool is_operator(t_packet_type tid) {
    return is_sum(tid) || is_prod(tid) || is_min(tid) || is_max(tid) || is_greater(tid) || is_less(tid) || is_equal(tid);
}

bool is_total_length_ltid(char ltid) { return ltid == '0'; }
bool is_number_of_subpackets_ltid(char ltid) { return ltid == '1'; }

bool all_subpackets_decoded(t_packet* parent, uint64_t processedSubPackets, uint64_t processedLength) {
    bool val = false;
    if(!is_literal(parent->tid)) {
        if(is_total_length_ltid(parent->ltid)) {                    /* total length in bits                         */
            val = processedLength >= atoi(parent->val);
        } else if(is_number_of_subpackets_ltid(parent->ltid)) {     /* number of sub-packets immediately contained  */
            val = processedSubPackets >= atoi(parent->val);
        }
    }
    return val;
}

uint64_t number_of_digits(uint64_t x) { return snprintf(0, 0, "%+ld", x) - 1; }

const char* hex_to_bin(char hex) {
    return hex == '0' ? "0000"
         : hex == '1' ? "0001"
         : hex == '2' ? "0010"
         : hex == '3' ? "0011"
         : hex == '4' ? "0100"
         : hex == '5' ? "0101"
         : hex == '6' ? "0110"
         : hex == '7' ? "0111"
         : hex == '8' ? "1000"
         : hex == '9' ? "1001"
         : hex == 'A' ? "1010"
         : hex == 'B' ? "1011"
         : hex == 'C' ? "1100"
         : hex == 'D' ? "1101"
         : hex == 'E' ? "1110"
         : hex == 'F' ? "1111"
         : NULL;
}

uint64_t bin_to_dec(char* bin /* must be null terminated */) {
    uint64_t dec = 0;
    for(size_t i = 0; bin[i]; i += 1) {
        if(bin[i] == '1') {
            dec = dec * 2 + 1;
        } else if(bin[i] == '0') {
            dec *= 2;
        }
    }
    return dec;
}

char* get_hexSeq(FILE* input, size_t* hexSeqSize) {
    *hexSeqSize += 1;
    char* hex = calloc(*hexSeqSize, sizeof(*hex));
    char c = fgetc(input);
    while(c != EOF && c != '\n') {
        hex[*hexSeqSize - 1] = c;
        *hexSeqSize += 1;
        hex = realloc(hex, *hexSeqSize * sizeof(*hex));
        c = fgetc(input);
    }
    hex[*hexSeqSize - 1] = '\0';
    rewind(input);
    return hex;
}

char* hexSeq_to_binSeq(char* hexSeq, size_t binSeqSize) {
    char* binSeq = calloc(binSeqSize, sizeof(*binSeq));
    for(size_t i = 0; hexSeq[i]; i += 1) {
        strcat(binSeq, hex_to_bin(hexSeq[i]));
    }
    binSeq[binSeqSize - 1] = '\0';
    return binSeq;
}

void packet_destroy(t_packet* self) {
    free(self->subPackets);
    free(self->val);
    free(self->currified);
    free(self);
}

t_packet* packet_create(uint8_t ver, uint8_t tid, char* val, char ltid) {
    t_packet* self = malloc(sizeof(*self));
    self->ver = ver;
    self->tid = tid;
    self->val = val;
    self->ltid = ltid;
    self->subPacketsSize = 0;
    self->subPackets = NULL;
    if(!is_literal(self->tid)) {
        self->currified = NULL;
    } else {
        self->currified = strdup(val);
    }
    return self;
}

char* currify(char* LVal, uint8_t operator, char* RVal) {
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

t_packet* decode_packet(char* binSeq, size_t* offset, uint64_t* versionSum) {
    char* versionStr = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(*versionStr));
    char* typeIDStr = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(*typeIDStr));
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

    *versionSum += version;

    if(is_literal(typeID)) {
        uint64_t valueInBinSize = LITERAL_GROUP_SIZE + 1;
        char* valueInBin = calloc(valueInBinSize, sizeof(*valueInBin));
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
        if(is_total_length_ltid(ltid)) {
            binSize = SUBPACKETS_TOTAL_LENGTH_SIZE + 1;
        } else if(is_number_of_subpackets_ltid(ltid)) {
            binSize = SUBPACKETS_NUMBER_SIZE + 1;
        }
        char* binAux = calloc(binSize, sizeof(*binAux));
        memcpy(binAux, binSeq + *offset, binSize - 1);
        binAux[binSize - 1] = '\0';
        *offset += binSize - 1;
        value = bin_to_dec(binAux);
        free(binAux);
    }
    char* val = calloc(number_of_digits(value) + 1, sizeof(*val));
    sprintf(val, "%ld", value);

    free(versionStr);
    free(typeIDStr);

    return packet_create(version, typeID, val, ltid);
}

void decode_subpackets(t_packet* parent, char* binSeq, size_t* offset, uint64_t processedSubPackets, uint64_t* processedLength, uint64_t* versionSum) {
    while(!all_subpackets_decoded(parent, processedSubPackets, *processedLength)) {
        size_t initialOffset = *offset;
        t_packet* child = decode_packet(binSeq, offset, versionSum);
        size_t finalOffset = *offset;
        size_t diffOffset = finalOffset - initialOffset;
        *processedLength += diffOffset;
        processedSubPackets += 1;
        if(!is_literal(child->tid)) {
            uint64_t childProcessedLength = 0;
            uint64_t childProcessedSubPackets = 0;
            decode_subpackets(child, binSeq, offset, childProcessedSubPackets, &childProcessedLength, versionSum);
            *processedLength += childProcessedLength;
            processedSubPackets += childProcessedSubPackets;
        }
        parent->subPacketsSize += 1;
        parent->subPackets = realloc(parent->subPackets, parent->subPacketsSize * sizeof(**(parent->subPackets)));
        (parent->subPackets)[parent->subPacketsSize - 1] = child;
    }
}

void push(char*** stack, uint32_t* stackSize, char* elem) {
    *stackSize += 1;
    *stack = realloc(*stack, *stackSize * sizeof(**stack));
    (*stack)[*stackSize - 1] = strdup(elem);
}

char* pop(char*** stack, uint32_t* stackSize) {
    char* elem = (*stack)[*stackSize - 1];
    *stackSize -= 1;
    *stack = realloc(*stack, *stackSize * sizeof(**stack));
    if(*stackSize == 0) {
        *stack = NULL;
    }
    return elem;
}

void ast_print(t_packet* packet, uint32_t depth) {
    for(size_t i = 0; i < depth; i += 1) {
        printf("  ");
    }
    printf(
        "<Packet ver: %d, tid: %d, ltid: %c, val: %s, subpsize: %ld, currified: %s>\n",
        packet->ver,
        packet->tid,
        packet->ltid,
        packet->val,
        packet->subPacketsSize,
        packet->currified
    );
    for(size_t i = 0; i < packet->subPacketsSize; i += 1) {
        ast_print(packet->subPackets[i], depth + 1);
    }
}

void ast_destroy(t_packet* packet) {
    for(size_t i = 0; i < packet->subPacketsSize; i += 1) {
        ast_destroy(packet->subPackets[i]);
    }
    packet_destroy(packet);
}

void ast_eval(t_packet* packet) {
    for(size_t i = 0; i < packet->subPacketsSize; i += 1) {
        if(!is_literal(packet->subPackets[i]->tid)) {
            ast_eval(packet->subPackets[i]);
        }
    }
    uint32_t stackSize = 0;
    char** stack = NULL;
    for(size_t i = packet->subPacketsSize; i > 0; i -= 1) {
        push(&stack, &stackSize, packet->subPackets[i - 1]->currified);
    }
    while(stackSize > 1) {
        char* LVal = pop(&stack, &stackSize);
        char* RVal = pop(&stack, &stackSize);
        char* currified = currify(LVal, packet->tid, RVal);
        push(&stack, &stackSize, currified);
        free(LVal);
        free(RVal);
        free(currified);
    }
    packet->currified = pop(&stack, &stackSize);
}

uint64_t solution(FILE* input) {
    size_t hexSeqSize = 0;
    char* hexSeq = get_hexSeq(input, &hexSeqSize);
    size_t binSeqSize = HEX_BIN_SIZE * (hexSeqSize - 1) + 1;
    char* binSeq = hexSeq_to_binSeq(hexSeq, binSeqSize);
    printf("Hex: %s\n\n", hexSeq);
    printf("Binary: %s\n\n", binSeq);

    uint64_t versionSum = 0;
    size_t offset = 0;
    t_packet* outer = decode_packet(binSeq, &offset, &versionSum);
    decode_subpackets(outer, binSeq, &offset, (uint64_t) {0}, &(uint64_t) {0}, &versionSum);

    printf("Before eval:\n");
    ast_print(outer, (uint64_t) {0});
    printf("\n==================================================================================================\n\n");

    ast_eval(outer);

    printf("After eval:\n");
    ast_print(outer, (uint64_t) {0});
    printf("\n");

    uint64_t eval = atoll(outer->currified);

    free(hexSeq);
    free(binSeq);
    ast_destroy(outer);

    printf("Version sum: %ld\n", versionSum);
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
    _();
    return 0;
}
