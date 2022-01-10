#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define POSSIBLE_MOVES_SIZE 4

typedef struct t_node t_node;
struct t_node {
    double cost;
    uint32_t x;
    uint32_t y;
    bool processed;
    t_node* parent;
};

void matrix_destroy(void** matrix, size_t dim) {
    for(size_t i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

uint8_t** cost_matrix_create(FILE* input, size_t* rows, size_t* cols) {
    char c = '\0';
    while((c = fgetc(input)) != '\n') {
        (*cols)++;
    }
    rewind(input);
    (*rows)++;
    while((c = fgetc(input)) != EOF) {
        if(c == '\n') {
            (*rows)++;
        }
    }
    rewind(input);
    uint32_t cost = 0;
    uint8_t** self = calloc(*rows, sizeof(uint32_t*));
    for(size_t i = 0; i < *rows; i++) {
        self[i] = calloc(*cols, sizeof(uint32_t));
        for(size_t j = 0; j < *cols; j++) {
            fscanf(input, "%1d", &cost);
            self[i][j] = cost;
        }
    }
    return self;
}

t_node node_create(double cost, uint32_t x, uint32_t y) {
    t_node self = {
        .cost = cost,
        .x = x,
        .y = y,
        .processed = false,
        .parent = NULL
    };
    return self;
}

t_node** node_matrix_create(size_t rows, size_t cols) {
    t_node** nodeMatrix = malloc(rows * sizeof(t_node*));
    for(size_t y = 0; y < rows; y++) {
        nodeMatrix[y] = malloc(cols * sizeof(t_node));
        for(size_t x = 0; x < cols; x++) {
            nodeMatrix[y][x] = node_create(INFINITY, x, y);
        }
    }
    nodeMatrix[0][0].cost = 0;
    return nodeMatrix;
}

void node_matrix_print(t_node** self, size_t rows, size_t cols) {
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            t_node* parent = self[i][j].parent;
            if(parent != NULL) {
                printf("[%d,%d] | cost: %.f | processed: %d | parent: [%d,%d]\n", self[i][j].y, self[i][j].x, self[i][j].cost, self[i][j].processed, parent->y, parent->x);
                continue;
            }
            printf("[%d,%d] | cost: %.f | processed: %d | parent: null\n", self[i][j].y, self[i][j].x, self[i][j].cost, self[i][j].processed);
        }
    }
}

t_node* get_node_with_minimum_cost(t_node** matrix, size_t rows, size_t cols) {
    t_node* min = NULL;
    bool locateFirstMin = true;
    for(size_t i = 0; locateFirstMin && i < rows; i++) {
        for(size_t j = 0; locateFirstMin && j < cols; j++) {
            if(!matrix[i][j].processed) {
                min = &matrix[i][j];
                locateFirstMin = false;
            }
        }
    }
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            if(!matrix[i][j].processed && matrix[i][j].cost < min->cost) {
                min = &matrix[i][j];
            }
        }
    }
    return min;
}

uint8_t** get_possible_moves(t_node* node, size_t rows, size_t cols, size_t* movesSize) {
    uint8_t** moves = NULL;
    const uint8_t possibleMoves[POSSIBLE_MOVES_SIZE][2] = {
        {node->y, node->x + 1},
        {node->y + 1, node->x},
        {node->y, node->x - 1},
        {node->y - 1, node->x}
    };
    for(size_t i = 0; i < POSSIBLE_MOVES_SIZE; i++) {
        if(possibleMoves[i][0] >= 0 && possibleMoves[i][0] < rows && possibleMoves[i][1] >= 0 && possibleMoves[i][1] < cols) {
            moves = realloc(moves, ++(*movesSize) * sizeof(uint8_t*));
            moves[*movesSize - 1] = calloc(POSSIBLE_MOVES_SIZE, sizeof(uint8_t));
            moves[*movesSize - 1][0] = possibleMoves[i][0];
            moves[*movesSize - 1][1] = possibleMoves[i][1];
        }
    }
    return moves;
}

void generate_shortest_path_graph(t_node** nodeMatrix, uint8_t** cost, size_t rows, size_t cols) {
    for(size_t i = 0; i <= rows * cols - 1; i++) {
        t_node* parent = get_node_with_minimum_cost(nodeMatrix, rows, cols);
        parent->processed = true;
        size_t movesSize = 0;
        uint8_t** moves = get_possible_moves(parent, rows, cols, &movesSize);
        for(size_t j = 0; j < movesSize; j++) {
            size_t y = moves[j][0];
            size_t x = moves[j][1];
            t_node* node = &nodeMatrix[y][x];
            if(!node->processed) {
                double tempCost = parent->cost + cost[y][x];
                if(tempCost < node->cost) {
                    node->cost = tempCost;
                    node->parent = parent;
                }
            }
        }
        matrix_destroy((void**) moves, movesSize);
    }
}

uint64_t get_cost_to_last_node(t_node** nodeMatrix, uint8_t** cost, size_t rows, size_t cols) {
    uint64_t totalCost = 0;
    t_node* curr = &nodeMatrix[rows - 1][cols - 1];
    while(curr->parent != NULL) {
        totalCost += cost[curr->y][curr->x];
        curr = curr->parent;
    }
    return totalCost;
}

uint64_t solution(FILE* input) {
    size_t cols = 0;
    size_t rows = 0;
    uint8_t** cost = cost_matrix_create(input, &rows, &cols);
    t_node** nodeMatrix = node_matrix_create(rows, cols);
    generate_shortest_path_graph(nodeMatrix, cost, rows, cols);
    node_matrix_print(nodeMatrix, rows, cols);
    uint64_t totalCost = get_cost_to_last_node(nodeMatrix, cost, rows, cols);
    matrix_destroy((void**) cost, rows);
    matrix_destroy((void**) nodeMatrix, rows);
    return totalCost;
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
