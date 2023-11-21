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

// void gpio_callback_encoder(uint gpio, uint32_t events);
// void setupWheelEncoders(void *params);

void leftEncoder(void *params);
void rightEncoder(void *params);


#endif