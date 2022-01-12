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

char* get_msg(FILE* input, size_t* msgSize) {
    char* msg = NULL;
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        msg = realloc(msg, ++(*msgSize) * sizeof(char));
        msg[*msgSize - 1] = c;
    }
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

bool is_last_packet(char* binSeq, size_t offset) {
    for(size_t i = offset; binSeq[i]; i++) {
        if(binSeq[i] == '1') return false;
    }
    return true;
}

uint64_t decode_sequence(char* binSeq) {
    char* version = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(char));
    char* typeIDBin = calloc(HEADER_COMPONENT_SIZE + 1, sizeof(char));
    version[HEADER_COMPONENT_SIZE] = '\0';
    typeIDBin[HEADER_COMPONENT_SIZE] = '\0';
    size_t offset = 0;
    uint64_t sumOfVersions = 0;
    do {
        printf("\n");
        memcpy(version, binSeq + offset, HEADER_COMPONENT_SIZE);
        offset += HEADER_COMPONENT_SIZE;
        sumOfVersions += bin_to_dec(version);
        printf("Version: %ld\n", bin_to_dec(version));
        memcpy(typeIDBin, binSeq + offset, HEADER_COMPONENT_SIZE);
        offset += HEADER_COMPONENT_SIZE;
        uint8_t typeIDDec = bin_to_dec(typeIDBin);
        printf("Type ID: %d\n", typeIDDec);
        if(typeIDDec == LITERAL_PACKET) {
            size_t valueInBinSize = LITERAL_GROUP_SIZE + 1;
            char* valueInBin = calloc(valueInBinSize, sizeof(char));
            uint64_t literalValue = 0;
            bool literalsEnd = false;
            while(!literalsEnd) {
                char prefix = binSeq[offset];
                offset += PREFIX_SIZE;
                strncat(valueInBin, binSeq + offset, LITERAL_GROUP_SIZE);
                offset += LITERAL_GROUP_SIZE;
                if(prefix == '0') {
                    literalsEnd = true;
                    valueInBin[valueInBinSize - 1] = '\0';
                    literalValue = bin_to_dec(valueInBin);
                    printf("Literal value: %ld\n", literalValue);
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
            size_t binSize = 0;
            char* binAux = NULL;
            if(lengthTypeID == '0') {
                binSize = SUBPACKETS_TOTAL_LENGTH_SIZE + 1;
                binAux = calloc(binSize, sizeof(char));
            } else if(lengthTypeID == '1') {
                binSize = SUBPACKETS_NUMBER_SIZE + 1;
                binAux = calloc(binSize, sizeof(char));
            }
            memcpy(binAux, binSeq + offset, binSize);
            binAux[binSize - 1] = '\0';
            offset += binSize - 1;
            uint64_t dec = bin_to_dec(binAux);
            printf("Value: %ld\n", dec);
            free(binAux);
        }
    } while(!is_last_packet(binSeq, offset));
    free(version);
    free(typeIDBin);
    return sumOfVersions;
}

uint64_t solution(FILE* input) {
    size_t hexSeqSize = 0;
    char* hexSeq = get_msg(input, &hexSeqSize);
    printf("Hex: %s\n", hexSeq);
    char* binSeq = hexSeq_to_binSeq(hexSeq, hexSeqSize);
    printf("Binary: %s\n", binSeq);
    uint64_t sum = decode_sequence(binSeq);
    free(hexSeq);
    free(binSeq);
    return sum;
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
