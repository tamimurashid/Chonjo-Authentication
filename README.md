Chonjo Multifactor Authentication System
Overview



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