/*A simple set of functions to write to 4x20 LCD,
operating in 4-bit mode. */
#include "LCD_and_temp.h"

//Using the LCD figured out with the following links
//https://www.youtube.com/watch?v=_gHjvVjxtC0
//https://forum.arduino.cc/t/add-to-reference-for-liquidcrystal-w-4-data-lines/46463/13

char commands[] = {0x51, 0xAA, 0xAC, 0x02, 0xA2, 0xA1}; //0x0C for 0.0625C granularity in 4.3.1b
char read_temp[2];
char write_temp[2] = {0x19, 0x00}; //77F or 25C, I think
char LCD_result[5];

PwmOut speaker(D9);
DigitalOut CS(D10);
BufferedSerial pc(USBTX, USBRX,9600); //UART example
SPI ser_port(D11, D12, D13); // Initialise SPI, using default settings
I2C temp_sensor(I2C_SDA, I2C_SCL); //Configure I2C interface
const int temp_addr = 0x90; //I2C address of temperature sensor DS1631

void clr_lcd(void){
    /*
    Clears display and waits required time
    */
    write_cmd(0x01); //display clear
    wait_us(1520);
}

void init_lcd(void){ 
    /*
    Follow designated procedure in data sheet
    */
    thread_sleep_for (40);
    shift_out(0x30); //function set 8-bit
    wait_us(37);
    write_cmd(0x20); //function set
    wait_us(37);
    write_cmd(0x20); //function set
    wait_us(37);
    write_cmd(0x0C); //display ON/OFF
    wait_us(37);
    write_cmd(0x01); //display clear
    wait_us(1520);
    write_cmd(0x06); //entry-mode set
    wait_us(37);
    write_cmd(0x28); //function set
    wait_us(37);
}
void new_line(char code, const char *string){
    /*
    Chooses where to start writing and what to write

    code - Hexadecimal code that locates starting point of text on LCD, per
    setCursor function here: 
    https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library/blob/master/LiquidCrystal_I2C.cpp

    string - Text to print to the LCD
    */
    write_cmd(code);
    wait_us(40);
    print_lcd(string);
}

void play_tune(int rise){
    /*
    Play musical notes on a speaker

    rise - index referring to starting note in 1D array of notes playable
    0 to 2 (inclusively) is the DONE jingle, 3 to 5 is the ERROR jingle
    */

    speaker.resume();
    int freq[6]={392,494,440,494,494,440};
    for (int i=rise;i<rise+3;i++) {
        speaker.period(1.0/(2*freq[i])); // set PWM period, which is (1/(2*freq)).
        speaker=0.5; // set duty cycle, hence volume control. 0 is silent, 1 is full blast
        thread_sleep_for(500); //hold for beat prd
    }
    //indicate end of song
    speaker.suspend();
}

void print_lcd(const char *string){
    /*
    Sends character string to LCD

    string - Text to print to the LCD
    */
    while(*string) {
        write_data(*string++);
        wait_us(40);
    }
}

float read_temperature(bool display){
    /*
    Writes and reads to temperature sensor per data sheet:
    https://www.analog.com/media/en/technical-documentation/data-sheets/DS1631-DS1731.pdf

    display - will reveal the temperature on the LCD only if True

    Returns:
    temp - Fahrenheit temperature in float
    */

    //Write 0x51 to 0x90 to start temperature conversion
    temp_sensor.write(temp_addr, &commands[0], 1, false);
    thread_sleep_for(500); //wait for the conversion to complete
    //sets sensor to 0.5C steps
    temp_sensor.write(temp_addr, &commands[2], 1, true);
    temp_sensor.write(temp_addr, &commands[3], 1, false);
    //the below sets the TOut pin high when it surpasses the inputted temp, and back low when it falls below inputted temp
    temp_sensor.write(temp_addr, &commands[5], 1, true); //Temp High Flag
    temp_sensor.write(temp_addr, write_temp, 2, false);
    temp_sensor.write(temp_addr, &commands[4], 1, true); //Temp Low Flag
    temp_sensor.write(temp_addr, write_temp, 2, false);
    temp_sensor.write(temp_addr, &commands[1], 1, false); //Write 0xAA to 0x90 to read the last converted temperature
    temp_sensor.read(temp_addr, read_temp, 2); //Read the temperature into the read_temp array
    //Convert temperature to Celsius
    float temp = (float((read_temp[0] << 8) | read_temp[1]) / 256) * 1.8 + 32;
    //Print temperature to the serial monitor
    printf("Temp = %d.%d\r\n", int(temp), int((temp-int(temp))*10));//UART example, cont.
    if (display){
        sprintf(LCD_result, "%d.%d", int(temp), int((temp-int(temp))*10));
        write_cmd(0xD4); //cursor to 4th line
        wait_us(40);
        for (char i=0;i<4;i++) write_data(LCD_result[i]);
        print_lcd("F / 150.0F");
    }
    return temp;
}

void shift_out(int data){
    /*
    Sends word to SPI port

    data - 
    */

    CS = 0;
    ser_port.write(data);
    CS = 1;
}

void write_4bit(int data, int mode){
    /*
    Converts data from temperature sensor to fathomable numbers

    data - Numbers
    mode - How numbers will be manipulated (mode is RS line, cmd=0, data=1)
    */

    int hi_n;
    int lo_n;
    hi_n = (data & 0xF0); //form the two 4-bit nibbles that will be sent
    lo_n = ((data << 4) &0xF0);
    //send each word twice, strobing the Enable line
    shift_out(hi_n | ENABLE | mode);
    wait_us(1);
    shift_out(hi_n & ~ENABLE);
    shift_out(lo_n | ENABLE | mode);
    wait_us(1);
    shift_out(lo_n & ~ENABLE);
}

void write_cmd(int cmd){
    /*
    Configures LCD command word

    cmd - LCD command word, obviously ;)
    */
    write_4bit(cmd, COMMAND_MODE);
}

void write_data(char c){
    /*
    Configures LCD data word

    c - LCD data word, obviously ;)
    */
    write_4bit(c, DATA_MODE); //1 for data mode
}