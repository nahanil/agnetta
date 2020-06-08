#include <Adafruit_NeoPixel.h>

#define LED_PIN 6
#define BTN_A_PIN 7
#define BTN_B_PIN 8
#define BTN_C_PIN 9
#define MOTOR_PIN 12

// Function modes
#define MODE_RAINBOW 1
#define MODE_SOLID 2

// Event loop time
#define TICK 50
#define DEBOUNCE_TIME 200

Adafruit_NeoPixel strip = Adafruit_NeoPixel(23 * 3, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t color;
int mode = MODE_RAINBOW;
int i = 0;
int j = 0;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BTN_A_PIN, INPUT);
//  pinMode(BTN_B_PIN, INPUT);
//  pinMode(BTN_C_PIN, INPUT);

  strip.begin();
  strip.setBrightness(50);
  strip.show();

  digitalWrite(MOTOR_PIN, LOW);

  color = randColor();
  setMode(MODE_SOLID);
}

void loop() {
  long start = millis();

  button_handler();
  on_tick();
  
  long loopdelay = millis() - start + TICK;
  if (loopdelay > 0) {
    delay(loopdelay);
  }
}

void on_tick () {
  // Update lights
  if (mode == MODE_RAINBOW) {
    tick_rainbow();  
  }
  else if (mode == MODE_SOLID) {
    tick_solid();
  }
}

void tick_solid() {  
  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

// Slightly different, this makes the rainbow equally distributed throughout
void tick_rainbow() {
  if (j > 256 * 5) { j = 0; }

  for(i=0; i< strip.numPixels(); i++) {
    strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
  }
  strip.show();

  j++;
}

void setMode (int m) {
  mode = m;
  i = 0;
  j = 0;
}

static unsigned long last_interrupt_a = 0;
static unsigned long last_interrupt_b = 0;
static unsigned long last_interrupt_c = 0;

void button_handler () {
  unsigned long interrupt_time = millis();

  if (digitalRead(BTN_A_PIN) == HIGH) {
    if (interrupt_time - last_interrupt_a > DEBOUNCE_TIME) {
      last_interrupt_a = interrupt_time;
      on_btn_a();
    }
  }

  if (digitalRead(BTN_B_PIN) == HIGH) {
    if (interrupt_time - last_interrupt_b > DEBOUNCE_TIME) {
      last_interrupt_b = interrupt_time;
      on_btn_b();
    }
  }

  if (digitalRead(BTN_C_PIN) == HIGH) {
    if (interrupt_time - last_interrupt_c > DEBOUNCE_TIME) {
      last_interrupt_c = interrupt_time;
      on_btn_c();
    }
  }
}

uint8_t _mode = 0;
void on_btn_a () {
  // motor on/off
  if (_mode == 0 || _mode == 1) {
    digitalWrite(MOTOR_PIN, LOW);
  } else {
    digitalWrite(MOTOR_PIN, HIGH);
  }

  // rainbow/solid mode
  if (_mode == 0 || _mode == 2) {
    color = randColor();
    setMode(MODE_SOLID);
  } else {
    setMode(MODE_RAINBOW);
  }

  // Incr
  if (_mode >= 3) {
    _mode = 0;
  } else {
    _mode = _mode + 1;
  }
}

void on_btn_b () {
  strip.setBrightness(
    (strip.getBrightness() + 50) % 255
  );
  strip.show();
}

void on_btn_c () {
  color = randColor();
}

uint32_t randColor () {
  return strip.Color(random(0, 254), random(0, 254), random(0, 254));
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel (byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
