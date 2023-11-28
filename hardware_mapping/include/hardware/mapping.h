/**
 * @file mapping.h
 *
 * @brief Provides functions for interfacing with an ultrasonic sensor.
 *
 * This module contains the definitions and function declarations necessary for
 * initializing and using an ultrasonic sensor. Functions include sensor initialization,
 * triggering pulse, distance measurement, and GPIO interrupt handling.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2023 Team 61. All rights reserved.
 */

#ifndef _MAPPING_H
#define _MAPPING_H

#define MAX_MAP_WIDTH 11

#define X_POS 1
#define Y_POS 2
#define MAP_HEIGHT 3
#define INCREMENT 1
#define DECREMENT 2
#define GET_VALUE 3
#define UPDATE 4
#define SET_NEW_MAP 5

uint variables(uint type, uint action);
void setMap(uint dir);
void getShortestPath();
void printMap();
void map_init();

#endif

/*** End of file ***/