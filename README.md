Chonjo Multifactor Authentication System
Overview

The Chonjo Multifactor Authentication System is a robust security system designed to implement multi-layered authentication using a combination of RFID, password entry, and fingerprint verification. This system ensures that only authorized personnel can gain access, making it ideal for secure environments where multi-factor authentication (MFA) is required.
Components Used

    Microcontroller: Arduino Uno
    RFID Module: MFRC522
    Fingerprint Sensor: Adafruit Fingerprint Sensor
    Keypad: 4x4 Matrix Keypad
    LCD Display: 16x2 LiquidCrystal_I2C (I2C interface)
    Relay Module: Used for controlling external devices like door locks
    Buzzer: For auditory feedback
    Libraries:
        SPI.h - For SPI communication with the RFID module
        MFRC522.h - RFID module library
        Wire.h - I2C communication for LCD
        LiquidCrystal_I2C.h - LCD library for I2C interface
        Keypad.h - Library to manage the 4x4 keypad
        SoftwareSerial.h - To facilitate serial communication with the fingerprint sensor
        Adafruit_Fingerprint.h - Adafruit's library for fingerprint management

Methodology

    Initialization:
        The system initializes all the peripherals, including the fingerprint sensor, RFID reader, and LCD. The microcontroller also sets up serial communication for debugging and interfacing with the fingerprint sensor.
        The LCD displays a welcome message and prompts the user to scan their RFID tag.

    RFID Authentication:
        The system waits for a new RFID card to be presented.
        Once a card is detected, it reads the RFID tag and compares it with predefined authorized tags stored in the system.
        If the tag matches, the system proceeds to the next authentication step (password entry).
        If the tag does not match, a warning message is displayed, and access is denied.

    Password Authentication:
        Upon successful RFID authentication, the system prompts the user to enter a password using the 4x4 keypad.
        The entered password is masked on the LCD screen to maintain confidentiality.
        If the correct password is entered, the system advances to fingerprint authentication.
        If the password is incorrect, access is denied, and the system returns to the initial state.

    Fingerprint Authentication:
        The system prompts the user to place their finger on the fingerprint sensor.
        The fingerprint is scanned and compared against the enrolled fingerprints stored in the sensor.
        If a matching fingerprint is found, access is granted, and the relay is activated to unlock the door.
        If no match is found after multiple attempts, access is denied, and the system resets.

    Access Control:
        Once all three factors (RFID, password, fingerprint) are authenticated, the system triggers the relay, unlocking the door or performing another specified action.
        The relay remains active for a predefined duration (e.g., 5 seconds) before locking the door again.
        The system then resets, ready for the next authentication sequence.

Key Functions

    rfidAuthentication(): Manages the detection and validation of the RFID tag.
    getFingerprintID() and getFingerprintIDez(): Handle fingerprint scanning and matching.
    scrollmessage(String title, String message): Displays scrolling messages on the LCD, enhancing user interaction and providing clear feedback.

Advantages

    High Security: By combining three forms of authentication, the system significantly reduces the likelihood of unauthorized access.
    User Feedback: The LCD and buzzer provide real-time feedback to the user, making the system easy to use and understand.
    Scalability: Additional features, such as logging attempts or integrating with a network, can be added to further enhance security and monitoring.

Conclusion

The Chonjo Multifactor Authentication System is a comprehensive solution for environments that require stringent access controls. With its combination of RFID, keypad, and fingerprint authentication, this system ensures that only individuals with proper credentials can gain access. This project serves as an excellent foundation for further enhancements in secure access systems.