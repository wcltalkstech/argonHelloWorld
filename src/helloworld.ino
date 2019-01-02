// This put us in multi threading mode where the system thread that handles wifi, etc is on a separate thread from loop
// This means the setup will start immediately
SYSTEM_THREAD(ENABLED);

#include "neopixel.h"
#define PIXEL_PIN D3
#define PIXEL_COUNT 1
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// Here we can create a new Thread. OMG, hooray!
Thread* t1;
Thread* neo;

// Here we can create a new Mutex which is also awesome!
Mutex* m1;
Timer timer1(10000, signalEvery10Seconds);

void signalEvery10Seconds() {
  m1->unlock();
  Particle.publish("message", "Hello World!");
}

os_thread_return_t neoRainbow() {
  while (1) {
    rainbow(2);
  }
}
// This is the function that will be run as the thread t1
// Its goal is to take over the rgb LED built into the photon, temporarily and turning the led either solid blue or red
// It will alternate each time the mutex lock is cleared which is done in the loop thread
os_thread_return_t test() {
  bool isBlue = false;
  while (1) {
    m1->lock(); // this will suspend the thread if the lock is currently held.
    Serial.printlnf("localIP: %s", Mesh.localIP().toString().c_str());
    if (!isBlue) {
      Serial.println("setting blue");
      RGB.control(true);
      RGB.color(0,0,255);
      delay(3000);
      RGB.control(false);
    } else {
      Serial.println("setting red");
      RGB.control(true);
      RGB.color(255,0,0);
      delay(3000);
      RGB.control(false);
    }
    isBlue = !isBlue;
  }
}
void setup() {

  strip.begin();
  strip.show();
  Serial.begin(9600);
  // Lets fire up the thread t1 and the mutex. Note that the mutex is used right away by the thread so propbaby should
  // initialize the mutex first.

  m1 = new Mutex();
  t1 = new Thread("testThread", test);
  neo = new Thread("neoThread", neoRainbow);

  timer1.start();

}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// So loop is the "main application thread" but is really now a thread just like t1
// It runs every 5 seconds publishing a message to the console but also unlocking the mutex
void loop() {

}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}