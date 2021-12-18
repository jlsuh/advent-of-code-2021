#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

typedef struct {
    int x;
    int y;
} t_point;

typedef struct {
    t_point* points;
    int numberOfPoints;
} t_path;

void print_points(t_path* path) {
    t_point* points = path->points;
    for(int i = 0; i < path->numberOfPoints; i++) {
        printf("(%d,%d)", points[i].x, points[i].y);
        if(i < path->numberOfPoints - 1) {
            printf(" -> ");
        } else {
            printf("\n");
        }
    }
    printf("\n");
}

void print_matrix(int** matrix, int maxDim) {
    for(int i = 0; i < maxDim; i++) {
        for(int j = 0; j < maxDim; j++) {
            if(!matrix[j][i]) {
                printf(".  ");
            } else {
                printf("%d  ", matrix[j][i]);
            }
        }
        printf("\n");
    }
}

int get_max_between(int a, int b) {
    return a > b ? a : b;
}

int maximum_member(t_point p1, t_point p2) {
    int xMax = get_max_between(p1.x, p2.x);
    int yMax = get_max_between(p1.y, p2.y);
    return get_max_between(xMax, yMax);
}

int get_max_dim(FILE* input) {
    int maxMember = 0;
    int tempMaxMember = 0;
    t_point p1 = {.x = 0, .y = 0};
    t_point p2 = {.x = 0, .y = 0};
    while(fscanf(input, "%d,%d -> %d,%d", &p1.x, &p1.y, &p2.x, &p2.y) != EOF) {
        tempMaxMember = maximum_member(p1, p2);
        if(tempMaxMember > maxMember) {
            maxMember = tempMaxMember;
        }
    }
    rewind(input);
    return maxMember + 1;
}

bool is_vertical_vent(t_point p1, t_point p2) {
    return p1.x == p2.x;
}

bool is_horizontal_vent(t_point p1, t_point p2) {
    return p1.y == p2.y;
}

bool is_diagonal_vent(t_point p1, t_point p2) {
    return !is_vertical_vent(p1, p2) && !is_horizontal_vent(p1, p2);
}

void set_one_dim_initial_final_point(t_point* iPoint, t_point* fPoint, t_point p1, t_point p2) {
    if(is_vertical_vent(p1, p2)) {
        if(p1.y < p2.y) {
            iPoint->x = p1.x;
            iPoint->y = p1.y;
            fPoint->x = p2.x;
            fPoint->y = p2.y;
        } else {
            iPoint->x = p2.x;
            iPoint->y = p2.y;
            fPoint->x = p1.x;
            fPoint->y = p1.y;
        }
    } else if(is_horizontal_vent(p1, p2)) {
        if(p1.x < p2.x) {
            iPoint->x = p1.x;
            iPoint->y = p1.y;
            fPoint->x = p2.x;
            fPoint->y = p2.y;
        } else {
            iPoint->x = p2.x;
            iPoint->y = p2.y;
            fPoint->x = p1.x;
            fPoint->y = p1.y;
        }
    }
}

void set_two_dim_initial_final_point(t_point* iPoint, t_point* fPoint, t_point p1, t_point p2) {
    if(p1.x < p2.x) {
        iPoint->x = p1.x;
        iPoint->y = p1.y;
        fPoint->x = p2.x;
        fPoint->y = p2.y;
    } else {
        iPoint->x = p2.x;
        iPoint->y = p2.y;
        fPoint->x = p1.x;
        fPoint->y = p1.y;
    }
}

bool is_northwest_southeast(t_point p1, t_point p2) {
    return (p1.x < p2.x && p1.y < p2.y) || (p1.x > p2.x && p1.y > p2.y);
}

bool is_northeast_southwest(t_point p1, t_point p2) {
    return (p1.x > p2.x && p1.y < p2.y) || (p1.x < p2.x && p1.y > p2.y);
}

t_point* generate_one_dim_points(t_point* points, int* currSize, t_point iPoint, t_point fPoint, t_point p1, t_point p2) {
    int x = iPoint.x;
    int y = iPoint.y;
    while(x <= fPoint.x && y <= fPoint.y) {
        t_point point = {
            .x = x,
            .y = y
        };
        points[*currSize - 1] = point;
        if(y < fPoint.y || x < fPoint.x) {
            points = realloc(points, ++(*currSize) * sizeof(t_point));
        }
        if(is_vertical_vent(p1, p2)) {
            y++;
        } else if(is_horizontal_vent(p1, p2)) {
            x++;
        }
    }
    return points;
}

t_point* generate_two_dim_points(t_point* points, int* currSize, t_point iPoint, t_point fPoint, t_point p1, t_point p2,
                                 bool (*loop_condition)(int x, int y, t_point point),
                                 bool (*realloc_condition)(int x, int y, t_point point),
                                 void (*post_index_increment)(int* x, int* y)) {
    int x = iPoint.x;
    int y = iPoint.y;
    while(loop_condition(x, y, fPoint)) {
        t_point point = {
            .x = x,
            .y = y
        };
        points[*currSize - 1] = point;
        if(realloc_condition(x, y, fPoint)) {
            points = realloc(points, ++(*currSize) * sizeof(t_point));
        }
        post_index_increment(&x, &y);
    }
    return points;
}

bool northwest_southeast_loop_condition(int x, int y, t_point point) {
    return x <= point.x && y <= point.y;
}

bool northeast_southwest_loop_condition(int x, int y, t_point point) {
    return x <= point.x && y >= point.y;
}

bool northwest_southeast_realloc_condition(int x, int y, t_point point) {
    return x < point.x && y < point.y;
}

bool northeast_southwest_realloc_condition(int x, int y, t_point point) {
    return x < point.x && y > point.y;
}

void northeast_southwest_post_index_increment(int* x, int* y) {
    (*x)++;
    (*y)--;
}

void northwest_southeast_post_index_increment(int* x, int* y) {
    (*x)++;
    (*y)++;
}

t_path* path_create(t_point p1, t_point p2) {
    int currSize = 0;
    t_point* points = NULL;
    t_path* path = NULL;
    points = calloc(++currSize, sizeof(t_point));
    path = malloc(sizeof(t_path));

    t_point iPoint = {
        .x = -1,
        .y = -1
    };
    t_point fPoint = {
        .x = -1,
        .y = -1
    };

    if(!is_diagonal_vent(p1, p2)) {
        set_one_dim_initial_final_point(&iPoint, &fPoint, p1, p2);
        points = generate_one_dim_points(points, &currSize, iPoint, fPoint, p1, p2);
    } else {
        set_two_dim_initial_final_point(&iPoint, &fPoint, p1, p2);
        if(is_northwest_southeast(p1, p2)) {
            points = generate_two_dim_points(
                points, &currSize, iPoint, fPoint, p1, p2,
                northwest_southeast_loop_condition,
                northwest_southeast_realloc_condition,
                northwest_southeast_post_index_increment
            );
        } else if(is_northeast_southwest(p1, p2)) {
            points = generate_two_dim_points(
                points, &currSize, iPoint, fPoint, p1, p2,
                northeast_southwest_loop_condition,
                northeast_southwest_realloc_condition,
                northeast_southwest_post_index_increment
            );
        }
    }
    path->numberOfPoints = currSize;
    path->points = points;
    return path;
}

void path_destroy(t_path* path) {
    free(path->points);
    free(path);
}

void matrix_destroy(void** matrix, int dim) {
    for(int i = 0; i < dim; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void register_points(t_path* path, int dim, int** matrix) {
    for(int i = 0; i < path->numberOfPoints; i++) {
        matrix[path->points[i].x][path->points[i].y] += 1;
    }
}

void register_paths(FILE* input, int maxDim, int** matrix) {
    t_point p1 = {.x = 0, .y = 0};
    t_point p2 = {.x = 0, .y = 0};
    while(fscanf(input, "%d,%d -> %d,%d", &p1.x, &p1.y, &p2.x, &p2.y) != EOF) {
        t_path* path = path_create(p1, p2);
        if(path == NULL) {
            continue;
        }
        // print_points(path);
        register_points(path, maxDim, matrix);
        path_destroy(path);
    }
}

int number_of_points_where_at_least_n_lines_overlap(int n, int** matrix, int maxDim) {
    int count = 0;
    for(int i = 0; i < maxDim; i++) {
        for(int j = 0; j < maxDim; j++) {
            if(matrix[j][i] >= n) {
                count++;
            }
        }
    }
    return count;
}

int solution(FILE* input) {
    int maxDim = get_max_dim(input);
    int** matrix = calloc(maxDim, sizeof(char*));
    for(int i = 0; i < maxDim; i++) {
        matrix[i] = calloc(maxDim, sizeof(int));
    }
    register_paths(input, maxDim, matrix);
    // print_matrix(matrix, maxDim);
    int overlappedPoints = number_of_points_where_at_least_n_lines_overlap(2, matrix, maxDim);
    matrix_destroy((void**) matrix, maxDim);
    return overlappedPoints;
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
