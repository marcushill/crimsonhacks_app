
#include <CurieBLE.h>

#define BUTTON_PIN 2
#define SWITCH_PIN 4
#define TIMER_LENGTH 4000

BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService helpService("8ce255c0-200a-11e0-ac64-0800200c9a66");

//BLE LED Switch Characteristic - custom 128-bit UUID, read and writable by central
BLEUnsignedCharCharacteristic switchCharacteristic("8ce255c0-200a-11e0-ac64-0800200c9a66", BLERead | BLEWrite | BLENotify);

unsigned long timer = 0;

void setup() {
    Serial.begin(9600);                         // begin Serial session
    while (!Serial);                                         // wait for serial port to connect.
    pinMode(BUTTON_PIN, INPUT);
    pinMode(SWITCH_PIN, INPUT);

    blePeripheral.setLocalName("Help me I need to leave SOS SOS SOS");
    blePeripheral.setAdvertisedServiceUuid(helpService.uuid());

    // add service and characteristic:
    blePeripheral.addAttribute(helpService);
    blePeripheral.addAttribute(switchCharacteristic);

    // set the initial value for the characeristic:
    switchCharacteristic.setValue(0);

    // begin advertising BLE service:
    blePeripheral.begin();

    Serial.println("BLE Help Peripheral");
}

void loop() {
    
    BLECentral central = blePeripheral.central();

    // if a central is connected to peripheral:
     if (central) {
        Serial.print("Connected to central: ");
    // print the central's MAC address:
        Serial.println(central.address());
        int mode = digitalRead(SWITCH_PIN);
        if (!mode)
            switchCharacteristic.setValue(0xA0);
        else
            switchCharacteristic.setValue(0xB0);    

        int numPushes = 0;
        
        while(central.connected()){  
            int push_state = digitalRead(BUTTON_PIN);
            int mode = digitalRead(SWITCH_PIN);
            
            if (push_state == 1){
                numPushes++;
                while(push_state == 1)
                    push_state = digitalRead(BUTTON_PIN);
            }
            
            if ((numPushes == 1) && !timer){
                Serial.println("Timer started");
                timer = millis() + TIMER_LENGTH;
                if (!mode)
                    switchCharacteristic.setValue(0xA1);
                else
                    switchCharacteristic.setValue(0xB1); 
            }  
            else if ((millis() <= timer) && (numPushes == 2)){
                Serial.println("Timer reset to zero");
                timer = 0;
                numPushes = 0;
                if (!mode)
                    switchCharacteristic.setValue(0xA0);
                else
                    switchCharacteristic.setValue(0xB0);
            }
            else if ((millis() > timer) && (numPushes == 1) && timer){
                Serial.println("Timer expired");
                timer = 0;
                numPushes = 0;
                if (!mode)
                    switchCharacteristic.setValue(0xA2);
                else
                    switchCharacteristic.setValue(0xB2); 
            }
        }
        Serial.println("Disconnected");  
    }
}
