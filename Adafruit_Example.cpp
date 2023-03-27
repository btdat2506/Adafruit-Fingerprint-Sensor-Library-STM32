#include "Adafruit_Example.h"

void emptyDatabase(Adafruit_Fingerprint &finger)
{
	finger.debug_println("\n\nDeleting all fingerprint templates!");
	finger.debug_println("Press 'Y' key to continue");
	uint8_t ch;
	while (1)
	{
		HAL_UART_Receive(&huart2, &ch, 1, 10);
		if (ch == 'Y')
			break;
	}

	if (finger.verifyPassword())
		finger.debug_println("Found fingerprint sensor!");
	else
	{
		finger.debug_println("Did not find fingerprint sensor :(");
		while (1)
			;
	}

	finger.emptyDatabase();
	finger.debug_println("Now database is empty :)");
}

void changePassword(Adafruit_Fingerprint &finger)
{
	finger.debug_println("Adafruit fingerprint sensor, change password example");

	if (finger.verifyPassword())
	{
		finger.debug_println("Found fingerprint sensor!");
	}
	else
	{
		finger.debug_println("Did not find fingerprint sensor :(");
		while (1)
			;
	}

	finger.debug_print("Set password... ");
	uint8_t p = finger.setPassword(1337);
	if (p == FINGERPRINT_OK)
	{
		finger.debug_println("OK"); // Password is set
	}
	else
	{
		finger.debug_println("ERROR"); // Failed to set password
	}
}

uint8_t deleteFingerprint(Adafruit_Fingerprint &finger)
{
	finger.debug_print("Please type in the ID # (from 1 to 127) you want to delete...");
	uint8_t id;
	while (1)
	{
		HAL_UART_Receive(&huart2, &id, 1, 10);
		if (1 <= id && id <= 127)
			break;
	}

	finger.debug_println("Deleting ID #%d", id);

	uint8_t p = -1;

	p = finger.deleteModel(id);

	if (p == FINGERPRINT_OK)
	{
		finger.debug_println("Deleted!");
	}
	else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		finger.debug_println("Communication error");
	}
	else if (p == FINGERPRINT_BADLOCATION)
	{
		finger.debug_println("Could not delete in that location");
	}
	else if (p == FINGERPRINT_FLASHERR)
	{
		finger.debug_println("Error writing to flash");
	}
	else
	{
		finger.debug_println("Unknown error: 0x%X", p);
	}

	return p;
}

void enroll(Adafruit_Fingerprint &finger)
{
	finger.debug_println("\n\nAdafruit Fingerprint sensor enrollment");

	if (finger.verifyPassword())
		finger.debug_println("Found fingerprint sensor!");
	else
	{
		finger.debug_println("Did not find fingerprint sensor :(");
		while (1)
			HAL_Delay(1);
	}

	readSensorParameters(finger);

	finger.debug_println("Ready to enroll a fingerprint!");
	finger.debug_println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
	uint8_t id;
	while (1)
	{
		HAL_UART_Receive(&huart2, &id, 3, 10); //Can't input number (Because it read as characters, so it needs to read in 3 bytes).
		if (1 <= id && id <= 127)
			break;
	}
	if (id == 0)
	{ // ID #0 not allowed, try again!
		return;
	}
	finger.debug_print("Enrolling ID #%d", id);

	while (!getFingerprintEnroll(finger, id));
}

uint8_t getFingerprintEnroll(Adafruit_Fingerprint &finger, uint8_t id)
{

	int p = -1;
	finger.debug_print("Waiting for valid finger to enroll as #%d", id);
	while (p != FINGERPRINT_OK)
	{
		p = finger.getImage();
		switch (p)
		{
		case FINGERPRINT_OK:
			finger.debug_println("Image taken");
			break;
		case FINGERPRINT_NOFINGER:
			finger.debug_println(".");
			break;
		case FINGERPRINT_PACKETRECIEVEERR:
			finger.debug_println("Communication error");
			break;
		case FINGERPRINT_IMAGEFAIL:
			finger.debug_println("Imaging error");
			break;
		default:
			finger.debug_println("Unknown error");
			break;
		}
	}

	// OK success!

	p = finger.image2Tz(1);
	switch (p)
	{
	case FINGERPRINT_OK:
		finger.debug_println("Image converted");
		break;
	case FINGERPRINT_IMAGEMESS:
		finger.debug_println("Image too messy");
		return p;
	case FINGERPRINT_PACKETRECIEVEERR:
		finger.debug_println("Communication error");
		return p;
	case FINGERPRINT_FEATUREFAIL:
		finger.debug_println("Could not find fingerprint features");
		return p;
	case FINGERPRINT_INVALIDIMAGE:
		finger.debug_println("Could not find fingerprint features");
		return p;
	default:
		finger.debug_println("Unknown error");
		return p;
	}

	finger.debug_println("Remove finger");
	HAL_Delay(2000);
	p = 0;
	while (p != FINGERPRINT_NOFINGER)
	{
		p = finger.getImage();
	}
	finger.debug_print("ID %d", id);
	p = -1;
	finger.debug_println("Place same finger again");
	while (p != FINGERPRINT_OK)
	{
		p = finger.getImage();
		switch (p)
		{
		case FINGERPRINT_OK:
			finger.debug_println("Image taken");
			break;
		case FINGERPRINT_NOFINGER:
			finger.debug_print(".");
			break;
		case FINGERPRINT_PACKETRECIEVEERR:
			finger.debug_println("Communication error");
			break;
		case FINGERPRINT_IMAGEFAIL:
			finger.debug_println("Imaging error");
			break;
		default:
			finger.debug_println("Unknown error");
			break;
		}
	}

	// OK success!

	p = finger.image2Tz(2);
	switch (p)
	{
	case FINGERPRINT_OK:
		finger.debug_println("Image converted");
		break;
	case FINGERPRINT_IMAGEMESS:
		finger.debug_println("Image too messy");
		return p;
	case FINGERPRINT_PACKETRECIEVEERR:
		finger.debug_println("Communication error");
		return p;
	case FINGERPRINT_FEATUREFAIL:
		finger.debug_println("Could not find fingerprint features");
		return p;
	case FINGERPRINT_INVALIDIMAGE:
		finger.debug_println("Could not find fingerprint features");
		return p;
	default:
		finger.debug_println("Unknown error");
		return p;
	}

	// OK converted!
	finger.debug_println("Creating model for #%d", id);

	p = finger.createModel();
	if (p == FINGERPRINT_OK)
	{
		finger.debug_println("Prints matched!");
	}
	else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		finger.debug_println("Communication error");
		return p;
	}
	else if (p == FINGERPRINT_ENROLLMISMATCH)
	{
		finger.debug_println("Fingerprints did not match");
		return p;
	}
	else
	{
		finger.debug_println("Unknown error");
		return p;
	}

	finger.debug_println("ID %d", id);
	p = finger.storeModel(id);
	if (p == FINGERPRINT_OK)
	{
		finger.debug_println("Stored!");
	}
	else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		finger.debug_println("Communication error");
		return p;
	}
	else if (p == FINGERPRINT_BADLOCATION)
	{
		finger.debug_println("Could not store in that location");
		return p;
	}
	else if (p == FINGERPRINT_FLASHERR)
	{
		finger.debug_println("Error writing to flash");
		return p;
	}
	else
	{
		finger.debug_println("Unknown error");
		return p;
	}

	return true;
}

uint8_t getFingerprintID(Adafruit_Fingerprint &finger)
{
	uint8_t p = finger.getImage();
	switch (p)
	{
	case FINGERPRINT_OK:
		finger.debug_println("Image taken");
		break;
	case FINGERPRINT_NOFINGER:
		finger.debug_println("No finger detected");
		return p;
	case FINGERPRINT_PACKETRECIEVEERR:
		finger.debug_println("Communication error");
		return p;
	case FINGERPRINT_IMAGEFAIL:
		finger.debug_println("Imaging error");
		return p;
	default:
		finger.debug_println("Unknown error");
		return p;
	}

	// OK success!

	p = finger.image2Tz();
	switch (p)
	{
	case FINGERPRINT_OK:
		finger.debug_println("Image converted");
		break;
	case FINGERPRINT_IMAGEMESS:
		finger.debug_println("Image too messy");
		return p;
	case FINGERPRINT_PACKETRECIEVEERR:
		finger.debug_println("Communication error");
		return p;
	case FINGERPRINT_FEATUREFAIL:
		finger.debug_println("Could not find fingerprint features");
		return p;
	case FINGERPRINT_INVALIDIMAGE:
		finger.debug_println("Could not find fingerprint features");
		return p;
	default:
		finger.debug_println("Unknown error");
		return p;
	}

	// OK converted!
	p = finger.fingerSearch();
	if (p == FINGERPRINT_OK)
	{
		finger.debug_println("Found a print match!");
	}
	else if (p == FINGERPRINT_PACKETRECIEVEERR)
	{
		finger.debug_println("Communication error");
		return p;
	}
	else if (p == FINGERPRINT_NOTFOUND)
	{
		finger.debug_println("Did not find a match");
		return p;
	}
	else
	{
		finger.debug_println("Unknown error");
		return p;
	}

	// found a match!
	finger.debug_println("Found ID #%d with confidence of %d", finger.fingerID, finger.confidence);

	return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez(Adafruit_Fingerprint &finger)
{
	uint8_t p = finger.getImage();
	if (p != FINGERPRINT_OK)
		return -1;

	p = finger.image2Tz();
	if (p != FINGERPRINT_OK)
		return -1;

	p = finger.fingerFastSearch();
	if (p != FINGERPRINT_OK)
		return -1;

	// found a match!
	finger.debug_print("Found ID #%d", finger.fingerID);
	finger.debug_println(" with confidence of %d", finger.confidence);
	return finger.fingerID;
}

void ScanFingerprint(Adafruit_Fingerprint &finger)
{
	finger.debug_println("\n\nAdafruit finger detect test");

	HAL_Delay(5);
	if (finger.verifyPassword())
	{
		finger.debug_println("Found fingerprint sensor!");
	}
	else
	{
		finger.debug_println("Did not find fingerprint sensor :(");
		while (1)
		{
			HAL_Delay(1);
		}
	}

	readSensorParameters(finger);

	finger.getTemplateCount();

	if (finger.templateCount == 0)
	{
		finger.debug_print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
	}
	else
	{
		finger.debug_println("Waiting for valid finger...");
		finger.debug_println("Sensor contains %d templates", finger.templateCount);
	}
	while (1)
	{
		getFingerprintID(finger);
		HAL_Delay(50); // don't ned to run this at full speed.
	}
}

void readSensorParameters(Adafruit_Fingerprint &finger)
{
	finger.debug_println("Reading sensor parameters");
	finger.getParameters();
	finger.debug_println("Status: 0x%X", finger.status_reg);
	finger.debug_println("Sys ID: 0x%X", finger.system_id);
	finger.debug_println("Capacity: %d", finger.capacity);
	finger.debug_println("Security level: %d", finger.security_level);
	finger.debug_println("Device address: 0x%X", finger.device_addr);
	finger.debug_println("Packet len: %d", finger.packet_len);
	finger.debug_println("Baud rate: %d", finger.baud_rate);
}

void LEDControl(Adafruit_Fingerprint &finger)
{
	// LED fully on
	finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_RED);
	HAL_Delay(250);
	finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_BLUE);
	HAL_Delay(250);
	finger.LEDcontrol(FINGERPRINT_LED_ON, 0, FINGERPRINT_LED_PURPLE);
	HAL_Delay(250);

	// flash red LED
	finger.LEDcontrol(FINGERPRINT_LED_FLASHING, 25, FINGERPRINT_LED_RED, 10);
	HAL_Delay(2000);
	// Breathe blue LED till we say to stop
	finger.LEDcontrol(FINGERPRINT_LED_BREATHING, 100, FINGERPRINT_LED_BLUE);
	HAL_Delay(3000);
	finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_ON, 200, FINGERPRINT_LED_PURPLE);
	HAL_Delay(2000);
	finger.LEDcontrol(FINGERPRINT_LED_GRADUAL_OFF, 200, FINGERPRINT_LED_PURPLE);
	HAL_Delay(2000);
}
