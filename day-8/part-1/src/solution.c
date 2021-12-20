#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define NUMBER_OF_SEGMENTS 7

typedef struct {
    bool** digitOutputValue;
} t_entry;

t_entry* entry_create(int numberOfDigitsInDisplay) {
    t_entry* self = malloc(sizeof(t_entry));
    self->digitOutputValue = calloc(numberOfDigitsInDisplay, sizeof(bool*));
    for (int i = 0; i < numberOfDigitsInDisplay; i++) {
        self->digitOutputValue[i] = calloc(NUMBER_OF_SEGMENTS, sizeof(bool));
    }
    return self;
}

void entry_destroy(t_entry* self, int numberOfDigitsInDisplay) {
    for (int i = 0; i < numberOfDigitsInDisplay; i++) {
        free(self->digitOutputValue[i]);
    }
    free(self->digitOutputValue);
    free(self);
}

int get_number_of_entries(FILE* input) {
    int numberOfEntries = 0;
    char c = '\0';
    while ((c = fgetc(input)) != EOF) {
        if(c == '|') {
            numberOfEntries++;
        }
    }
    rewind(input);
    return numberOfEntries;
}

void print_entry(t_entry* entry, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        for (int j = 0; j < NUMBER_OF_SEGMENTS; j++) {
            printf("%d ", entry->digitOutputValue[i][j]);
        }
        printf("\n");
    }
}

void print_entries(t_entry** entries, int numberOfDigitsInDisplay, int numberOfEntries) {
    for(int i = 0; i < numberOfEntries; i++) {
        print_entry(entries[i], numberOfDigitsInDisplay);
        printf("\n");
    }
}

int get_segment_index(char c) {
    switch(c) {
        case 'a':
            return 0;
        case 'b':
            return 1;
        case 'c':
            return 2;
        case 'd':
            return 3;
        case 'e':
            return 4;
        case 'f':
            return 5;
        case 'g':
            return 6;
    }
    return -1;
}

t_entry* extract_entry(FILE* input, t_entry* entry, int numberOfSignalPatterns, int numberOfDigitsInDisplay) {
    char* sequence = calloc(NUMBER_OF_SEGMENTS + 1, sizeof(char));
    for(int i = 0; i < numberOfSignalPatterns; i++) {
        fscanf(input, "%s ", sequence);
    }
    fscanf(input, "| ");
    for(int i = 0; i < numberOfDigitsInDisplay; i++) {
        fscanf(input, "%s ", sequence);
        for(int j = 0; isalpha(sequence[j]); j++) {
            int segmentIndex = get_segment_index(sequence[j]);
            entry->digitOutputValue[i][segmentIndex] = true;
        }
    }
    free(sequence);
    return entry;
}

void entries_destroy(t_entry** entries, int numberOfEntries, int numberOfDigitsInDisplay) {
    for(int i = 0; i < numberOfEntries; i++) {
        entry_destroy(entries[i], numberOfDigitsInDisplay);
    }
    free(entries);
}

bool is_unique_number_of_segments(int numberOfActiveSegments) {
    return numberOfActiveSegments == 2 ||
            numberOfActiveSegments == 3 ||
            numberOfActiveSegments == 4 ||
            numberOfActiveSegments == 7;
}

int get_number_of_active_segments(bool* digit) {
    int numberOfActiveSegments = 0;
    for(int i = 0; i < NUMBER_OF_SEGMENTS; i++) {
        if(digit[i]) {
            numberOfActiveSegments++;
        }
    }
    return numberOfActiveSegments;
}

int easy_digits_ocurrences(t_entry** entries, int numberOfEntries, int numberOfDigitsInDisplay) {
    int ocurrences = 0;
    for(int i = 0; i < numberOfEntries; i++) {
        t_entry* entry = entries[i];
        for(int j = 0; j < numberOfDigitsInDisplay; j++) {
            bool* digit = entry->digitOutputValue[j];
            int numberOfActiveSegments = get_number_of_active_segments(digit);
            if(is_unique_number_of_segments(numberOfActiveSegments)) {
                ocurrences++;
            }
        }
    }
    return ocurrences;
}

int solution(FILE* input, int numberOfSignalPatterns, int numberOfDigitsInDisplay) {
    int numberOfEntries = get_number_of_entries(input);
    t_entry** entries = calloc(numberOfEntries, sizeof(t_entry*));
    for (int i = 0; i < numberOfEntries; i++) {
        entries[i] = entry_create(numberOfDigitsInDisplay);
        entries[i] = extract_entry(input, entries[i], numberOfSignalPatterns, numberOfDigitsInDisplay);
    }
    int easyDigits = easy_digits_ocurrences(entries, numberOfEntries, numberOfDigitsInDisplay);

    entries_destroy(entries, numberOfEntries, numberOfDigitsInDisplay);
    return easyDigits;
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
