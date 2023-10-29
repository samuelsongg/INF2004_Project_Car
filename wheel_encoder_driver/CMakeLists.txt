add_executable(wheel_encoder_driver
        wheel_encoder_driver.c
        )

# pull in common dependencies
target_link_libraries(wheel_encoder_driver pico_stdlib hardware_pwm hardware_adc)


# create map/bin/hex file etc.
pico_add_extra_outputs(wheel_encoder_driver)

pico_enable_stdio_usb(wheel_encoder_driver 1)

# add url via pico_set_program_url
example_auto_set_url(wheel_encoder_driver)
