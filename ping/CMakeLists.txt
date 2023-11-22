
# Fill this in sir
set(WIFI_SSID "Galaxy_N")
set(WIFI_PASSWORD "12345678")


if (EXISTS ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c)
    add_executable(picow_freertos_ping_nosys
            picow_freertos_ping.c
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
            )
    target_compile_definitions(picow_freertos_ping_nosys PRIVATE
            WIFI_SSID=\"${WIFI_SSID}\"
            WIFI_PASSWORD=\"${WIFI_PASSWORD}\"
            )
    target_include_directories(picow_freertos_ping_nosys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            )
    target_link_libraries(picow_freertos_ping_nosys
            pico_cyw43_arch_lwip_threadsafe_background
            pico_stdlib
            pico_lwip_iperf
            FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
            hardware_pwm
            pico_enable_stdio_usb(picow_freertos_ping_sys 1)
            )
    pico_add_extra_outputs(picow_freertos_ping_nosys)


    add_executable(picow_freertos_ping_sys
            picow_freertos_ping.c 
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c 
            )
    target_compile_definitions(picow_freertos_ping_sys PRIVATE
            NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
            LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
            PING_USE_SOCKETS=1
            )
    target_include_directories(picow_freertos_ping_sys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            )
    target_link_libraries(picow_freertos_ping_sys
        hardware_adc
        pico_cyw43_arch_lwip_sys_freertos
        pico_stdlib     
        pico_lwip_iperf
        FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
        hardware_pwm
        pico_lwip_http
        hardware_barcode
        hardware_motor
        hardware_ultrasonic
        hardware_encoder
        hardware_irline
        hardware_magnetometer
        hardware_i2c
        )
    
    pico_enable_stdio_usb(picow_freertos_ping_sys 1)
    pico_add_extra_outputs(picow_freertos_ping_sys)
endif()