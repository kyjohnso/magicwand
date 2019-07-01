#include <IRLib.h>
#include <Wire.h>

const int MPU=0x68;  // I2C address of the MPU-6050
const int buttonPin = 4;
int16_t AcX, AcY, AcZ, GyX, GyY, GyZ, Tmp;

float accum;
int i, j, p, q, r, command;

/******************************************************************************
 * Neural Network 
 *****************************************************************************/

const int n_lookback=4;
const int n_features=6;
const int n_commands=7;

float input[n_lookback*n_features] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

float output[n_commands] = {
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

const float weights[n_lookback*n_features][n_commands] = {
    {-0.564432,-1.083129,-2.473132,0.144819,-1.345450,1.963713,-2.575714},
    {-7.146026,-3.725602,4.832800,-2.092429,-0.584238,1.452593,6.953203},
    {-2.428850,1.473558,3.972067,0.048832,4.908653,4.840707,1.037443},
    {1.758150,1.943153,2.411292,2.400856,1.416014,-4.728519,-2.907049},
    {-4.298436,-1.689865,-1.928795,-4.122108,8.220893,-4.604435,-0.602354},
    {1.506839,4.425248,3.636387,-1.316531,7.131639,-5.672682,1.187278},
    {-1.830604,-0.299281,-1.735074,-1.062762,-1.480794,1.986322,-0.430398},
    {-3.380674,0.281217,3.377275,1.573442,2.758604,-1.614207,0.688871},
    {2.294996,0.450414,0.919197,0.527096,-0.309666,3.611857,3.798430},
    {6.504260,2.115866,-4.676786,3.429514,-6.253214,-7.811475,1.890395},
    {-1.763634,0.043138,-2.868149,-0.353915,-0.320147,5.057618,-0.224008},
    {2.293773,4.318697,-0.941052,-5.796593,-0.113480,-0.003744,-1.482390},
    {-1.016426,-1.180648,-1.468248,-0.031351,0.392228,-2.170453,-1.184169},
    {-2.075575,-0.203173,0.914842,1.598890,-1.694891,2.655497,-1.957845},
    {1.990921,1.935485,0.889917,2.421314,0.627956,-0.013213,2.265650},
    {2.801530,0.055526,-3.893097,2.874426,-1.717211,-2.040635,0.535892},
    {4.001897,6.074271,-1.940358,-4.062213,-3.615236,1.576797,-0.206670},
    {4.750781,6.783901,0.706028,-4.742378,-2.557796,3.801516,1.807481},
    {-1.433919,-1.614468,-3.467160,-2.535298,-0.569133,-2.935711,-1.254306},
    {5.903408,0.658922,-6.488503,-2.825819,-1.799659,-0.211524,-6.058407},
    {-0.760642,0.736866,3.163045,2.527609,-0.921472,-0.934029,2.315453},
    {0.831799,-0.709512,-4.517357,-1.000960,2.252186,-0.740086,0.531570},
    {3.741864,5.053699,1.255466,-4.842499,5.459579,-1.601173,4.021405},
    {0.835115,2.693720,-5.309175,-3.674399,-1.836785,0.831916,-0.460310}};

const float biases[n_commands] = {
    -0.114612,0.604834,-3.580164,0.095987,-3.393858,-2.284501,0.597269};

void setup() {
    // put your setup code here, to run once:
    pinMode(buttonPin, INPUT);
    Wire.begin();
    Wire.beginTransmission(MPU);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    Serial.begin(9600);
}

void loop() {
    //read the accelerometer readings from the registers
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,14,true);  // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 
    	     // 0x3C (ACCEL_XOUT_L)     
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) &
    	     // 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 
    			     // 0x40 (ACCEL_ZOUT_L)
    Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 
                                     // 0x42 (TEMP_OUT_L)
    GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 
                             // 0x44 (GYRO_XOUT_L)
    GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 
                             // 0x46 (GYRO_YOUT_L)
    GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 
                                     // 0x48 (GYRO_ZOUT_L)

    Serial.print("AcX = "); Serial.print(AcX);
    Serial.print(" | AcY = "); Serial.print(AcY);
    Serial.print(" | AcZ = "); Serial.print(AcZ);
    Serial.print(" | Tmp = "); Serial.print(Tmp/340.00+36.53);  //equation for temperature in degrees C from datasheet
    Serial.print(" | GyX = "); Serial.print(GyX);
    Serial.print(" | GyY = "); Serial.print(GyY);
    Serial.print(" | GyZ = "); Serial.println(GyZ);

    input[0] = AcX/32768.;
    input[1] = AcY/32768.;
    input[2] = AcZ/32768.;
    input[2] = GyX/32768.;
    input[4] = GyY/32768.;
    input[5] = GyZ/32768.;

    for( i = 0 ; i < n_features ; i++) {
        Serial.print(input[i]); Serial.print(',');
    }
    Serial.println(']');

    //accumulate the inputs * weights + biases 
    for( i = 0 ; i < n_commands ; i++) { 
        accum = biases[i];
        for (j = 0; j < (n_lookback * n_features); j++) {
            accum += input[j] * weights[j][i];
        }
        output[i] = accum;
    }
    /*
    command = 0;
    for( i = 1 ; i < n_commands ; i++) {
        if (output[i]>output[command]) {
            command = i;
        } 
    Serial.print("command = "); Serial.println(command);
    }
    */

    for( i = n_lookback-1; i > 0; i--) {
        for( j = 0; i < n_features; j++) {
            input[i*n_features+j] = input[(i-1)*n_lookback+j];
        }
    }
    delay(100);
}
