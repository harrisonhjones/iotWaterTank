// IoT Water Tank
// Author: Harrison Jones
// Description: Main logic for the IoT Water Tank
// Change Log
// JUN 24 2016 - Initial Release

#include "clickButton.h"        // https://github.com/pkourany/clickButton/
#include "elapsedMillis.h"      // https://github.com/pkourany/elapsedMillis

// Level Switch
elapsedMillis timerLeveLSwitchDebounce; 
unsigned int debounceTime = 1000;
#define PIN_LEVEL_SWITCH D0
#define LEVEL_SWITCH_LOW LOW
#define LEVEL_SWITCH_HIGH HIGH
int levelSwitchState = LEVEL_SWITCH_HIGH;   // Debounced levelSwitchState

// Buzzer
#define PIN_BUZZER D1
#define BUZZER_ON HIGH
#define BUZZER_OFF LOW
bool buzzerToggle = false;
elapsedMillis timerBuzzer; /
bool buzzerSilenced = false;

// Buzzer Silence Button
#define PIN_BUTTON_1 D2
ClickButton btn1(PIN_BUTTON_1, LOW, CLICKBTN_PULLUP);
int btn1Result = 0;

// Main Logic
void setup() {
    pinMode(PIN_LEVEL_SWITCH, INPUT_PULLUP);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    
    btn1.debounceTime   = 20;   // Debounce timer in ms
    btn1.multiclickTime = 250;  // Time limit for multi clicks
    btn1.longClickTime  = 1000; // time until "held-down clicks" register
    
    timerLeveLSwitchDebounce = 0;  // Reset the timer
}

void loop() {

    // Update the multipurpose button for reference throughout the rest of the code. 
    btn1.Update();
    if(btn1.clicks != 0) btn1Result = btn1.clicks;
    
    // Check the latest state of the level switch. If it differs from the "known truth" see if the debounce timer has elapsed. If so, consider it the "new truth". If not, don't do anything. If the latest switch state is the same reset the timer.
    int tempLevelSwitchState = digitalRead(PIN_LEVEL_SWITCH);
    if(tempLevelSwitchState != levelSwitchState) {
        if(timerLeveLSwitchDebounce > debounceTime) {
            levelSwitchState = tempLevelSwitchState;
            // Do other state transition stuff here like sending an event.
        }
    } else {
        timerLeveLSwitchDebounce = 0;
    }
    
    
    // Check state of the debounced level switch. If it's low then try to do a number of things
    // If the buzzer has not been silenced then buzz
    // If the button was single pressed then silence the buzzer until the next time the tank transitions from high to low
    if (levelSwitchState  == LEVEL_SWITCH_LOW) {
        if(!buzzerSilenced) {
            if(btn1Result == 1) {   
                buzzerSilenced = true;
            } else {
                // If buzzer is not silenced and the button was not pressed to silence the button
                buzzerOn();
            }
        } else {
            // If the buzzer is silenced turn off the buzzer
            buzzerOff();
        }
    } else {
        // If the state of the level switch is HIGH then silnce the buzzer and reset the buzzerSilenced flag so the buzzer will buzz again next time the level switch goes low 
        buzzerOff();
        buzzerSilenced = false;
    }
      
    // Reset the button result and do a slight delay (may not be necessary)
    btn1Result = 0;
    delay(5);
}


// Turn on the buzzer. If the buzzer timer has elapsed (> 500 ms) then toggle the buzzer state (on vs off) and reset the timer. This achieves a alternating buzzer sound.
void buzzerOn() {
    if(timerBuzzer > 500)
    {
        buzzerToggle = !buzzerToggle;

        if(buzzerToggle)
            tone(PIN_BUZZER, 4000, 0);
        else
            noTone(PIN_BUZZER);

        timerBuzzer = 0;
    }    
}

// Turn off the buzzer
void buzzerOff() {
    noTone(PIN_BUZZER);
}