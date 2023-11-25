# INF2004_Project_Car

## Setting Up
- Put `hardware_<sensor>` files into `\Pico-v1.5.1\pico-extras\src\rp2_common`.
  - Make sure to add `pico_add_subdirectory(hardware_<sensor>)` for each sensor file into the `CMakeList.txt`.

- Put `ping` folder into `\Pico-v1.5.1\pico-examples\pico_w\wifi\freertos`.
  - Wifi module is located inside the `ping` folder.
  - Main integration file is `picow_freertos_ping.c` inside `ping` folder.
