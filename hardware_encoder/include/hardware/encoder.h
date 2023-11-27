#ifndef _ENCODER_H
#define _ENCODER_H

#define LEFT_ENCODER_PIN 16
#define RIGHT_ENCODER_PIN 17
#define NOTCHES_PER_CYCLE 20
#define CM_PER_NOTCH 1.0

void leftEncoder(void *params);
void rightEncoder(void *params);
double getLeftSpeed(void *params);
double getRightSpeed(void *params);
uint32_t getLeftNotchCount(void *params);
uint32_t getRightNotchCount(void *params);


#endif

/*** End of file ***/