#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#define POSSIBLE_MOVES_SIZE 4
#define TILES 5
#define MAX_COST 9

typedef struct t_node t_node;
struct t_node {
    double cost;
    uint32_t x;
    uint32_t y;
    t_node* parent;
    t_node* next;
};

void matrix_destroy(void** matrix, size_t dim) {
    for(size_t i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void increment_value(uint8_t** values, uint32_t cost, size_t colTile, size_t rowTile, size_t currX, size_t currY) {
    uint8_t finalCost = 0;
    uint32_t totalCost = cost + colTile + rowTile;
    if(totalCost <= MAX_COST) {
        finalCost = totalCost;
    } else {
        uint32_t modulus = totalCost % MAX_COST;
        finalCost = modulus;
    }
    values[currY][currX] = finalCost;
}

uint8_t** cost_matrix_create(FILE* input, size_t* rows, size_t* cols) {
    char c = '\0';
    while((c = fgetc(input)) != '\n') {
        (*cols)++;
    }
    rewind(input);
    while((c = fgetc(input)) != EOF) {
        if(c == '\n') {
            (*rows)++;
        }
    }
    rewind(input);
    printf("Rows: %ld | Cols: %ld\n", *rows, *cols);
    uint8_t cost = 0;
    uint8_t** self = calloc(*rows * TILES, sizeof(uint32_t*));
    for(size_t i = 0; i < *rows * TILES; i++) {
        self[i] = calloc(*cols * TILES, sizeof(uint32_t));
    }
    for(size_t i = 0; i < *rows; i++) {
        for(size_t j = 0; j < *cols; j++) {
            fscanf(input, "%1hhd", &cost);
            self[i][j] = cost;
        }
    }
    size_t rowTile = 0;
    size_t colTile = 0;
    for(size_t i = 0; i < *rows * TILES; i++) {
        for(size_t j = 0; j < *cols * TILES; j++) {
            cost = self[i - *rows * rowTile][j - *cols * colTile];
            increment_value(self, cost, colTile, rowTile, j, i);
            if((j + 1) % *cols == 0) {
                colTile++;
            }
        }
        colTile = 0;
        if((i + 1) % *rows == 0) {
            rowTile++;
        }
    }
    *rows = *rows * TILES;
    *cols = *cols * TILES;
    return self;
}

t_node node_create(double cost, uint32_t x, uint32_t y) {
    t_node self = {
        .cost = cost,
        .x = x,
        .y = y,
        .parent = NULL,
        .next = NULL
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
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            t_node* curr = &nodeMatrix[i][j];
            t_node* next = NULL;
            if(j == cols - 1) {
                if(i < rows - 1) {
                    next = &nodeMatrix[i + 1][0];
                }
            } else {
                next = &nodeMatrix[i][j + 1];
            }
            curr->next = next;
        }
    }
    nodeMatrix[0][0].cost = 0;
    return nodeMatrix;
}

uint32_t** get_possible_moves(t_node* node, size_t rows, size_t cols, size_t* movesSize) {
    uint32_t** moves = NULL;
    const uint32_t possibleMoves[POSSIBLE_MOVES_SIZE][2] = {
        {node->y, node->x + 1},
        {node->y + 1, node->x},
        {node->y, node->x - 1},
        {node->y - 1, node->x}
    };
    for(size_t i = 0; i < POSSIBLE_MOVES_SIZE; i++) {
        if(possibleMoves[i][0] >= 0 && possibleMoves[i][0] < rows && possibleMoves[i][1] >= 0 && possibleMoves[i][1] < cols) {
            moves = realloc(moves, ++(*movesSize) * sizeof(uint32_t*));
            moves[*movesSize - 1] = calloc(POSSIBLE_MOVES_SIZE, sizeof(uint32_t));
            moves[*movesSize - 1][0] = possibleMoves[i][0];
            moves[*movesSize - 1][1] = possibleMoves[i][1];
        }
    }
    return moves;
}

size_t hash(t_node* node, size_t cols) {
    return node->y * cols + node->x;
}

void hash_table_insert(t_node** hashTable, t_node* node, size_t cols) {
    size_t index = hash(node, cols);
    hashTable[index] = node;
}

size_t hash_table_remove(t_node** hashTable, t_node* node, size_t cols) {
    size_t index = hash(node, cols);
    hashTable[index] = NULL;
    return index;
}

bool hash_table_contains(t_node** hashTable, t_node* node, size_t cols) {
    size_t index = hash(node, cols);
    if(hashTable[index] == node) {
        return true;
    }
    return false;
}

t_node* get_node_with_minimum_cost(t_node* list) {
    t_node* minNode = NULL;
    t_node* curr = list;
    while(curr != NULL) {
        if(minNode == NULL || curr->cost < minNode->cost) {
            minNode = curr;
        }
        curr = curr->next;
    }
    return minNode;
}

t_node** hash_table_init(t_node** nodeMatrix, size_t rows, size_t cols) {
    t_node** hashTable = malloc(rows * cols * sizeof(t_node*));
    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            hash_table_insert(hashTable, &nodeMatrix[i][j], cols);
        }
    }
    return hashTable;
}

void node_list_remove(t_node** list, t_node* node) {
    t_node* curr = *list;
    if(curr == node) {
        *list = curr->next;
        return;
    }
    while(curr->next != NULL) {
        if(curr->next == node) {
            curr->next = node->next;
            return;
        }
        curr = curr->next;
    }
}

t_node** generate_shortest_path_graph(t_node** nodeMatrix, uint8_t** cost, size_t rows, size_t cols) {
    t_node** unprocessedNodes = hash_table_init(nodeMatrix, rows, cols);
    t_node* list = &nodeMatrix[0][0];
    for(size_t i = 0; i < rows * cols; i++) {
        printf("Current: %ld\n", i + 1);
        t_node* parent = get_node_with_minimum_cost(list);
        hash_table_remove(unprocessedNodes, parent, cols);
        node_list_remove(&list, parent);
        size_t movesSize = 0;
        uint32_t** moves = get_possible_moves(parent, rows, cols, &movesSize);
        for(size_t j = 0; j < movesSize; j++) {
            size_t y = moves[j][0];
            size_t x = moves[j][1];
            t_node* node = &nodeMatrix[y][x];
            if(hash_table_contains(unprocessedNodes, node, cols)) {
                double tempCost = parent->cost + cost[y][x];
                if(tempCost < node->cost) {
                    node->cost = tempCost;
                    node->parent = parent;
                }
            }
        }
        matrix_destroy((void**) moves, movesSize);
    }
    matrix_destroy((void**) unprocessedNodes, rows * cols);
    return nodeMatrix;
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
    printf("Rows: %ld | Cols: %ld\n", rows, cols);
    t_node** nodeMatrix = node_matrix_create(rows, cols);
    nodeMatrix = generate_shortest_path_graph(nodeMatrix, cost, rows, cols);
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
