#include <SparkFun_ADXL345.h>
#include <Keyboard.h>
#include <Gamepad.h>
#include <SoftPWM.h>

constexpr float PLAY_ANGLE_FULL_LOCK = 60.0 / 180.0;  // ±60° is full lock 

const long NORMAL_PERIOD = 10000; // ms
const long NORMAL_PERIOD_VARIANCE = 5000; // ms 

const long REVERSE_PERIOD = 5000; // ms
const long REVERSE_PERIOD_VARIANCE = 2500; // ms

const int LED_TOP_FAR_LEFT = A3;
const int LED_TOP_LEFT = 7;

const int LED_BOTTOM_FAR_LEFT = 6;
const int LED_BOTTOM_LEFT = 9;

const int LED_TOP_FAR_RIGHT = A0;
const int LED_TOP_RIGHT = 4;

const int LED_BOTTOM_FAR_RIGHT = A2;
const int LED_BOTTOM_RIGHT = 3;

const int LED_CENTER_LEFT = 8;
const int LED_CENTER_RIGHT = 5;

const int BUTTON_PIN = 2;

ADXL345 adxl = ADXL345(10);
Gamepad gp;

void setup() {
  Serial.begin(9600); 
    
  setup_gpios();
  setup_accelerometer();
  randomSeed(analogRead(A1));
}

void setup_accelerometer() {
  adxl.powerOn();
  adxl.setRangeSetting(2);  // 2g, 4g, 8g or 16g
  adxl.setSpiBit(0);  // use 4 wire SPI
}

void setup_gpios() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SoftPWMBegin(SOFTPWM_NORMAL);
  
  SoftPWMSet(LED_TOP_FAR_LEFT, 0);
  SoftPWMSet(LED_TOP_LEFT, 0);
  SoftPWMSet(LED_BOTTOM_FAR_LEFT, 0);
  SoftPWMSet(LED_BOTTOM_LEFT, 0);
  SoftPWMSet(LED_TOP_FAR_RIGHT, 0);
  SoftPWMSet(LED_TOP_RIGHT, 0);
  SoftPWMSet(LED_BOTTOM_FAR_RIGHT, 0);
  SoftPWMSet(LED_BOTTOM_RIGHT, 0);
  SoftPWMSet(LED_CENTER_LEFT, 0);
  SoftPWMSet(LED_CENTER_RIGHT, 0);

  int fadeUpTime = 50;
  int fadeDownTime = 100;
  
  SoftPWMSetFadeTime(LED_TOP_FAR_LEFT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_TOP_LEFT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_BOTTOM_FAR_LEFT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_BOTTOM_LEFT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_TOP_FAR_RIGHT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_TOP_RIGHT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_BOTTOM_FAR_RIGHT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_BOTTOM_RIGHT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_CENTER_LEFT, fadeUpTime, fadeDownTime);
  SoftPWMSetFadeTime(LED_CENTER_RIGHT, fadeUpTime, fadeDownTime);
}

void read_accelerometer_calibrated(float &x_val, float &y_val, float &z_val) {
  // Accelerometer Readings
  int x,y,z;   
  adxl.readAccel(&x, &y, &z);

  // Calibrate values
  x_val = (x - 2.5)/33.5; //-2.5
  y_val = (y - 0.0)/34.0;
  z_val = (z + 4.0)/32.0;
}

float get_steering_angle(float x, float y) {
  // Calculate angle about z-axis with four-quadrant inverse tangent
  float angle = atan2(x ,-y);

  // Limit steering angle
  angle /= PLAY_ANGLE_FULL_LOCK; 
  if(angle > PI) angle = PI;
  if(angle < -PI) angle = -PI;

  // Scale to [-1, 1]
  return angle / PI;
}

bool is_in_playing_position(float y, float z) {
  // Calculate angle about x-axis with four-quadrant inverse tangent
  float angle = atan2(y ,z);

  // Not flat on table
  return abs(angle) > PI/6;
}

void blink_in_direction(float angle) {
  static long lastIntervalMillis = 0;
  if(lastIntervalMillis + 75 < millis()) {
    lastIntervalMillis = millis();

    static int row = 2;
    if(angle < 0) {
      if(--row < 0) row = 4;
    }
    else {
      if(++row > 4) row = 0;
    }
    
    SoftPWMSet(LED_TOP_FAR_LEFT, row == 0 ? 255 : 0);
    SoftPWMSet(LED_BOTTOM_FAR_LEFT, row == 0 ? 255 : 0);
    
    SoftPWMSet(LED_TOP_LEFT, row == 1 ? 255 : 0);
    SoftPWMSet(LED_BOTTOM_LEFT, row == 1 ? 255 : 0);
  
    SoftPWMSet(LED_CENTER_LEFT, row == 2 ? 255 : 0);
    SoftPWMSet(LED_CENTER_RIGHT, row == 2 ? 255 : 0);
    
    SoftPWMSet(LED_BOTTOM_RIGHT, row == 3 ? 255 : 0);
    SoftPWMSet(LED_TOP_RIGHT, row == 3 ? 255 : 0);
  
    SoftPWMSet(LED_TOP_FAR_RIGHT, row == 4 ? 255 : 0);
    SoftPWMSet(LED_BOTTOM_FAR_RIGHT, row == 4 ? 255 : 0);
  }
}

void blink_random() {
  static long lastIntervalMillis = 0;
  if(lastIntervalMillis + 75 < millis()) {
    lastIntervalMillis = millis();

    int rnd = random(10);
    SoftPWMSet(LED_TOP_FAR_LEFT, rnd == 0 ? 255 : 0);
    SoftPWMSet(LED_BOTTOM_FAR_LEFT, rnd == 1 ? 255 : 0);
    
    SoftPWMSet(LED_TOP_LEFT, rnd == 2 ? 255 : 0);
    SoftPWMSet(LED_BOTTOM_LEFT, rnd == 3 ? 255 : 0);
  
    SoftPWMSet(LED_CENTER_LEFT, rnd == 4 ? 255 : 0);
    SoftPWMSet(LED_CENTER_RIGHT, rnd == 5 ? 255 : 0);
    
    SoftPWMSet(LED_BOTTOM_RIGHT, rnd == 6 ? 255 : 0);
    SoftPWMSet(LED_TOP_RIGHT, rnd == 7 ? 255 : 0);
  
    SoftPWMSet(LED_TOP_FAR_RIGHT, rnd == 8 ? 255 : 0);
    SoftPWMSet(LED_BOTTOM_FAR_RIGHT, rnd == 9 ? 255 : 0);
  }
}

void set_all_leds(boolean value) {
  SoftPWMSet(LED_TOP_FAR_LEFT, value ? 255 : 0);
  SoftPWMSet(LED_BOTTOM_FAR_LEFT, value ? 255 : 0);
  
  SoftPWMSet(LED_TOP_LEFT, value ? 255 : 0);
  SoftPWMSet(LED_BOTTOM_LEFT, value ? 255 : 0);

  SoftPWMSet(LED_CENTER_LEFT, value ? 255 : 0);
  SoftPWMSet(LED_CENTER_RIGHT, value ? 255 : 0);
  
  SoftPWMSet(LED_BOTTOM_RIGHT, value ? 255 : 0);
  SoftPWMSet(LED_TOP_RIGHT, value ? 255 : 0);

  SoftPWMSet(LED_TOP_FAR_RIGHT, value ? 255 : 0);
  SoftPWMSet(LED_BOTTOM_FAR_RIGHT, value ? 255 : 0);
}

void loop() {
  static bool reverseMode = false;
  static bool reverse = false;
  static long nextSwitch = 0;
  static long buttonDownMillis = 0;
  static bool buttonStateOld = false;

  // Get button state
  int buttonState = !digitalRead(BUTTON_PIN);

  if(buttonStateOld == false && buttonState == true) {
    buttonDownMillis = millis();
  }

  if(buttonState == false) {
    buttonDownMillis = 0;
  }

  if(buttonState == true && buttonDownMillis != 0 && buttonDownMillis + 5000 < millis()) {
    reverseMode = !reverseMode; 
    buttonDownMillis = 0;
  }

  buttonStateOld = buttonState;

  // Get calibrated accelerometer values
  float x, y, z;
  read_accelerometer_calibrated(x, y, z);

  // Get controller steering angle
  float angle = get_steering_angle(x, y); 

  // Get if controller is in playing position and not just lying around
  bool playing = is_in_playing_position(y, z);

  if(playing) {
    if(reverseMode) {
      if(buttonState) {
        nextSwitch = millis() + NORMAL_PERIOD + random(-NORMAL_PERIOD_VARIANCE, NORMAL_PERIOD_VARIANCE);
        reverse = false;
      }
      
      if(nextSwitch < millis()) {
        if(reverse) {
          nextSwitch = millis() + NORMAL_PERIOD + random(-NORMAL_PERIOD_VARIANCE, NORMAL_PERIOD_VARIANCE);
        } else {
          nextSwitch = millis() + REVERSE_PERIOD + random(-REVERSE_PERIOD_VARIANCE, REVERSE_PERIOD_VARIANCE);
        }
        reverse = !reverse;
      }

      if(reverse) {
        blink_random();
        angle *= -1;
      } else {
        set_all_leds(HIGH);
      }
    
    } else {
      if(abs(angle) > 0.2) {
        blink_in_direction(angle);
      } else {
        set_all_leds(HIGH);
      }
    }
    
    // Set gamepad axis:
    gp.setLeftXaxis(angle * 127.0);
  } else {
    set_all_leds(LOW);
  }

  // Set gamepad button
  gp.setButtonState(0, buttonState);
}
