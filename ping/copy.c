        xMessageBufferReceive(
            sendDataLeftEncoderSpeedCMB,
            (void *) &left_encoder_speed,
            sizeof(left_encoder_speed),
            portMAX_DELAY);

        xMessageBufferReceive(
            sendDataRightEncoderSpeedCMB,
            (void *) &right_encoder_speed,
            sizeof(right_encoder_speed),
            portMAX_DELAY);

            void read_wheel_encoder(__unused void *params) {
    setupWheelEncoders(NULL);

    while (true) {
        vTaskDelay(10);

        // printf("Left: %.2f\n", leftEncoderSpeed);
        
        xMessageBufferSend(
                sendDataLeftEncoderSpeedCMB,
                (void *) &leftEncoderSpeed,
                sizeof(leftEncoderSpeed),
                0);

        xMessageBufferSend(
                sendDataRightEncoderSpeedCMB,
                (void *) &rightEncoderSpeed,
                sizeof(rightEncoderSpeed),
                0);
    }
}


sendDataLeftEncoderSpeedCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);
    sendDataRightEncoderSpeedCMB = xMessageBufferCreate(mbaTASK_MESSAGE_BUFFER_SIZE);

    TaskHandle_t readWheelEncoderTask;
    xTaskCreate(read_wheel_encoder, "ReadWheelEncoderThread", configMINIMAL_STACK_SIZE, NULL, 5, &readWheelEncoderTask);

    