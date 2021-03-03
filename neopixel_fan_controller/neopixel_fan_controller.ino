#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

#define TIMEOUT_MS 500
#define LED_PIN    3
#define LED_COUNT 18
#define CYCLE_SPEED 25      //[1/ms]
#define CYCLE_INTERVALL 10  //[ms]    //results in 250 per cycle

const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = 0;
volatile unsigned long timeout;
int addr = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);  //generate the NeoPixel 'strip' object

//Initialize some variables for the NeoPixel stuff
uint32_t weiss = strip.gamma32(strip.ColorHSV(6000, 75, 255));
uint32_t turkis = strip.Color(0, 192, 221);
uint32_t rot = strip.Color(255, 0, 0);
uint32_t pink = strip.Color(255, 0, 255);
uint32_t rgbcolor = 0;
uint16_t rgb_offset = 0;    
uint8_t i = 0;          

void setup() {
  //NeoPixel Setup
  strip.begin();    //Initialize the Strip
  //strip.setBrightness(215);
  strip.show();     //Display nothing (to prevent the LEDs from displaying stuff from before this programm)
  
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);    //Set the Built-in LED to Output mode
  digitalWrite(13, LOW);  //Switch the Built-in LED Off

  //Button Setup
  pinMode(interruptPin, INPUT_PULLUP);    //Enable Pullup for the Pin
  attachInterrupt(digitalPinToInterrupt(interruptPin), changeMode, FALLING);  //Interrupt Function is called each time the Button is pressed
  
  state = EEPROM.read(addr);  //Restore the last known state from the EEPROM memory
}



void loop() {
  switch(state){
    case 0:   //LEDs OFF
      strip.clear();
      break;

    case 1:   //All LEDs are white
      strip.fill(weiss);
      break;

    case 2:   //All LEDs are Turquoise
      strip.fill(turkis);
      break;

    case 3:   //Unicorn puke rainbow cycle
      for(int i = 0; i < LED_COUNT; i++){
        rgbcolor = strip.gamma32(strip.ColorHSV(rgb_offset + (65536*i)/LED_COUNT)); //calculate the LEDs color based on its number
        strip.setPixelColor(i, rgbcolor);   //Assign the color to the LED
      }
        rgb_offset = rgb_offset + CYCLE_SPEED*CYCLE_INTERVALL;  //'rotate' the color circle by 400 each executio 
      break;

    case 4:
      if(i >= LED_COUNT){
        i = 0;
      }
    strip.clear();
    strip.setPixelColor(i, turkis);
    strip.setPixelColor(((LED_COUNT)/2 + i)%LED_COUNT, turkis);
    i++;
    delay(17);  //additional delay to slow the animation down
    break;
      
    case 5:
      strip.clear();
      //strip.fill(pink);
      //strip.fill(turkis, 1, 14);
      strip.setPixelColor(0, pink);
    break;
  
    default:    //ERROR - State out of bounds
      strip.fill(rot);
  }
  delay(CYCLE_INTERVALL/2);      //Delay the executuion
  strip.show();   //Update the LED color
  delay(CYCLE_INTERVALL/2);      //Delay the executuion

}

void changeMode(){    //Interrupt Function to change the LED Mode once the switch is pressed
  if(timeout <= millis()){          //Timeout to debounce the Button
    timeout = millis() + TIMEOUT_MS;
    state++;
    if(state >= 5){
      state = 0;
    }
    EEPROM.write(addr, state);      //Write the current state to the EEPROM memory to save the state during power loss
  }
}
