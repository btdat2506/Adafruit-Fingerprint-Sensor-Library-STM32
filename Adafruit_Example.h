#ifndef FINGERPRINT_FUNCTIONS_H
#define FINGERPRINT_FUNCTIONS_H

#include "main.h"
#include "Adafruit_Fingerprint.h"
#include "UART_Interface.h"

void emptyDatabase(Adafruit_Fingerprint &finger);
void changePassword(Adafruit_Fingerprint &finger);
uint8_t deleteFingerprint (Adafruit_Fingerprint &finger);
void enroll(Adafruit_Fingerprint &finger);
uint8_t getFingerprintID(Adafruit_Fingerprint &finger);
int getFingerprintIDez(Adafruit_Fingerprint &finger);
void ScanFingerprint(Adafruit_Fingerprint &finger);
void readSensorParameters(Adafruit_Fingerprint &finger);
void LEDControl(Adafruit_Fingerprint &finger);
uint8_t getFingerprintEnroll(Adafruit_Fingerprint &finger, uint8_t id);

#endif // FINGERPRINT_FUNCTIONS_H
