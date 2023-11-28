/** @file mapping.c
 *
 * @brief This module handles the mapping algorithm for the car.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2023 Team 61. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"

#define MAX_MAP_WIDTH 11

#define X_POS 1
#define Y_POS 2
#define MAP_HEIGHT 3
#define INCREMENT 1
#define DECREMENT 2
#define GET_VALUE 3
#define UPDATE 4
#define SET_NEW_MAP 5

/**
 * @brief Manipulate or get the value of variables related to the map.
 *
 * @param type Type of variable to manipulate (X_POS, Y_POS, MAP_HEIGHT).
 * @param action Action to perform (INCREMENT, DECREMENT, GET_VALUE).
 * @return Current value of the specified variable.
 */
uint variables(uint type, uint action) {
    static uint posX = 5;
    static uint posY = -1;
    static uint mapHeight = 1;

    if (type == X_POS) {
        if (action == INCREMENT) {
            posX++;
        } else if (action == DECREMENT) {
            posX--;
        } else return posX;
    } else if (type == Y_POS) {
        if (action == INCREMENT) {
            posY++;
        } else if (action == DECREMENT) {
            posY--;
        } else return posY;
    } else if (type == MAP_HEIGHT) {
        if (action == INCREMENT) {
            mapHeight++;
        } else return mapHeight;
    }

    return 0;
}

/**
 * @brief Manipulate or get the map.
 *
 * @param action Action to perform (GET_VALUE, UPDATE, SET_NEW_MAP).
 * @param newMap Pointer to a new map to set, if action is SET_NEW_MAP.
 * @return Pointer to the current map.
 */
char** map(uint action, char** newMap) {
    static char** map;

    if (action == GET_VALUE) {
        return map;
    } else if (action == UPDATE) {
        uint mapHeight = variables(MAP_HEIGHT, GET_VALUE);
        // Allocate memory for the new dynamic array
        char** newMap = (char**)malloc(mapHeight * sizeof(char*));
        for (int i = 0; i < mapHeight; i++) {
            newMap[i] = (char*)malloc(MAX_MAP_WIDTH * sizeof(char));
            for (int j = 0; j < MAX_MAP_WIDTH; j++) {
                newMap[i][j] = 'X';
            }
        }

        // Copy the existing map to the new map
        for (int i = 0; i < mapHeight - 1; i++) {
            for (int j = 0; j < MAX_MAP_WIDTH; j++) {
                newMap[i][j] = map[i][j];
            }
        }

        // Free the memory of the old map
        for (int i = 0; i < mapHeight - 1; i++) {
            free(map[i]);
        }
        free(map);

        // Update the map and height
        map = newMap;
    } else if (action == SET_NEW_MAP) {
        map = newMap;
    }

    return map;
}

/**
 * @brief Update the map based on movement direction.
 *
 * @param dir Direction of movement (1: Up, 2: Right, 3: Left, 4: Down).
 */
void setMap(uint dir) {
    if (dir == 1) {
        variables(Y_POS, INCREMENT);
    } else if (dir == 2) {
        variables(X_POS, INCREMENT);
    } else if (dir == 3) {
        variables(X_POS, DECREMENT);
    } else {
        variables(Y_POS, DECREMENT);
    }

    if (variables(Y_POS, GET_VALUE) >= variables(MAP_HEIGHT, GET_VALUE)) {
        variables(MAP_HEIGHT, INCREMENT);
        map(UPDATE, NULL);
    }

    map(GET_VALUE, NULL)[variables(Y_POS, GET_VALUE)][variables(X_POS, GET_VALUE)] = ' ';
}

// Structure to represent points in the map.
typedef struct {
    int row;
    int col;
} Point;

// Structure to represent nodes in a queue.
typedef struct {
    Point point;
    int distance;
} QueueNode;

// Queue structure for BFS.
typedef struct {
    QueueNode* array;
    int front;
    int rear;
    unsigned capacity;
} Queue;

// Queue-related functions.
Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = 0;
    queue->array = (QueueNode*)malloc(capacity * sizeof(QueueNode));
    return queue;
}

int isEmpty(Queue* queue) {
    return queue->front == queue->rear;
}

void enqueue(Queue* queue, QueueNode item) {
    queue->array[queue->rear++] = item;
}

QueueNode dequeue(Queue* queue) {
    return queue->array[queue->front++];
}

/**
 * @brief Check if a cell is within the map boundaries.
 *
 * @param row Row index of the cell.
 * @param col Column index of the cell.
 * @return 1 if valid, 0 otherwise.
 */
int isValid(int row, int col) {
    return (row >= 0) && (row < variables(MAP_HEIGHT, GET_VALUE)) && (col >= 0) && (col < MAX_MAP_WIDTH);
}

/**
 * @brief Find the shortest path from start to end points.
 *
 * @param start Starting point.
 * @param end Ending point.
 * @return The map with the path marked.
 */
char** shortestPath(Point start, Point end) {
    int dr[] = {-1, 0, 1, 0};
    int dc[] = {0, 1, 0, -1};

    int ROWS = variables(MAP_HEIGHT, GET_VALUE);
    int COLS = MAX_MAP_WIDTH;

    Queue* queue = createQueue(ROWS * COLS);

    int visited[ROWS][COLS];
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            visited[i][j] = 0;
        }
    }

    visited[start.row][start.col] = 1;

    QueueNode startNode = {.point = start, .distance = 0};
    enqueue(queue, startNode);

    while (!isEmpty(queue)) {
        QueueNode current = dequeue(queue);
        Point currentPoint = current.point;

        if (currentPoint.row == end.row && currentPoint.col == end.col) {

            while (currentPoint.row != start.row || currentPoint.col != start.col) {
                map(GET_VALUE, NULL)[currentPoint.row][currentPoint.col] = '+';
                int direction = visited[currentPoint.row][currentPoint.col] - 1;
                currentPoint.row -= dr[direction];
                currentPoint.col -= dc[direction];
            }
            break;
        }

        for (int i = 0; i < 4; i++) {
            int newRow = currentPoint.row + dr[i];
            int newCol = currentPoint.col + dc[i];

            if (isValid(newRow, newCol) && map(GET_VALUE, NULL)[newRow][newCol] == ' ' && !visited[newRow][newCol]) {
                visited[newRow][newCol] = i + 1;
                QueueNode neighbor = {.point = {.row = newRow, .col = newCol}, .distance = current.distance + 1};
                enqueue(queue, neighbor);
            }
        }
    }

    map(GET_VALUE, NULL)[start.row][start.col] = '+';

    free(queue->array);
    free(queue);

    return map(GET_VALUE, NULL);
}

/**
 * Function to get and mark the shortest path on the map.
 */
void getShortestPath(){
    Point start = {.row = 0, .col = 5};
    Point end = {.row = variables(Y_POS, GET_VALUE), .col = variables(X_POS, GET_VALUE)};
    map(SET_NEW_MAP, shortestPath(start, end));
}

/**
 * Function to print the current state of the map.
 */
void printMap() {
    uint mapHeight = variables(MAP_HEIGHT, GET_VALUE);
    char** currentMap = map(GET_VALUE, NULL);
    for (int y = mapHeight - 1; y >= 0 ; y--) {
        for (int x = MAX_MAP_WIDTH - 1; x >= 0 ; x--) {
            printf("%c ", currentMap[y][x]);
        }
        printf("\n");
    }
    printf("\n\n");
}

/**
 * Function to initialize or update the map.
 */
void map_init(){
    map(UPDATE, NULL);
}
