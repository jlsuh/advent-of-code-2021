#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    bool** markGrid;
    int** matrix;
    int drawsUntilWin;
} t_matrix_analysis;

void skip_until_next_matrix(FILE* input) {
    char c = fgetc(input);
    while(c == '\n') {
        c = fgetc(input);
    }
    ungetc(c, input);
}

int* extract_draw_sequence(FILE* input, int drawSequenceSize) {
    int* drawSequence = malloc(sizeof(int) * drawSequenceSize);
    int i = 0;
    fscanf(input, "%d", &drawSequence[i++]);
    while(i < drawSequenceSize) {
        fscanf(input, ",%d", &drawSequence[i]);
        i++;
    }
    return drawSequence;
}

int draw_sequence_size(FILE* input) {
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

t_matrix_analysis matrix_analysis_until_win(int squaredDim, int** matrix, int* drawSequence, int drawSequenceSize) {
    t_matrix_analysis matrixAnalysis;

    bool bingo = false;
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
            bingo = is_bingo(squaredDim, markGrid);
            if(bingo) {
                break;
            }
        }
    }

    matrixAnalysis.drawsUntilWin = drawIndex;
    matrixAnalysis.markGrid = markGrid;
    matrixAnalysis.matrix = matrix;

    return matrixAnalysis;
}

t_matrix_analysis fewest_draws_matrix(t_matrix_analysis* matrixAnalysisAllocator, int matrixAnalysisAllocatorSize) {
    t_matrix_analysis lowest = matrixAnalysisAllocator[0];
    for(int i = 1; i < matrixAnalysisAllocatorSize - 1; i++) {
        if(matrixAnalysisAllocator[i].drawsUntilWin < lowest.drawsUntilWin) {
            lowest = matrixAnalysisAllocator[i];
        }
    }
    return lowest;
}

int** matrix_create(int squaredDim) {
    int** self = calloc(squaredDim, sizeof(int*));
    for(int i = 0; i < squaredDim; i++) {
        self[i] = calloc(squaredDim, sizeof(int));
    }
    return self;
}

int score_of_matrix_on_bingo(t_matrix_analysis fewestDrawsMatrix, int* drawSequence, int squaredDim) {
    int sum = 0;
    for(int i = 0; i < squaredDim; i++) {
        for(int j = 0; j < squaredDim; j++) {
            if(!fewestDrawsMatrix.markGrid[j][i]) {
                sum += fewestDrawsMatrix.matrix[j][i];
            }
        }
    }
    return sum * drawSequence[fewestDrawsMatrix.drawsUntilWin - 1];
}

void free_matrix(void** matrix, int squaredDim) {
    for(int j = 0; j < squaredDim; j++) {
        free(matrix[j]);
    }
    free(matrix);
}

void free_matrix_analysis_allocator(t_matrix_analysis* matrixAnalysisAllocator, int matrixAnalysisAllocatorSize, int squaredDim) {
    for(int i = 0; i < matrixAnalysisAllocatorSize - 1; i++) {
        bool** markGrid = matrixAnalysisAllocator[i].markGrid;
        int** matrix = matrixAnalysisAllocator[i].matrix;
        free_matrix((void**) markGrid, squaredDim);
        free_matrix((void**) matrix, squaredDim);
    }
    free(matrixAnalysisAllocator);
}

int solution(FILE* input, int squaredDim) {
    int drawSequenceSize = draw_sequence_size(input);

    int* drawSequence = extract_draw_sequence(input, drawSequenceSize);
    skip_until_next_matrix(input);

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
                matrixAnalysisAllocator[matrixAnalysisAllocatorSize - 1] = matrix_analysis_until_win(squaredDim, matrix, drawSequence, drawSequenceSize);
                skip_until_next_matrix(input);
                y = 0;
                matrixAnalysisAllocator = realloc(matrixAnalysisAllocator, ++matrixAnalysisAllocatorSize * sizeof(t_matrix_analysis));
                matrix = matrix_create(squaredDim);
            }
        }
    }

    t_matrix_analysis fewestDrawsMatrix = fewest_draws_matrix(matrixAnalysisAllocator, matrixAnalysisAllocatorSize);
    int score = score_of_matrix_on_bingo(fewestDrawsMatrix, drawSequence, squaredDim);

    free(drawSequence);
    free_matrix_analysis_allocator(matrixAnalysisAllocator, matrixAnalysisAllocatorSize, squaredDim);
    free_matrix((void**) matrix, squaredDim);

    return score;
}

int main(int argc, char *argv[] /*ARGS="../input.txt 5"*/) {
    FILE* input = fopen(argv[1], "r");
    if(input == NULL) {
        perror("Failed");
        return -1;
    } else {
        int answer = solution(input, atoi(argv[2]));
        printf("Answer: %d\n", answer);
    }
    fclose(input);
    return 0;
}
