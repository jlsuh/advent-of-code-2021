#include <stdio.h>
#include <stdlib.h>

#define POSSIBLE_MOVES_SIZE 4

typedef struct {
    int x;
    int y;
} t_point;

typedef struct t_cell t_cell;
struct t_cell {
    t_point point;
    int value;
    t_cell *adjacentLower;
};

typedef struct {
    t_cell** matrix;
    int rows;
    int cols;
} t_map;

t_map* map_create(FILE* input) {
    char value = '\0';
    int rows = 1;
    int cols = 0;
    while((value = fgetc(input)) != EOF) {
        if(value == '\n') {
            rows++;
            cols = 0;
        } else {
            cols++;
        }
    }
    rewind(input);

    t_map* map = malloc(sizeof(t_map));
    map->rows = rows;
    map->cols = cols;
    map->matrix = calloc(map->rows, sizeof(t_cell*));

    for(int i = 0; i < map->rows; i++) {
        map->matrix[i] = calloc(map->cols, sizeof(t_cell));
        for(int j = 0; j < map->cols;) {
            map->matrix[i][j].point.x = j;
            map->matrix[i][j].point.y = i;
            map->matrix[i][j].adjacentLower = NULL;
            value = fgetc(input);
            if(value != '\n') {
                map->matrix[i][j].value = value - '0';
                j++;
            }
        }
    }
    return map;
}

void matrix_destroy(void** matrix, int dim) {
    for(int j = 0; j < dim; j++) {
        free(matrix[j]);
    }
    free(matrix);
}

void map_destroy(t_map* map) {
    matrix_destroy((void**) map->matrix, map->rows);
    free(map);
}

t_point* possible_moves(t_map* map, t_point* moves, t_point currPoint, int* size) {
    int possibleMoves[POSSIBLE_MOVES_SIZE][2] = {
        {currPoint.x, currPoint.y + 1}, /* shift up     */
        {currPoint.x + 1, currPoint.y}, /* shift right  */
        {currPoint.x, currPoint.y - 1}, /* shift down   */
        {currPoint.x - 1, currPoint.y}  /* shift left   */
    };
    for(int i = 0; i < POSSIBLE_MOVES_SIZE; i++) {
        if(possibleMoves[i][0] >= 0 && possibleMoves[i][0] < map->cols && possibleMoves[i][1] >= 0 && possibleMoves[i][1] < map->rows) {
            moves = realloc(moves, ++(*size) * sizeof(t_point));
            moves[*size - 1].x = possibleMoves[i][0];
            moves[*size - 1].y = possibleMoves[i][1];
        }
    }
    return moves;
}

void print_lower_points(t_map* map) {
    for(int i = 0; i < map->rows; i++) {
        for(int j = 0; j < map->cols; j++) {
            t_cell cell = map->matrix[i][j];
            if(cell.adjacentLower == NULL) {
                printf("\e[1;93m%d\e[0m", cell.value);
            } else {
                printf("%d", cell.value);
            }
        }
        printf("\n");
    }
}

t_map* set_lower_adjacent_points(t_map* map) {
    for(int i = 0; i < map->rows; i++) {
        for(int j = 0; j < map->cols; j++) {
            t_point* possibleMoves = NULL;
            t_cell* current = &map->matrix[i][j];
            int possibleMovesSize = 0;
            possibleMoves = possible_moves(map, possibleMoves, current->point, &possibleMovesSize);
            for(int k = 0; k < possibleMovesSize; k++) {
                t_point move = possibleMoves[k];
                t_cell* adjacent = &map->matrix[move.y][move.x];
                if(adjacent->value <= current->value) {
                    current->adjacentLower = adjacent;
                    break;
                }
            }
            free(possibleMoves);
        }
    }
    return map;
}

int risk_level(t_map* map) {
    int risk = 0;
    for(int i = 0; i < map->rows; i++) {
        for(int j = 0; j < map->cols; j++) {
            t_cell cell = map->matrix[i][j];
            if(cell.adjacentLower == NULL) {
                risk += cell.value + 1;
            }
        }
    }
    return risk;
}

int solution(FILE* input) {
    t_map* map = map_create(input);
    map = set_lower_adjacent_points(map);
    print_lower_points(map);
    int riskLevel = risk_level(map);
    map_destroy(map);
    return riskLevel;
}

int main(int argc, char *argv[] /*ARGS="../input.txt"*/) {
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
