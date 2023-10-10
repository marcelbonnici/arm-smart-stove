/*Header file for a simple set of functions to write to 4x20 LCD (operating in
4-bit mode) without a library, as well as writing to other components. */
#include "mbed.h"
#ifndef FOUR_BIT_LCD_H
#define FOUR_BIT_LCD_H
#define ENABLE 0x08 //ORed in to data value to strobe E bit
#define COMMAND_MODE 0x00 //to clear RS line to 0, for command transfer
#define DATA_MODE 0x04 //to set RS line to 1, for data transfer
//Function Prototypes
void clr_lcd(void);
void init_lcd(void);
void new_line(char code, const char *string);
void play_tune(int rise);
void print_lcd(const char *string);
float read_temperature(bool display);
void shift_out(int data);
void write_4bit(int data, int mode); //added int mode because of discrepancy
void write_cmd(int cmd);
void write_data(char c);
#endif