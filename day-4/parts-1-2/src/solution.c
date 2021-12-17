#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    bool** markGrid;
    int** matrix;
    int drawsUntilWin;
} t_matrix_analysis;

void skip_newlines_until_next_matrix(FILE* input) {
    char c = fgetc(input);
    while(c == '\n') {
        c = fgetc(input);
    }
    ungetc(c, input);
}

int* get_draw_sequence(FILE* input, int drawSequenceSize) {
    int* drawSequence = malloc(sizeof(int) * drawSequenceSize);
    int i = 0;
    fscanf(input, "%d", &drawSequence[i++]);
    while(i < drawSequenceSize) {
        fscanf(input, ",%d", &drawSequence[i]);
        i++;
    }
    return drawSequence;
}

int get_draw_sequence_size(FILE* input) {
    int drawSequenceSize = 1;
    char c = '\0';
    while((c = fgetc(input)) != EOF) {
        if(c == ',') {
            drawSequenceSize++;
        }
    }
    rewind(input);
    return drawSequenceSize;
}

bool is_bingo(int squaredDim, bool** markGrid) {
    int horizontalCounter = 0;
    int verticalCounter = 0;
    for(int i = 0; i < squaredDim; i++) {
        for(int j = 0; j < squaredDim; j++) {
            if(markGrid[j][i]) {
                horizontalCounter++;
            }
            if(markGrid[i][j]) {
                verticalCounter++;
            }
        }
        if(horizontalCounter == squaredDim || verticalCounter == squaredDim) {
            return true;
        }
        horizontalCounter = 0;
        verticalCounter = 0;
    }
    return false;
}

t_matrix_analysis get_matrix_analysis_until_win(int squaredDim, int** matrix, int* drawSequence, int drawSequenceSize) {
    t_matrix_analysis matrixAnalysis;

    bool** markGrid = calloc(squaredDim * squaredDim, sizeof(bool*));
    for(int i = 0; i < squaredDim; i++) {
        markGrid[i] = calloc(squaredDim, sizeof(bool));
    }

    int drawIndex = 0;
    while(drawIndex < drawSequenceSize) {
        bool found = false;
        for(int i = 0; i < squaredDim && !found; i++) {
            for(int j = 0; j < squaredDim && !found; j++) {
                if(drawSequence[drawIndex] == matrix[j][i]) {
                    markGrid[j][i] = true;
                    found = true;
                }
            }
        }
        drawIndex++;
        if(found) {
            if(is_bingo(squaredDim, markGrid)) {
                break;
            }
        }
    }

    matrixAnalysis.drawsUntilWin = drawIndex;
    matrixAnalysis.markGrid = markGrid;
    matrixAnalysis.matrix = matrix;

    return matrixAnalysis;
}

t_matrix_analysis draws_matrix(t_matrix_analysis* matrixAnalysisAllocator, int matrixAnalysisAllocatorSize,
                                bool(*condition)(int drawsUntilWin1, int drawsUntilWin2)) {
    t_matrix_analysis matrixAnalysisAux = matrixAnalysisAllocator[0];
    for(int i = 1; i < matrixAnalysisAllocatorSize - 1; i++) {
        if(condition(matrixAnalysisAllocator[i].drawsUntilWin, matrixAnalysisAux.drawsUntilWin)) {
            matrixAnalysisAux = matrixAnalysisAllocator[i];
        }
    }
    return matrixAnalysisAux;
}

bool maximize_draws(int drawsUntilWin1, int drawsUntilWin2) {
    return drawsUntilWin1 > drawsUntilWin2;
}

bool minimize_draws(int drawsUntilWin1, int drawsUntilWin2) {
    return drawsUntilWin1 < drawsUntilWin2;
}

bool is_octo_lose(char* outcome) {
    return strcmp(outcome, "OCTOLOSE") == 0;
}

bool is_octo_win(char* outcome) {
    return strcmp(outcome, "OCTOWIN") == 0;
}

int get_score(t_matrix_analysis drawsMatrix, int* drawSequence, int squaredDim) {
    int unmarkedSum = 0;
    for(int i = 0; i < squaredDim; i++) {
        for(int j = 0; j < squaredDim; j++) {
            if(!drawsMatrix.markGrid[j][i]) {
                unmarkedSum += drawsMatrix.matrix[j][i];
            }
        }
    }
    return unmarkedSum * drawSequence[drawsMatrix.drawsUntilWin - 1];
}

int** matrix_create(int squaredDim) {
    int** self = calloc(squaredDim, sizeof(int*));
    for(int i = 0; i < squaredDim; i++) {
        self[i] = calloc(squaredDim, sizeof(int));
    }
    return self;
}

void matrix_destroy(void** matrix, int squaredDim) {
    for(int j = 0; j < squaredDim; j++) {
        free(matrix[j]);
    }
    free(matrix);
}

void matrix_analysis_allocator_destroy(t_matrix_analysis* matrixAnalysisAllocator, int matrixAnalysisAllocatorSize, int squaredDim) {
    for(int i = 0; i < matrixAnalysisAllocatorSize - 1; i++) {
        bool** markGrid = matrixAnalysisAllocator[i].markGrid;
        int** matrix = matrixAnalysisAllocator[i].matrix;
        matrix_destroy((void**) markGrid, squaredDim);
        matrix_destroy((void**) matrix, squaredDim);
    }
    free(matrixAnalysisAllocator);
}

void print_matrix_analysis(t_matrix_analysis matrixAnalysis, int squaredDim) {
    printf("DRAWS UNTIL WIN: %d\n", matrixAnalysis.drawsUntilWin);
    for(int i = 0; i < squaredDim; i++) {
        for(int j = 0; j < squaredDim; j++) {
            printf("%d\t", matrixAnalysis.matrix[j][i]);
        }
        printf("\n");
    }
    printf("\n");
    for(int i = 0; i < squaredDim; i++) {
        for(int j = 0; j < squaredDim; j++) {
            printf("%d\t", matrixAnalysis.markGrid[j][i]);
        }
        printf("\n");
    }
}

int solution(FILE* input, int squaredDim, char* outcome) {
    int drawSequenceSize = get_draw_sequence_size(input);

    int* drawSequence = get_draw_sequence(input, drawSequenceSize);
    skip_newlines_until_next_matrix(input);

    int matrixAnalysisAllocatorSize = 1;
    t_matrix_analysis* matrixAnalysisAllocator = calloc(matrixAnalysisAllocatorSize, sizeof(t_matrix_analysis));

    int x = 0;
    int y = 0;

    int** matrix = matrix_create(squaredDim);
    while(fscanf(input, "%d", &matrix[x][y]) != EOF) {
        x++;
        if(x == squaredDim) {
            x = 0;
            if(y < squaredDim - 1) {
                y++;
            } else if(y == squaredDim - 1) {
                matrixAnalysisAllocator[matrixAnalysisAllocatorSize - 1] = get_matrix_analysis_until_win(squaredDim, matrix, drawSequence, drawSequenceSize);
                skip_newlines_until_next_matrix(input);
                y = 0;
                matrixAnalysisAllocator = realloc(matrixAnalysisAllocator, ++matrixAnalysisAllocatorSize * sizeof(t_matrix_analysis));
                matrix = matrix_create(squaredDim);
            }
        }
    }

    t_matrix_analysis matrixAnalysis;
    if(is_octo_lose(outcome)) {
        printf("OCTOLOSE!\n");
        matrixAnalysis = draws_matrix(matrixAnalysisAllocator, matrixAnalysisAllocatorSize, minimize_draws);
    } else if(is_octo_win(outcome)) {
        printf("OCTOWIN!\n");
        matrixAnalysis = draws_matrix(matrixAnalysisAllocator, matrixAnalysisAllocatorSize, maximize_draws);
    }
    print_matrix_analysis(matrixAnalysis, squaredDim);
    int score = get_score(matrixAnalysis, drawSequence, squaredDim);

    free(drawSequence);
    matrix_analysis_allocator_destroy(matrixAnalysisAllocator, matrixAnalysisAllocatorSize, squaredDim);
    matrix_destroy((void**) matrix, squaredDim);

    return score;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 5 (OCTOLOSE | OCTOWIN)"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input, atoi(argv[2]), argv[3]);
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
