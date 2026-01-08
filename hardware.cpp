/*
#include <Arduino.h>
#define LED_PIN 12 // LED to be controlled
String command = "";
void setup()
{
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println("ESP32 ready");
} // end setup()
void loop()
{
    while (Serial.available())
    {
        char c = Serial.read();

        if (c == '\n')
        {
            command.trim();

            if (command == "OPEN")
            {
                digitalWrite(LED_PIN, HIGH);
                Serial.println("LED ON");
            }
            else if (command == "FIST")
            {
                digitalWrite(LED_PIN, LOW);
                Serial.println("LED OFF");
            }

            command = "";
        }
        else
        {
            command += c;
        }
    }
} // end of main loop
*/

/*
 * Author: Dean Etim (Radiant Tech Nig. Ltd.)
 * Last Updated: 06/01/2026
 * Version: Gesture Controlled Smart Bin
 */

#include <ESP32Servo.h>

Servo lidServo;

// Pin definitions
const int SERVO_PIN = 15;
const int TRIG_PIN = 14;
const int ECHO_PIN = 12;

// Distance measurement (not in use for now)
const float conversionFactor = 0.0171;

bool lidOpen = false;
String lastCommand = "";

// Function Prototypes
void openLid();
void closeLid();

void setup()
{
    Serial.begin(115200);

    lidServo.attach(SERVO_PIN);
    lidServo.write(180); // Closed

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
} // end setup()

void loop()
{

    if (Serial.available())
    {
        String command = Serial.readStringUntil('\n');
        command.trim();
        command.toLowerCase();

        // Avoid repeating same command
        if (command == lastCommand)
            return;

        if (command == "open" && !lidOpen)
        {
            openLid();
            lidOpen = true;
            // SerialBT.println("Lid opened");
        }

        else if (command == "close" && lidOpen)
        {
            closeLid();
            lidOpen = false;
            // SerialBT.println("Lid closed");
        }

        lastCommand = command;
    }
} // end main loop

void openLid()
{
    for (int angle = 180; angle >= 95; angle--)
    {
        lidServo.write(angle);
        delay(15);
    }
} // end openLid()

void closeLid()
{
    for (int angle = 95; angle <= 180; angle++)
    {
        lidServo.write(angle);
        delay(15);
    }
} // end closeLid()
