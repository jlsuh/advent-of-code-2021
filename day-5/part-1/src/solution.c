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

int max(int a, int b) {
    return a > b ? a : b;
}

int maximum_member(t_point p1, t_point p2) {
    int xMax = max(p1.x, p2.x);
    int yMax = max(p1.y, p2.y);
    return max(xMax, yMax);
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

void set_start_end_points(int* start, int* end, int p1Member, int p2Member) {
    int difference = p2Member - p1Member;
    if(difference < 0) {
        *start = p2Member;
        *end = p1Member;
    } else {
        *start = p1Member;
        *end = p2Member;
    }
}

t_path* generate_path(t_point p1, t_point p2) {
    int currSize = 0;
    t_point* points = NULL;
    t_path* path = NULL;
    if(!is_diagonal_vent(p1, p2)) {
        points = calloc(++currSize, sizeof(t_point));
        path = malloc(sizeof(t_path));
        int* i = malloc(sizeof(int));
        int* end = malloc(sizeof(int));
        int* xValue = NULL;
        int* yValue = NULL;
        *i = -1;
        *end = -1;
        if(is_vertical_vent(p1, p2)) {
            set_start_end_points(i, end, p1.y, p2.y);
            xValue = malloc(sizeof(int));
            *xValue = p1.x;
            yValue = i;
        } else if(is_horizontal_vent(p1, p2)) {
            set_start_end_points(i, end, p1.x, p2.x);
            xValue = i;
            yValue = malloc(sizeof(int));
            *yValue = p1.y;
        }
        while(*i <= *end) {
            t_point point = {.x = *xValue, .y = *yValue};
            points[currSize - 1] = point;
            if(*i < *end) {
                points = realloc(points, ++currSize * sizeof(t_point));
            }
            (*i)++;
        }
        path->numberOfPoints = currSize;
        path->points = points;
        free(end);
        free(xValue);
        free(yValue);
    } else {
        return NULL;
    }
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
        t_path* path = generate_path(p1, p2);
        if(path == NULL) {
            continue;
        }
        print_points(path);
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
    print_matrix(matrix, maxDim);
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
