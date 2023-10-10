#include <iostream>
#include "mbed.h"
#include "time.h"
#include "LCD_and_temp.h"
#include "Egg_doneness.h"

DigitalIn browse(D8);
DigitalIn confirm(D7);
DigitalOut relay(D4);
BusOut leds(D5, D6); //D5 is red (oven ON), D6 is green (water is boiling)
void browsing();
void count_down(double &duration, double &elapsed_time, time_t &start);
void preheating(const char *doneness, float maxtemp);
void selection(const char *modenumber);
void stove_off(int cool_count);
void threshold_transitioning(float maxtemp, bool &at_temp, float &cool_count, time_t &start, time_t &cool_start);

// main() runs in its own thread in the OS

Mutex lcd_mutex;
float cool_count;
bool so;
Thread thread1;
void stove_off(void const *args){
    /*
    Turns off the stove upon error or cooking finishing

    so - boolean variable representing when to enable thread's contents to run
    */
    while(1){
        if (so){
            lcd_mutex.lock();
            if (cool_count>=10){
                new_line(0x94, "ERROR  "); //cursor to 3rd line
                new_line(0xD4, "Temp Unsustainable"); //cursor to 4th line
                play_tune(3);
            }
            else{
                new_line(0x94, "DONE   "); //cursor to 3rd line
                new_line(0xD9, "0 Seconds      "); //cursor to 4th line
                play_tune(0);
            }
            lcd_mutex.unlock();
            so=false;
        }
    }
}

void browsing(){
    /*
    Handles user toggling between egg donenesses, from soft boil to hard
    */

    int i=0;
    bool last_state=false;
    while (!confirm){
        if (browse && !last_state){
            i+=1;
            if (i%3==0) doneness_ptr=&Soft;
            else if (i%3==1) doneness_ptr=&Medium;
            else doneness_ptr=&Hard;
            clr_lcd();
            print_lcd("Choose Doneness:");
            new_line(0xC0, doneness_ptr->name); //cursor to 2nd line
            new_line(0x94, "> Browse"); //cursor to 3rd line
            new_line(0xD4, "> Confirm"); //cursor to 4th line
        }
        last_state=browse;
        thread_sleep_for(100);
    }
}

void count_down(double &duration, double &elapsed_time, time_t &start){
    /*
    Decrements the time left until the egg has reached the desired doneness

    duration - Total egg cooking time needed
    elapsed_time - How long egg has cooked for so far
    start - Timestamped moment in time where cooking started
    */

    int time_left=60*duration-elapsed_time;
    char countdown[2];
    char units[11];
    int unitsize;
    if (time_left>=60){
        time_left=ceil(time_left/60); //change countdown from minutes to seconds
        strcpy(units, " Minutes ");
    }
    else strcpy(units, " Seconds ");

    sprintf(countdown, "%d", time_left);
    if (time_left>=10) unitsize=2;
    else unitsize=1;

    write_cmd(0xD9); //cursor to 4th line, 0->1->2->3->4->5th column
    for (char i=0;i<unitsize;i++){ //write characters from string to LCD
        write_data(countdown[i]);
    }
    print_lcd(units);
    thread_sleep_for(100);
    elapsed_time = static_cast<double> (difftime( time(0), start));
}

void preheating(const char *doneness, float maxtemp){
    /*
    Function that runs until the egg in water reaches cooking temperature

    doneness - The doneness level the user selected
    maxtemp - The temp the water w/ egg must reached to surpass preheating
    */

    float temp=0;
    relay=1;
    leds=1; //red LED on only
    clr_lcd();
    print_lcd(doneness);
    new_line(0xC0, "===================="); //cursor to 2nd line
    new_line(0x94, "PREHEATING"); //cursor to 3rd line
    while(temp < maxtemp || temp >= 384) temp = read_temperature(true);
}

void selection(const char *modenumber){
    /*
    Displays the egg doneness that the user selected

    modenumber - Matches the egg doneness that the user selected
    */

    bool laststate = false;
    int i = 0;
    clr_lcd();
    print_lcd("Choose Doneness:");
    new_line(0xC0, modenumber); //cursor to 2nd line
    new_line(0x94, "> Browse"); //cursor to 3rd line
    new_line(0xD4, "> Confirm"); //cursor to 4th line
}

void threshold_transitioning(float maxtemp, bool &at_temp, float &cool_count, time_t &start, time_t &cool_start){
    /*
    Switch from the cooking time falling to pausing in case of temp volatility

    maxtemp - The temp the water w/ egg must reached to surpass preheating
    at_temp - Egg in water at cooking temperature if True
    cool_count - Number of seconds where temperature fell beneath cooking temp
    start - Timestamp that tracks seconds passed where cooking temp was OK
    cool_start - Timestamp tracks seconds passed where cooking temp was not OK
    */

    float temp=read_temperature(false);
    if (temp < maxtemp){
        if (at_temp){
            cool_start = time(0);
            at_temp=false;
            cool_count=0;
        }
        double cool_end = difftime(time(0), cool_start);
        start+=cool_end;
        cool_count+=cool_end;
        cool_start = time(0);
    }
    else at_temp=true;
}

int main(){
    so=false;
    thread1.start(callback(stove_off,&so));
    init_lcd();
    while (true) {
        float maxtemp=150;
        bool at_temp=false;
        int i=0;
        relay=0, leds=0;
        doneness_ptr=&Soft;
        clr_lcd();
        selection(doneness_ptr->name);
        browsing();
        preheating(doneness_ptr->name, maxtemp);
        leds=3; //red & green LED on
        new_line(0x94, "COOKING   "); //cursor to 3rd line and extra spaces to cover the prior word, "PREHEATING"
        time_t start = time(0);
        double elapsed_time = difftime(time(0), start);
        new_line(0xD4, "ETA: "); //cursor to 4th line
        cool_count=0;
        time_t cool_start;
        while (cool_count<10 && elapsed_time < 60*doneness_ptr->duration){
            threshold_transitioning(maxtemp, at_temp, cool_count, start, cool_start);
            count_down(doneness_ptr->duration, elapsed_time, start);
        }
        relay=0, leds=0;
        so=true;
        thread_sleep_for(2000);
    }
}