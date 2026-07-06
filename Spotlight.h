#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H

#include <Arduino.h>
#include <Servo.h>

/*
 * SPOTLIGHT LIBRARY
 * This library handles the complex timing and hardware control for a Pan/Tilt 
 * Spotlight system. It runs background tasks like flashing LEDs and moving servos 
 * without using delay(), allowing the main loop to remain responsive.
 * 
 * Auto-Sleep Feature: Servos automatically detach after 500ms of inactivity 
 * to prevent constant motor buzzing.
 */

class Spotlight {
  private:
    Servo panServo;
    Servo tiltServo;

    // --- Hardcoded Pin Definitions ---
    // Servos
    const uint8_t TILT_PIN = 9;
    const uint8_t PAN_PIN = 10;
    
    // User-controlled LEDs
    const uint8_t RED_PIN = 4;
    const uint8_t GREEN_PIN = 6;
    const uint8_t BLUE_PIN = 7;
    const uint8_t MAIN_LED_PIN = 8; 

    // Background engine LEDs
    const uint8_t SECOND_FLASH = 2;
    const uint8_t TENTH_FLASH = 3;
    const uint8_t FADE_LED = 5;
    const uint8_t CONSTANT_LED = 13;

    // --- Timers for Background Tasks ---
    uint32_t prevTimeSecond = 0;
    uint32_t prevTimeTenth = 0;
    uint32_t prevTimeFade = 0;
    uint32_t prevTimeServo = 0;
    
    // Auto-sleep timers for the servos
    uint32_t lastPanMoveTime = 0;
    uint32_t lastTiltMoveTime = 0;

    // --- State Variables ---
    // LED States
    uint8_t fadeValue = 0;
    bool ledStateSecond = false;
    bool ledStateTenth = false;
    bool fadeState = false;

    // Servo Movement States
    int panAngle = 90;
    int tiltAngle = 90;
    
    bool isPanBouncing = false;
    int panMin = 0;
    int panMax = 180;
    int panDir = 1; // 1 for moving up, -1 for moving down

    bool isTiltBouncing = false;
    int tiltMin = 0;
    int tiltMax = 180;
    int tiltDir = 1;

  public:
    /*
     * INITIALIZATION
     */
    void begin() {
      pinMode(RED_PIN, OUTPUT);
      pinMode(GREEN_PIN, OUTPUT);
      pinMode(BLUE_PIN, OUTPUT);
      pinMode(MAIN_LED_PIN, OUTPUT);
      
      pinMode(SECOND_FLASH, OUTPUT);
      pinMode(TENTH_FLASH, OUTPUT);
      pinMode(FADE_LED, OUTPUT);
      pinMode(CONSTANT_LED, OUTPUT);

      digitalWrite(CONSTANT_LED, HIGH);

      // Attach servos, write starting positions, and log the time
      panServo.attach(PAN_PIN);
      tiltServo.attach(TILT_PIN);

      panServo.write(panAngle);
      tiltServo.write(tiltAngle);
      
      lastPanMoveTime = millis();
      lastTiltMoveTime = millis();
    }


    /* =========================================
     *           LED CONTROL FUNCTIONS
     * ========================================= */

    void turnOnLED() { 
      digitalWrite(MAIN_LED_PIN, HIGH); 
    }

    void turnOffLED() { 
      digitalWrite(MAIN_LED_PIN, LOW); 
    }

    void setRGB(bool red, bool green, bool blue) {
      digitalWrite(RED_PIN, red ? HIGH : LOW);
      digitalWrite(GREEN_PIN, green ? HIGH : LOW);
      digitalWrite(BLUE_PIN, blue ? HIGH : LOW);
    }


    /* =========================================
     *         MOVEMENT LOGIC FUNCTIONS
     * ========================================= */

    // Instantly moves the pan servo to a specific angle
    void set_pan(int angle) {
      isPanBouncing = false;
      if (!panServo.attached()) panServo.attach(PAN_PIN); // Wake up!
      
      panAngle = constrain(angle, 0, 180);
      panServo.write(panAngle);
      lastPanMoveTime = millis(); // Reset the sleep timer
    }

    // Instantly moves the tilt servo to a specific angle
    void set_tilt(int angle) {
      isTiltBouncing = false;
      if (!tiltServo.attached()) tiltServo.attach(TILT_PIN); // Wake up!
      
      tiltAngle = constrain(angle, 0, 180);
      tiltServo.write(tiltAngle);
      lastTiltMoveTime = millis(); // Reset the sleep timer
    }

    void pan_bounce(int minAngle, int maxAngle) {
      panMin = constrain(minAngle, 0, 180);
      panMax = constrain(maxAngle, 0, 180);
      isPanBouncing = true;
    }

    void tilt_bounce(int minAngle, int maxAngle) {
      tiltMin = constrain(minAngle, 0, 180);
      tiltMax = constrain(maxAngle, 0, 180);
      isTiltBouncing = true;
    }

    void stop_bounce() {
      isPanBouncing = false;
      isTiltBouncing = false;
    }


    /* =========================================
     *             STATE FUNCTIONS
     * ========================================= */

    int get_pan() {
      return panAngle;
    }

    int get_tilt() {
      return tiltAngle;
    }


    /* =========================================
     *            BACKGROUND ENGINE
     * ========================================= */
     
    void update() {
      uint32_t currentTime = millis();

      // 1. One Second Flash Background LED
      if (currentTime - prevTimeSecond >= 1000) {
        prevTimeSecond = currentTime;
        ledStateSecond = !ledStateSecond;
        digitalWrite(SECOND_FLASH, ledStateSecond ? HIGH : LOW);
      }

      // 2. Tenth of a Second Flash Background LED
      if (currentTime - prevTimeTenth >= 100) {
        prevTimeTenth = currentTime;
        ledStateTenth = !ledStateTenth;
        digitalWrite(TENTH_FLASH, ledStateTenth ? HIGH : LOW);
      }

      // 3. Smooth Fade Background LED
      if (currentTime - prevTimeFade >= 5) {
        prevTimeFade = currentTime;
        if (fadeValue >= 255) fadeState = false;
        else if (fadeValue <= 0) fadeState = true;
        
        fadeValue = fadeState ? fadeValue + 1 : fadeValue - 1;
        analogWrite(FADE_LED, fadeValue);
      }

      // 4. Servo Logic (Updates every 25 milliseconds)
      if (currentTime - prevTimeServo >= 25) { 
        prevTimeServo = currentTime;

        // --- Handle Pan ---
        if (isPanBouncing) {
          if (!panServo.attached()) panServo.attach(PAN_PIN); // Wake up if sleeping
          
          panAngle += (panDir * 2); 
          if (panAngle >= panMax) {
            panAngle = panMax;
            panDir = -1; 
          } else if (panAngle <= panMin) {
            panAngle = panMin;
            panDir = 1; 
          }
          panServo.write(panAngle);
          lastPanMoveTime = currentTime; // Reset sleep timer because it's moving
          
        } else {
          // If not bouncing, check if it's been sitting still for > 500ms
          if (panServo.attached() && (currentTime - lastPanMoveTime > 500)) {
            panServo.detach(); // Go to sleep!
          }
        }

        // --- Handle Tilt ---
        if (isTiltBouncing) {
          if (!tiltServo.attached()) tiltServo.attach(TILT_PIN); // Wake up if sleeping
          
          tiltAngle += (tiltDir * 2); 
          if (tiltAngle >= tiltMax) {
            tiltAngle = tiltMax;
            tiltDir = -1; 
          } else if (tiltAngle <= tiltMin) {
            tiltAngle = tiltMin;
            tiltDir = 1; 
          }
          tiltServo.write(tiltAngle);
          lastTiltMoveTime = currentTime; // Reset sleep timer because it's moving
          
        } else {
          // If not bouncing, check if it's been sitting still for > 500ms
          if (tiltServo.attached() && (currentTime - lastTiltMoveTime > 500)) {
            tiltServo.detach(); // Go to sleep!
          }
        }
      }
    }
};

#endif
