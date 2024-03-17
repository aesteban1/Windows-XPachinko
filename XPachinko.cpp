#include "mbed.h"
#include "Servo.h"

int score = 0;
int moves = 5;

SPI fourDigit(p5, NC, p7);
DigitalOut cs(p8);

//               a    b    c    d    e    f    g
BusOut oneDigit(p11, p12, p29, p28, p30, p9, p10);

InterruptIn ir1(p14);
InterruptIn ir2(p15);
InterruptIn ir3(p16);
InterruptIn ir4(p17);
InterruptIn ir5(p18);

PwmOut buzzer(p26);

Servo servo1(p25);
Servo servo2(p24);
Servo servo3(p23);
Servo servo4(p22);
Servo servo5(p21);

void display1digit(int num) {
    switch (num) {
    case 0:
        oneDigit = 0b0111111;
        break;
    case 1:
        oneDigit = 0b0000110;
        break;
    case 2:
        oneDigit = 0b1011011;
        break;
    case 3:
        oneDigit = 0b1001111;
        break;
    case 4:
        oneDigit = 0b1100110;
        break;
    case 5:
        oneDigit = 0b1101101;
        break;
    default:
        oneDigit = 0b0000000;
        break;
    }
}

void display4digits(int num) {
    // fourDigit.write(0x76);
    cs = 0;

    int first = num / 1000;
    fourDigit.write(first);
    num -= (first * 1000);

    int second = num / 100;
    fourDigit.write(second);
    num -= (second * 100);

    int third = num / 10;
    fourDigit.write(third);
    num -= (third * 10);

    int fourth = num;
    fourDigit.write(fourth);

    cs = 1;
}

void stepServo(Servo *s) {
    s->write(0.2);
    
    while(true) {
        ThisThread::sleep_for(500ms);

        auto x = s->read();
        x += 0.2;
        if(x > 1.0) {
            x = 0.2;
        }
        s->write(x);
    }
}

void playSong(int notes[], int beats[], int length) {
    // A4 = 0
    // notes are represented as half steps
    // up or down from A4

    // beats = number of 16th notes

    double f0 = 440.0;
    double a = pow(2.0, (1.0 / 12.0));
    double fn;

    for(int i = 0; i < length; i++) {
        fn = f0 * pow(a, notes[i]);
        buzzer.period(1.0/fn);
        buzzer = 0.5;

        wait_us(beats[i] * 125000);
    }
    buzzer = 0.0;
    buzzer.period(0.02);
}

void score1() {
    // Windows XP
    int notes[] = {17, 12, 10, 17, 12};
    int beats[] = {3, 3, 4, 2, 4};
    int length = 5;
    playSong(notes, beats, length);

    score += 250;
    moves -= 1;
}

void score2() {
    // 46 and 2
    int notes[] {-5, 7, -5, 5, 7, -5, 8, 7, -5, 5, 7, 3, 5};
    int beats[] {2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int length = 13;
    playSong(notes, beats, length);

    score += 500;
    moves -= 1;
}

void score3() {
    // Freddy
    int notes[] {7, 9, 7, 4, 4, 4, 2, 4, 5, 4};
    int beats[] {3, 2, 1, 3, 3, 2, 1, 2, 1, 6};
    int length = 10;
    playSong(notes, beats, length);

    score += 1000;
    moves -= 1;
}

void score4() {
    // random
    int notes[] {12, 10, 11, 9, 10, 8, 9, 7, 8, 6, 7, 5};
    int beats[] {1,  1,  1,  1, 1,  1, 1, 1, 1, 1, 1, 1};
    int length = 12;
    playSong(notes, beats, length);

    score += 25;
    moves -= 1;
}

void score5() {
    int notes[] {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int beats[] {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int length = 12;
    playSong(notes, beats, length);

    score += 400;
    moves -= 1;
}

int main() {

    Thread t1;
    t1.start(callback(stepServo, &servo1));
    wait_us(200000);

    Thread t2;
    t2.start(callback(stepServo, &servo2));
    wait_us(200000);

    Thread t3;
    t3.start(callback(stepServo, &servo3));
    wait_us(200000);

    Thread t4;
    t4.start(callback(stepServo, &servo4));
    wait_us(200000);

    Thread t5;
    t5.start(callback(stepServo, &servo5));
    wait_us(200000);


    ir1.fall(&score1);
    ir2.fall(&score2);
    ir3.fall(&score3);
    ir4.fall(&score4);
    ir5.fall(&score5);

    cs = 1;
    fourDigit.format(8, 1);
    fourDigit.frequency(250000);

    cs = 0;
    fourDigit.write(0x76);
    cs = 1;

    display4digits(score);
    // display4digits(1234);
    
    while(true) {
        wait_us(1000000);
        display4digits(score);
        display1digit(moves);

        if(moves == 0) {
            break;
        }
    }
}