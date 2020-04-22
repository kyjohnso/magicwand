#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>

int buttonPin = 21;
int buttonState = 0;
int lastButtonState = 0;
int firstvec = 0;
int switchPin0 = 20;
int switchPin1 = 19;
int command_int = 0;
int i;
const int pinArray[4] = {9,10,11,13};

/* Assign a unique ID to this sensor at the same time */
Adafruit_FXAS21002C gyro = Adafruit_FXAS21002C(0x0021002C);
Adafruit_FXOS8700 accelmag = Adafruit_FXOS8700(0x8700A, 0x8700B);

void displaySensorDetails(void)
{
    sensor_t sensor;
    gyro.getSensor(&sensor);
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(sensor.name);
    Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
    Serial.print  ("Unique ID:    0x"); Serial.println(sensor.sensor_id, HEX);
    Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); 
    Serial.println(" rad/s");
    Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); 
    Serial.println(" rad/s");
    Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); 
    Serial.println(" rad/s");
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);

    sensor_t accel, mag;
    accelmag.getSensor(&accel, &mag);
    Serial.println("------------------------------------");
    Serial.println("ACCELEROMETER");
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(accel.name);
    Serial.print  ("Driver Ver:   "); Serial.println(accel.version);
    Serial.print  ("Unique ID:    0x"); Serial.println(accel.sensor_id, HEX);
    Serial.print  ("Min Delay:    "); Serial.print(accel.min_delay); 
    Serial.println(" s");
    Serial.print  ("Max Value:    "); Serial.print(accel.max_value, 4); 
    Serial.println(" m/s^2");
    Serial.print  ("Min Value:    "); Serial.print(accel.min_value, 4); 
    Serial.println(" m/s^2");
    Serial.print  ("Resolution:   "); Serial.print(accel.resolution, 8); 
    Serial.println(" m/s^2");
    Serial.println("------------------------------------");
    Serial.println("");
    Serial.println("------------------------------------");
    Serial.println("MAGNETOMETER");
    Serial.println("------------------------------------");
    Serial.print  ("Sensor:       "); Serial.println(mag.name);
    Serial.print  ("Driver Ver:   "); Serial.println(mag.version);
    Serial.print  ("Unique ID:    0x"); Serial.println(mag.sensor_id, HEX);
    Serial.print  ("Min Delay:    "); Serial.print(accel.min_delay); 
    Serial.println(" s");
    Serial.print  ("Max Value:    "); Serial.print(mag.max_value); 
    Serial.println(" uT");
    Serial.print  ("Min Value:    "); Serial.print(mag.min_value); 
    Serial.println(" uT");
    Serial.print  ("Resolution:   "); Serial.print(mag.resolution); 
    Serial.println(" uT");
    Serial.println("------------------------------------");
    Serial.println("");
    delay(500);

}

void setup(void)
{
    Serial.begin(9600);

    gyro.begin();
    accelmag.begin();

    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);
    pinMode(13, OUTPUT);

    i = 0;
    Serial.println("[");

}

void loop(void)
{
    buttonState = digitalRead(buttonPin);
    if (buttonState != lastButtonState) {
        if (buttonState == HIGH) {
            /* get the pin setting */
            command_int = digitalRead(switchPin0)+2*digitalRead(switchPin1);
            Serial.print("{\"command\": "); Serial.print(command_int);
            Serial.println(", \"vectors\": [");
            firstvec = 1;
        } else {
            Serial.println("]},");
        }
    }
      if (buttonState == HIGH) {
        /* Get a new sensor event */
        sensors_event_t event;
        gyro.getEvent(&event);
        
        sensors_event_t aevent, mevent;
        accelmag.getEvent(&aevent, &mevent);

        /* Display the results (speed is measured in rad/s) */
        if (firstvec == 1) {
          Serial.print("[");
          firstvec = 0;
        } else {
          Serial.print(",[");
        }
        Serial.print(aevent.acceleration.x, 4); 
        Serial.print(", ");
        Serial.print(aevent.acceleration.y, 4); 
        Serial.print(", ");
        Serial.print(aevent.acceleration.z, 4); 
        Serial.print(", ");
        Serial.print(event.gyro.x); 
        Serial.print(", ");
        Serial.print(event.gyro.y); 
        Serial.print(", ");
        Serial.print(event.gyro.z); Serial.println("]"); 
    }
    lastButtonState = buttonState;
    delay(200);
}
