#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_COMMAND_LEN 8

bool is_forward(char* command) {
    return strcmp(command, "forward") == 0;
}

bool is_up(char* command) {
    return strcmp(command, "up") == 0;
}

bool is_down(char* command) {
    return strcmp(command, "down") == 0;
}

int solution(FILE* input) {
    char* command = calloc(MAX_COMMAND_LEN, sizeof(char));
    int value = 0;
    int xAxis = 0;
    int yAxis = 0;
    int aim = 0;
    while(fscanf(input, "%s %d", command, &value) != EOF) {
        if(is_forward(command)) {
            xAxis += value;
            yAxis += aim * value;
        } else if(is_up(command)) {
            aim -= value;
        } else if(is_down(command)) {
            aim += value;
        }
    }
    free(command);
    return xAxis * yAxis;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 1000"*/) {
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
