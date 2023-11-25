#ifndef _ENCODER_H
#define _ENCODER_H

#define LEFT_ENCODER_PIN 16
#define RIGHT_ENCODER_PIN 17
#define NOTCHES_PER_CYCLE 20
#define CM_PER_NOTCH 1.0

extern volatile double leftEncoderSpeed;
extern volatile double rightEncoderSpeed;
extern volatile double leftTotalDistance;
extern volatile double rightTotalDistance;
extern volatile uint32_t tempLeftNotchCount;
extern volatile uint32_t tempRightNotchCount;
extern volatile uint32_t leftNotchCount;
extern volatile uint32_t rightNotchCount;

void leftEncoder(void *params);
void rightEncoder(void *params);


#endif

/*** End of file ***/