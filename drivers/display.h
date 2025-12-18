#ifndef ST7735_DRIVER_H
#define ST7735_DRIVER_H

#include "stdlib.h"

void initDisplay();
void send_command(uint8_t cmd);
void send_data(uint8_t* data, size_t len);
void send_data_byte(uint8_t data);

#endif
