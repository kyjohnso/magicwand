#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_FXAS21002C.h>
#include <Adafruit_FXOS8700.h>

int buttonPin = 21;

float accum;
int hold_out = 0;
int counter = 0;
int i, j, p, q, r, command;

const int pinArray[4] = {9,10,11,13};

const int n_lookback = 6;
const int n_features = 6;
const int n_commands = 5; 

float input[n_lookback*n_features] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

float output[n_commands] = {
    0.0, 
    0.0, 
    0.0, 
    0.0, 
    0.0};

float weights[n_lookback*n_features][n_commands] = {
    {-0.353458,0.183684,-0.286829,0.196774,0.681830},
    {-0.090410,-0.172012,-0.180314,0.464083,0.005013},
    {0.162166,-0.008464,0.424227,-0.112499,-0.103996},
    {0.398354,-0.437756,-0.238773,0.250562,-0.563943},
    {0.917668,-0.206733,0.285872,-0.663612,-1.192386},
    {-0.758330,1.376069,-0.165047,-1.402331,-0.031906},
    {-0.300420,0.452633,0.211518,0.417932,0.650356},
    {-0.071054,-0.719854,-0.308909,0.269422,-0.058884},
    {-0.174262,-0.214619,0.054257,0.139822,-0.287583},
    {1.344105,-1.029421,0.084045,0.432271,-1.173141},
    {1.339511,-0.806878,0.124321,-0.753164,-0.310375},
    {-0.832714,0.687317,-0.575513,0.277261,0.183503},
    {0.089745,-0.334108,-0.367033,-0.143072,-0.090116},
    {0.056230,-0.035933,-0.013776,0.168615,0.152755},
    {-0.343318,0.035364,-0.090695,-0.212136,-0.585461},
    {0.770140,-0.669831,-0.074712,0.087363,-0.086766},
    {1.162428,-0.020703,0.042052,0.136180,0.720488},
    {0.735728,-1.337048,0.019566,0.820192,0.101820},
    {0.433881,-0.008910,0.306519,-0.264649,-0.076299},
    {0.024541,0.001531,-0.022611,-0.270914,-0.132397},
    {0.198585,0.050799,-0.130526,0.091802,-0.027621},
    {0.405464,0.327499,-0.336595,0.311945,-0.427157},
    {0.749683,-0.015591,-0.453805,-0.096247,0.848945},
    {0.030674,-0.569710,-0.059784,0.803832,-0.502812},
    {0.116791,-0.186338,0.020030,0.057132,-0.483446},
    {0.079323,0.300370,0.148893,-0.388027,-0.034074},
    {0.304073,0.278782,-0.209154,0.199230,0.375546},
    {1.135339,0.577640,-0.746603,-0.196726,-0.068745},
    {1.496283,-0.358694,-0.558207,0.399696,0.655773},
    {1.511961,-0.786344,-0.442384,-0.493417,0.178220},
    {0.123064,-0.224398,0.306357,-0.215535,-0.520818},
    {-0.063785,0.404967,0.207329,-0.360217,-0.128151},
    {0.213144,0.088867,-0.300101,-0.057863,0.391882},
    {1.944306,-0.606215,0.328196,-0.149578,-0.255339},
    {-0.810744,0.003579,0.412761,0.149402,-0.926533},
    {-0.194060,0.796400,-0.161615,-0.278420,0.071311}};

float biases[n_commands] = {
    1.887595,-1.261117,-0.444253,-1.085810,-1.230227};


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
}

void loop(void)
{
    /* Get a new sensor event */
    sensors_event_t event;
    gyro.getEvent(&event);
    
    sensors_event_t aevent, mevent;
    accelmag.getEvent(&aevent, &mevent);

    input[0] = aevent.acceleration.x;
    input[1] = aevent.acceleration.y;
    input[2] = aevent.acceleration.z;
    input[3] = event.gyro.x;
    input[4] = event.gyro.y;
    input[5] = event.gyro.z;

    /* print the input to show that it is updating correctly
    for( i = 0 ; i < n_lookback ; i++) {
        for( j = 0 ; j < n_features ; j++) {
            Serial.print(input[i*n_features+j]);
            Serial.print(',');
        }
        Serial.println(';');
    }
    Serial.println(';');
    */
    /* Display the results
    Serial.print("AcX = "); Serial.print(input[0], 4); 
    Serial.print(" | ");
    Serial.print("AcY = "); Serial.print(input[1], 4); 
    Serial.print(" | ");
    Serial.print("AcZ = "); Serial.print(input[2], 4); 
    Serial.print(" | ");
    Serial.print("GyX = "); Serial.print(input[3]); 
    Serial.print(" | ");
    Serial.print("GyY = "); Serial.print(input[4]); 
    Serial.print(" | ");
    Serial.print("GyZ = "); Serial.println(input[5]); 
    */

    //accumulate the inputs * weights + biases 
    for( i = 0 ; i < n_commands ; i++) { 
        accum = biases[i];
        for (j = 0; j < (n_lookback * n_features); j++) {
            accum += input[j] * weights[j][i];
        }
        output[i] = exp(accum);
    }
    for( i = 0 ; i < 4 ; i++) {
        digitalWrite(pinArray[i], LOW);
    }
    if ( command == 1) {
        digitalWrite(pinArray[1], HIGH);
    }
    if ( command == 2) {
        digitalWrite(pinArray[3], HIGH);
    }
    if ( command == 3) {
        digitalWrite(pinArray[2], HIGH);
    }
    if ( command == 4) {
        digitalWrite(pinArray[0], HIGH);
    }

    /*display the outputs
    for( i = 0 ; i < n_commands ; i++) {
        Serial.print(output[i]);
        Serial.print(',');
    }
    Serial.println(';');    
    */
    if (hold_out) {
        counter += 1;
    }
    else {
        command = 0;
        for( i = 1 ; i < n_commands ; i++) {
            if (output[i]>(1.5*output[command])) {
                command = i;
                hold_out = 1;
            }
        }
    }
    if ( counter > 10 ) {
        hold_out = 0;
        counter = 0;
    }
    Serial.print("command = "); Serial.println(command);

    // Shift all of the inputs by n_features
    for( i = (n_lookback-1); i > 0; i--) {
        for( j = 0; j < n_features; j++) {
            input[i*n_features+j] = input[(i-1)*n_features+j];
        }
    }

    delay(100);
}
