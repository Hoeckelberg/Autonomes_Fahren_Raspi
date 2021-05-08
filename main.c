#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define STEP_SIZE (0.087890625)
#define MIN_DELAY 2

const int MOTOR_A_PINS[] = {0, 1, 2, 3};
const int MOTOR_B_PINS[] = {11, 13, 15, 19};
const int ULTRASCHALL_TRG = 25;
const int ULTRASCHALL_ECHO = 24;
float delayValue = MIN_DELAY;

const int steps[][4] = {
    {1, 0, 0, 0},
    {1, 1, 0, 0},
    {0, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1}
};

void rotate(int*, float, int);
float ultraschallGetDistanceCM();

void rotate(int *pins, float deg, int len){
    int stepCount = deg / STEP_SIZE;

    for(int i = 0; i <= stepCount; i++){
        for(int j = 0; j < len; j++){
            digitalWrite(pins[j], steps[i%8][j]);
        }
        delay(delayValue);
    }
}

void driveToWall(int* pins, int len, float minDistance, float slowDownDistance){
    float distance;
    while((distance = ultraschallGetDistanceCM()) > minDistance){
        printf("%f - %f\n", distance, delayValue);
        rotate(pins, 1, len);
        if(distance > slowDownDistance)
            delayValue = MIN_DELAY;
        else
            delayValue = (1/distance*100)+MIN_DELAY;
    }
}


int main(int argc, char *argv[]){
    wiringPiSetup();

    initPins();
    resetPins();

    //rotate(MOTOR_A_PINS, 1*360, 4);
    //rotate(MOTOR_A_PINS, 360, 4);
    driveToWall(MOTOR_A_PINS, 4, 10.0, 20.0);
    resetPins();
}

void initPins(){
    for(int i=0; i< 4; i++){
        pinMode(MOTOR_A_PINS[i], OUTPUT);
        pinMode(MOTOR_B_PINS[i], OUTPUT);
    }
    pinMode(ULTRASCHALL_ECHO, INPUT);
    pinMode(ULTRASCHALL_TRG, OUTPUT);
}

void resetPins(){
    for(int i = 0; i < 4; i++){
        digitalWrite(MOTOR_A_PINS[i], 0);
    }
}

long ultraschallGetDistanceTime(){
    // Send Trigger
    digitalWrite(ULTRASCHALL_TRG, HIGH);
    delayMicroseconds(20);
    digitalWrite(ULTRASCHALL_TRG, LOW);

    // Wait for start signal
    while(!digitalRead(ULTRASCHALL_ECHO));

    // Stop time to response
    long start = micros();
    while(digitalRead(ULTRASCHALL_ECHO));

    // Return difference
    return micros() - start;
}

float ultraschallGetDistanceCM(){
    float distance = ultraschallGetDistanceTime() / 58;
    return distance;
}
