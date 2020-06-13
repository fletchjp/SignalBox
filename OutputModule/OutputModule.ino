/** OutputModule.
 */


#include <Servo.h>
#include <Wire.h>


#define I2C_BASE_ID    0x50   // Output nodes' base ID.
#define DELAY_STEP       50   // Delay (msecs) between steps of a Servo.

#define SERVO_MAX         8   // Maximum number of servos
#define SERVO_BASE_PIN    4   // Servos attached to this pin and the next 7 more.

#define JUMPER_PINS       4   // Four jumpers.
#define IO_PINS           8   // Eight IO pins.


// The i2c ID of the module.
uint8_t moduleID = 0;

// The module jumpers
uint8_t jumpers[JUMPER_PINS] = { 1, 0, A6, A7 };

// The digital IO pins.
uint8_t ioPins[IO_PINS]      = { 3, 2, A3, A2, A1, A0, 13, 12 };

// An Array of Servo control structures.
struct 
{
  Servo   servo;            // The Servo.
  uint8_t start  = 0;       // The angle we started at.
  uint8_t target = 0;       // The angle we want to reach.
  uint8_t steps  = 0;       // The number of steps to take.
  uint8_t step   = 0;       // The current step.
} servos[SERVO_MAX];


/** Setup the Arduino.
 */
void setup()
{
  Serial.begin(19200);           // Serial IO.

  // Configure the Jumper pins for input.
  for (int pin = 0; pin < JUMPER_PINS; pin++)
  {
    pinMode(pin, INPUT_PULLUP);
  }

  // Configure the IO pins for output.
  for (int pin = 0; pin < IO_PINS; pin++)
  {
    pinMode(pin, OUTPUT);
  }

  // Configure i2c from jumpers.
  for (int pin = 0; pin < JUMPER_PINS; pin++)
  {
    moduleID |= digitalRead(ioPins[pin]) << pin;
  }
  moduleID |= I2C_BASE_ID;

  // Attach all servos to their pins.
  for (int i = 0, pin = SERVO_BASE_PIN; i < SERVO_MAX; i++, pin++)
  {
    pinMode(pin, OUTPUT);
    servos[i].servo.attach(pin);
    delay(20);
  }

  // Start i2c communications.
  Wire.begin(moduleID);
  Wire.onReceive(processRequest);

  Serial.print("Module ID: 0x");
  Serial.println(moduleID, HEX);

  // Test-move a Servo
  moveServo(0, 180, 127, 1);
}


/** Upon receipt of a request, store it in the corresponding Servo's state.
 */
void processRequest(int aRequests)
{
  uint8_t pin   = Wire.read();
  uint8_t angle = Wire.read();
  uint8_t pace  = Wire.read();
  uint8_t state = Wire.read();   

  moveServo(pin, angle, pace, state);
}


/** Move a Servo from its current position to the desired one
 *  at the pace indicated.
 */
void moveServo(uint8_t aServo, uint8_t aTarget, uint8_t aPace, uint8_t aState)
{
  // Set the Servo's movement.
  servos[aServo].start  = servos[aServo].servo.read();
  servos[aServo].target = aTarget;
  servos[aServo].steps  = (127 - aPace) + 1;
  servos[aServo].step   = 0;

  // Action the IO flag immediately.
  digitalWrite(ioPins[aServo], aState);

  Serial.print(millis());
  Serial.print("\tMove: servo=");
  Serial.print(aServo);
  Serial.print(", start=");
  Serial.print(servos[aServo].start);
  Serial.print(", angle=");
  Serial.print(servos[aServo].servo.read());
  Serial.print(", target=");
  Serial.print(servos[aServo].target);
  Serial.print(", pace=");
  Serial.print(aPace);
  Serial.print(", steps=");
  Serial.print(servos[aServo].steps);
  Serial.print(", state=");
  Serial.print(aState);
  Serial.println();
}


/** Main loop.
 */
void loop()
{
  int angle;
  
  // Move any Servos that need moving.
  for (int servo = 0; servo < SERVO_MAX; servo++)
  {
    if (servos[servo].step < servos[servo].steps)
    {
      servos[servo].step += 1;
      angle = servos[servo].start 
            +   (servos[servo].target - servos[servo].start)
              * servos[servo].step / servos[servo].steps;
      servos[servo].servo.write(servos[servo].start + (servos[servo].target - servos[servo].start) * servos[servo].step / servos[servo].steps);

      if (   (servos[servo].step == 1)
          || (servos[servo].step == servos[servo].steps))
      {
        Serial.print(millis());
        Serial.print("\tStep: servo=");
        Serial.print(servo);
        Serial.print(", start=");
        Serial.print(servos[servo].start);
        Serial.print(", angle=");
        Serial.print(servos[servo].servo.read());
        Serial.print(", target=");
        Serial.print(servos[servo].target);
        Serial.print(", step=");
        Serial.print(servos[servo].step);
        Serial.print(", steps=");
        Serial.print(servos[servo].steps);
        Serial.println();
      }
      
      // Test code to move servo back to zero when complete.
      if (servos[servo].step == servos[servo].steps)
      {
        if (servos[servo].target != 0)
        {
          moveServo(servo, 0, 0, 0);
        }
      }
    }
  }

  // Wait a clock tick.
  delay(DELAY_STEP);
}
