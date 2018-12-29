// This put us in multi threading mode where the system thread that handles wifi, etc is on a separate thread from loop
// This means the setup will start immediately
SYSTEM_THREAD(ENABLED);

// Here we declare a cloud funtion that can be called from the console or using postman to cause the device to enter
// dfu mode. This is nice because we can enter DFU mode to do a compile and download locally without touching the device
// and holding down both buttons, then releasing reset and then setup to get into DFU mode
bool success = Particle.function("enterDFU", enterDFU);

int enterDFU(String something) {
  System.dfu();
}

// Here we can create a new Thread. OMG, hooray!
Thread* t1;
// Here we can create a new Mutex which is also awesome!
Mutex* m1;

// This is the function that will be run as the thread t1
// Its goal is to take over the rgb LED built into the photon, temporarily and turning the led either solid blue or red
// It will alternate each time the mutex lock is cleared which is done in the loop thread
os_thread_return_t test() {
  bool isBlue = false;
  while (1) {
    m1->lock(); // this will suspend the thread if the lock is currently held.
    if (!isBlue) {
      RGB.control(true);
      RGB.color(0,0,255);
      delay(3000);
      RGB.control(false);
    } else {
      RGB.control(true);
      RGB.color(255,0,0);
      delay(3000);
      RGB.control(false);
    }
    isBlue = !isBlue;
  }
}
void setup() {

  // Lets fire up the thread t1 and the mutex. Note that the mutex is used right away by the thread so propbaby should
  // initialize the mutex first.
  m1 = new Mutex();
  t1 = new Thread("testThread", test);
}

// So loop is the "main application thread" but is really now a thread just like t1
// It runs every 5 seconds publishing a message to the console but also unlocking the mutex
void loop() {

  m1->unlock();
  Particle.publish("message", "Hello World!");
  delay(10000);

}