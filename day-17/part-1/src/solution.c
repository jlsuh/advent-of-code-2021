#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int64_t x, y;
} t_vector2;

typedef struct {
    t_vector2 finalPos, initialPos;
} t_target;

typedef struct {
    t_vector2 vi;
    t_vector2** trajectory;
    size_t trajectorySize;
} t_trajectory;

t_target target_create(FILE* input) {
    int64_t xf, xi, yf, yi;
    fscanf(input, "target area: x=%ld..%ld, y=%ld..%ld", &xi, &xf, &yi, &yf);
    return (t_target const){.finalPos.x = xf, .finalPos.y = yf, .initialPos.x = xi, .initialPos.y = yi};
}

void target_print(t_target const target) {
    printf("Initial: (%ld, %ld) | Final: (%ld, %ld)\n",
           target.initialPos.x, target.initialPos.y,
           target.finalPos.x, target.finalPos.y);
}

t_vector2 * const initial_velocities(t_target const target, size_t* const velSize) {
    int64_t vxMax, vxMin, vyMax, vyMin;
    vxMin = 0;
    vxMax = target.finalPos.x;
    vyMin = target.initialPos.y;
    vyMax = abs(vyMin);
    t_vector2* initVels = NULL;
    for (int64_t i = vyMax; i >= vyMin; i -= 1) {
        for (int64_t j = vxMin; j <= vxMax; j += 1) {
            *velSize += 1;
            initVels = realloc(initVels, *velSize * sizeof(t_vector2));
            initVels[*velSize - 1] = (t_vector2){.x = j, .y = i};
        }
    }
    return initVels;
}

/////////////////////////////////////////////////////
t_vector2 position_transform(t_vector2 const currPos, t_vector2 const currVel) {
    return (t_vector2){.x = currPos.x + currVel.x, .y = currPos.y + currVel.y};
}

t_vector2 velocity_transform(t_vector2 const currVel) {
    t_vector2 newVel = currVel;
    if (newVel.x > 0) {
        newVel.x -= 1;
    } else if (newVel.x < 0) {
        newVel.x += 1;
    }
    newVel.y -= 1;
    return newVel;
}

t_vector2 trajectory_travel(t_vector2 const currPos, t_vector2* const currVel) {
    t_vector2 newPos = position_transform(currPos, *currVel);
    *currVel = velocity_transform(*currVel);
    return newPos;
}

bool is_within_limits(t_target target, t_vector2 currPos) {
    return currPos.x <= target.finalPos.x && ((target.initialPos.y < 0 && currPos.y >= target.initialPos.y) || (target.initialPos.y >= 0 && currPos.y >= 0));
}

t_vector2* vector2_create(int64_t x, int64_t y) {
    t_vector2* self = malloc(sizeof(t_vector2));
    self->x = x;
    self->y = y;
    return self;
}

void vector2_arr_print(t_vector2 const* const vectors2, size_t const size) {
    for (size_t i = 0; i < size; i += 1) {
        printf("(%ld, %ld)\n", vectors2[i].x, vectors2[i].y);
    }
}

void trajectory_print(t_trajectory* t) {
    for (size_t i = 0; i < t->trajectorySize; i += 1) {
        t_vector2* v = t->trajectory[i];
        printf("(%ld, %ld)\n", v->x, v->y);
    }
}

t_trajectory* trajectory_create(t_vector2 currVel, t_target target) {
    t_vector2 currPos = {.x = 0, .y = 0};
    t_vector2 auxVel  = currVel;

    t_vector2** trajectory = NULL;
    size_t trajectorySize = 0;

    currPos = trajectory_travel(currPos, &auxVel);
    while (is_within_limits(target, currPos)) {
        trajectorySize += 1;
        trajectory = realloc(trajectory, trajectorySize * sizeof(*trajectory));
        trajectory[trajectorySize - 1] = vector2_create(currPos.x, currPos.y);
        currPos = trajectory_travel(currPos, &auxVel);
    }

    t_trajectory* self = malloc(sizeof(*self));
    self->trajectory = trajectory;
    self->trajectorySize = trajectorySize;
    self->vi = currVel;

    return self;
}

int64_t trajectory_max_height(t_trajectory* t) {
    int64_t max = t->trajectory[0]->y;
    for (size_t i = 1; i < t->trajectorySize; i += 1) {
        if (t->trajectory[i]->y > max) {
            max = t->trajectory[i]->y;
        }
    }
    return max;
}

void trajectory_destroy(t_trajectory* t) {
    for (size_t i = 0; i < t->trajectorySize; i += 1) {
        free(t->trajectory[i]);
    }
    free(t->trajectory);
    free(t);
}

bool trajectory_target_hit(t_target target, t_trajectory* t) {
    for (size_t i = 0; i < t->trajectorySize; i += 1) {
        t_vector2* v = t->trajectory[i];
        if (v->x >= target.initialPos.x && v->x <= target.finalPos.x && v->y >= target.initialPos.y && v->y <= target.finalPos.y) {
            return true;
        }
    }
    return false;
}

t_trajectory* yMax_trajectory(t_target const target, t_vector2 const * const initVels, size_t const velSize) {
    t_trajectory* yMaxTrajectory = NULL;
    for (size_t i = 0; i < velSize; i += 1) {
        t_trajectory* trajectory = trajectory_create(initVels[i], target);
        if(trajectory_target_hit(target, trajectory)) {
            if (yMaxTrajectory != NULL) {
                if (trajectory_max_height(trajectory) > trajectory_max_height(yMaxTrajectory)) {
                    trajectory_destroy(yMaxTrajectory);
                    yMaxTrajectory = trajectory;
                } else {
                    trajectory_destroy(trajectory);
                }
            } else {
                yMaxTrajectory = trajectory;
            }
        } else {
            trajectory_destroy(trajectory);
        }
    }
    return yMaxTrajectory;
}
/////////////////////////////////////////////////////
uint64_t solution(FILE* input) {
    t_target const target = target_create(input);
    size_t velSize = 0;
    t_vector2 const * const initVels = initial_velocities(target, &velSize);
    target_print(target);
    t_trajectory* yMaxTrajectory = yMax_trajectory(target, initVels, velSize);
    printf("Maximum height Vi: (%ld, %ld)\n", yMaxTrajectory->vi.x, yMaxTrajectory->vi.y);
    int64_t highestPoint = trajectory_max_height(yMaxTrajectory);
    trajectory_destroy(yMaxTrajectory);
    free((void*) initVels);
    return highestPoint;
}

int main(int argc, char* argv[] /*ARGS="../input.txt"*/) {
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Failed");
        return -1;
    } else {
        uint64_t answer = solution(input);
        printf("Answer: %ld\n", answer);
    }
    fclose(input);
    return 0;
}
