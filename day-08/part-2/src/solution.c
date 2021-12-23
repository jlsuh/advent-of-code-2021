#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define NUMBER_OF_SEGMENTS 7

typedef struct {
    char* segmentsConfig;
    char** uniqueSequence;
    int* uniqueSequenceNumbers;
    char** digitOutputValue;
    int* digitOutputValueNumbers;
} t_entry;

void print_entry(t_entry* entry, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    printf("Unique sequence:\n");
    for (int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        printf("%s\t", entry->uniqueSequence[i]);
    }
    printf("\n");
    for(int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        printf("%d\t", entry->uniqueSequenceNumbers[i]);
    }
    printf("\n");
    printf("Digit output value:\n");
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        printf("%s\t", entry->digitOutputValue[i]);
    }
    printf("\n");
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        printf("%d\t", entry->digitOutputValueNumbers[i]);
    }
    printf("\n");
    printf(" %c%c%c%c \n", entry->segmentsConfig[0], entry->segmentsConfig[0], entry->segmentsConfig[0], entry->segmentsConfig[0]);
    printf("%c    %c\n", entry->segmentsConfig[5], entry->segmentsConfig[1]);
    printf("%c    %c\n", entry->segmentsConfig[5], entry->segmentsConfig[1]);
    printf(" %c%c%c%c \n", entry->segmentsConfig[6], entry->segmentsConfig[6], entry->segmentsConfig[6], entry->segmentsConfig[6]);
    printf("%c    %c\n", entry->segmentsConfig[4], entry->segmentsConfig[2]);
    printf("%c    %c\n", entry->segmentsConfig[4], entry->segmentsConfig[2]);
    printf(" %c%c%c%c \n", entry->segmentsConfig[3], entry->segmentsConfig[3], entry->segmentsConfig[3], entry->segmentsConfig[3]);
}

void print_entries(t_entry** entries, int numberOfEntries, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfEntries; i++) {
        printf("Extracting entry %d\n", i + 1);
        print_entry(entries[i], numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
        printf("\n\n");
    }
}

t_entry* entry_create(int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    t_entry* self = malloc(sizeof(t_entry));
    self->digitOutputValueNumbers = calloc(numberOfDigitsInDisplay, sizeof(int));
    memset(self->digitOutputValueNumbers, -1, numberOfDigitsInDisplay * sizeof(int));
    self->digitOutputValue = calloc(numberOfDigitsInDisplay, sizeof(char*));
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        self->digitOutputValue[i] = calloc(NUMBER_OF_SEGMENTS + 1, sizeof(char));
    }
    self->uniqueSequenceNumbers = calloc(numberOfUniqueSignalPatterns, sizeof(int));
    memset(self->uniqueSequenceNumbers, -1, numberOfUniqueSignalPatterns * sizeof(int));
    self->uniqueSequence = calloc(numberOfUniqueSignalPatterns, sizeof(char*));
    for(int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        self->uniqueSequence[i] = calloc(NUMBER_OF_SEGMENTS + 1, sizeof(char));
    }
    self->segmentsConfig = calloc(NUMBER_OF_SEGMENTS + 1, sizeof(char));
    memset(self->segmentsConfig, '.', NUMBER_OF_SEGMENTS * sizeof(char));
    return self;
}

void entry_destroy(t_entry* self, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        free(self->digitOutputValue[i]);
    }
    for(int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        free(self->uniqueSequence[i]);
    }
    free(self->digitOutputValueNumbers);
    free(self->digitOutputValue);
    free(self->uniqueSequenceNumbers);
    free(self->uniqueSequence);
    free(self->segmentsConfig);
    free(self);
}

void entries_destroy(t_entry** entries, int numberOfEntries, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfEntries; i++) {
        entry_destroy(entries[i], numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
    }
    free(entries);
}

int get_number_of_entries(FILE* input) {
    int numberOfEntries = 0;
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        if(c == '|') {
            numberOfEntries++;
        }
    }
    rewind(input);
    return numberOfEntries;
}

t_entry* extract_entry(FILE* input, t_entry* entry, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    char* sequence = calloc(NUMBER_OF_SEGMENTS + 1, sizeof(char));
    for(int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        fscanf(input, "%s ", sequence);
        strcpy(entry->uniqueSequence[i], sequence);
    }
    fscanf(input, "| ");
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        fscanf(input, "%s ", sequence);
        strcpy(entry->digitOutputValue[i], sequence);
    }
    free(sequence);
    return entry;
}

bool is_unique_number_of_segments(int numberOfActiveSegments) {
    return numberOfActiveSegments == 2 ||
            numberOfActiveSegments == 3 ||
            numberOfActiveSegments == 4 ||
            numberOfActiveSegments == 7;
}

int easy_digit(int numberOfActiveSegments) {
    switch(numberOfActiveSegments) {
    case 2:
        return 1;
    case 3:
        return 7;
    case 4:
        return 4;
    case 7:
        return 8;
    }
    return -1;
}

t_entry** set_initial_easy_digits(t_entry** entries, int numberOfEntries, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfEntries; i++) {
        t_entry* entry = entries[i];
        for(int j = 0; j < numberOfUniqueSignalPatterns + numberOfDigitsInDisplay; j++) {
            char* digit = NULL;
            int* array = NULL;
            int index = -1;
            if(j < numberOfUniqueSignalPatterns) {
                array = entry->uniqueSequenceNumbers;
                index = j;
                digit = entry->uniqueSequence[index];
            } else {
                array = entry->digitOutputValueNumbers;
                index = j - numberOfUniqueSignalPatterns;
                digit = entry->digitOutputValue[index];
            }
            int numberOfActiveSegments = strlen(digit);
            if(is_unique_number_of_segments(numberOfActiveSegments)) {
                array[index] = easy_digit(numberOfActiveSegments);
            }
        }
    }
    return entries;
}

bool display_digits_already_decoded(t_entry* entry, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        if(entry->digitOutputValueNumbers[i] == -1) {
            return false;
        }
    }
    return true;
}

char* associated_sequence_of_digit(t_entry* entry, int currDigit, int numberOfUniqueSignalPatterns) {
    for(int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        if(entry->uniqueSequenceNumbers[i] == currDigit) {
            return entry->uniqueSequence[i];
        }
    }
    return NULL;
}

bool str_contains_char(char* str, char c) {
    for(int i = 0; i < strlen(str); i++) {
        if(str[i] == c) {
            return true;
        }
    }
    return false;
}

char* complement_of_intersection(char* str, char* seqToTrim, int size) {
    char* sequenceAux = calloc(size + 1, sizeof(char));
    int j = 0;
    for(int i = 0; i < strlen(seqToTrim); i++) {
        char charSeq = seqToTrim[i];
        if(charSeq != '.') {
            if(!str_contains_char(str, charSeq)) {
                sequenceAux[j] = charSeq;
                j++;
            }
        }
    }
    return sequenceAux;
}

char* get_minimum_trimmed_sequence_by_size(t_entry* entry, int expectedOriginalSize, int expectedSizeAfterTrim, int numberOfUniqueSignalPatterns) {
    for(int i = 0; i < numberOfUniqueSignalPatterns; i++) {
        char* sequence = entry->uniqueSequence[i];
        if(strlen(sequence) == expectedOriginalSize) {
            char* compl = complement_of_intersection(entry->segmentsConfig, sequence, expectedOriginalSize);
            if(strlen(compl) == expectedSizeAfterTrim) {
                return compl;
            }
            free(compl);
        }
    }
    return NULL;
}

char get_first_intersection_char(char* str1, char* str2) {
    char c = '\0';
    for(int i = 0; i < strlen(str1); i++) {
        for(int j = 0; j < strlen(str2); j++) {
            if(str1[i] == str2[j]) {
                c = str1[i];
            }
        }
    }
    return c;
}

int register_segments_in_config(t_entry* entry, char* sequence, int currDigit, int numberOfUniqueSignalPatterns) {
    int indexes[NUMBER_OF_SEGMENTS] = {-1};
    int until = -1;
    int nextDigit = -1;
    char* sequenceAux = NULL;
    bool shouldFree = false;
    if(currDigit == 1) {
        indexes[0] = 1;
        indexes[1] = 2;
        until = 2;
        nextDigit = 7;
        sequenceAux = sequence;
    } else if(currDigit == 7) {
        indexes[0] = 0;
        until = 1;
        nextDigit = 4;
        sequenceAux = complement_of_intersection(entry->segmentsConfig, sequence, until);
        shouldFree = true;
    } else if(currDigit == 4) {
        until = 3;
        char* trimmedFour = complement_of_intersection(entry->segmentsConfig, sequence, until);
        char* trimmedThree = get_minimum_trimmed_sequence_by_size(entry, 5, until - 1, numberOfUniqueSignalPatterns);
        char intersectionChar = get_first_intersection_char(trimmedFour, trimmedThree); /* This is the 6th position char */
        char buffer[2] = {'\0'};
        sprintf(buffer, "%c", intersectionChar);
        char* trimmedFourIntersectionCompl = complement_of_intersection(buffer, trimmedFour, until);
        char* trimmedThreeIntersectionCompl = complement_of_intersection(buffer, trimmedThree, until);
        sequenceAux = calloc(5, sizeof(char));
        sprintf(sequenceAux, "%s%s%c", trimmedThreeIntersectionCompl, trimmedFourIntersectionCompl, intersectionChar);
        indexes[0] = 3;
        indexes[1] = 5;
        indexes[2] = 6;
        nextDigit = -1;
        shouldFree = true;
        free(trimmedFour);
        free(trimmedThree);
        free(trimmedFourIntersectionCompl);
        free(trimmedThreeIntersectionCompl);
    } else if(currDigit == -1) {
        until = 1;
        sequenceAux = complement_of_intersection(entry->segmentsConfig, "abcdefg", until);
        indexes[0] = 4;
        nextDigit = -1;
        shouldFree = true;
    }
    for(int i = 0; i < until; i++) {
        entry->segmentsConfig[indexes[i]] = sequenceAux[i];
    }
    if(shouldFree) {
        free(sequenceAux);
    }
    return nextDigit;
}

bool defined_config(char* config) {
    for(int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
        if(config[i] == '.') {
            return false;
        }
    }
    return true;
}

t_entry** build_entries_segments_config(t_entry** entries, int numberOfEntries, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfEntries; i++) {
        t_entry* entry = entries[i];
        if(!display_digits_already_decoded(entry, numberOfDigitsInDisplay)) {
            int currDigit = 1;
            while(!defined_config(entry->segmentsConfig)) {
                char* sequence = associated_sequence_of_digit(entry, currDigit, numberOfUniqueSignalPatterns);
                currDigit = register_segments_in_config(entry, sequence, currDigit, numberOfUniqueSignalPatterns);
            }
        }
    }
    return entries;
}

int get_index_of_segment(char* str, char c) {
    int index = 0;
    while(index < strlen(str)) {
        if(str[index] == c) {
            return index;
        }
        index++;
    }
    return -1;
}

int number_of_active_segments(bool* segments) {
    int count = 0;
    for(int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
        if(segments[i]) {
            count++;
        }
    }
    return count;
}

int number_based_from_active_segments(t_entry* entry, bool* segments) {
    for(int i = 0; i < 2; i++) {
        if(number_of_active_segments(segments) == 5) {
            if(segments[4]) {
                return 2;
            } else if(segments[2]) {
                if(segments[1]) {
                    return 3;
                }
                return 5;
            }
        } else if(number_of_active_segments(segments) == 6) {
            if(!segments[6]) {
                return 0;
            } else {
                if(segments[4]) {
                    return 6;
                }
                return 9;
            }
        }
        /* Switch position 1<->2 in case of wrong order on number "1" */
        if(i == 0) {
            if(segments[1]) {
                segments[1] = false;
                segments[2] = true;
            } else if(segments[2]) {
                segments[1] = true;
                segments[2] = false;
            }
        }
    }
    return -1;
}

t_entry** decode_numbers(t_entry** entries, int numberOfEntries, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfEntries; i++) {
        t_entry* entry = entries[i];
        if(!display_digits_already_decoded(entry, numberOfDigitsInDisplay)) {
            for(int j = 0; j < numberOfUniqueSignalPatterns + numberOfDigitsInDisplay; j++) {
                int elemIndex = -1;
                int* arrayOfNumbers = NULL;
                char** arrayOfStrings = NULL;
                if(j < numberOfUniqueSignalPatterns) {
                    elemIndex = j;
                    arrayOfNumbers = entry->uniqueSequenceNumbers;
                    arrayOfStrings = entry->uniqueSequence;
                } else {
                    elemIndex = j - numberOfUniqueSignalPatterns;
                    arrayOfNumbers = entry->digitOutputValueNumbers;
                    arrayOfStrings = entry->digitOutputValue;
                }
                if(arrayOfNumbers[elemIndex] == -1) {
                    bool* segments = calloc(NUMBER_OF_SEGMENTS, sizeof(bool));
                    char* str = arrayOfStrings[elemIndex];
                    for(int k = 0; k < strlen(str); k++) {
                        char c = str[k];
                        int indexOfSegm = get_index_of_segment(entry->segmentsConfig, c);
                        segments[indexOfSegm] = true;
                    }
                    arrayOfNumbers[elemIndex] = number_based_from_active_segments(entry, segments);
                    free(segments);
                }
            }
        }
    }
    return entries;
}

int arrayToInt(int* array, int size) {
    int arrayIntForm = 0;
    for (int i = 0; i < size; i++) {
        arrayIntForm = 10 * arrayIntForm + array[i];
    }
    return arrayIntForm;
}

int sum_of_digit_output_values(t_entry** entries, int numberOfEntries, int numberOfDigitsInDisplay) {
    int sum = 0;
    for(int i = 0; i < numberOfEntries; i++) {
        sum += arrayToInt(entries[i]->digitOutputValueNumbers, numberOfDigitsInDisplay);
    }
    return sum;
}

int solution(FILE* input, int numberOfUniqueSignalPatterns, int numberOfDigitsInDisplay) {
    int numberOfEntries = get_number_of_entries(input);
    t_entry** entries = calloc(numberOfEntries, sizeof(t_entry*));
    for(int i = 0; i < numberOfEntries; i++) {
        entries[i] = entry_create(numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
        entries[i] = extract_entry(input, entries[i], numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
    }

    entries = set_initial_easy_digits(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
    /* puts("------------------------ After set_initial_easy_digits ------------------------");
    print_entries(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay); */

    entries = build_entries_segments_config(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
    /* puts("------------------------ After build_entries_segments_config ------------------------");
    print_entries(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay); */

    entries = decode_numbers(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
    /* puts("------------------------ After decode_numbers ------------------------");
    print_entries(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay); */

    int totalSum = sum_of_digit_output_values(entries, numberOfEntries, numberOfDigitsInDisplay);
    entries_destroy(entries, numberOfEntries, numberOfUniqueSignalPatterns, numberOfDigitsInDisplay);
    return totalSum;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 10 4"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input, atoi(argv[2]), atoi(argv[3]));
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
