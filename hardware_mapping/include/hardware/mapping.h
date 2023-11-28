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