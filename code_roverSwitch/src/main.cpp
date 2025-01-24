#include <Arduino.h>
#include <SPI.h>

#define LED5 5
#define LED6 4

#define POE_1 6
#define POE_2 7
#define POE_3 8
#define POE_4 9
#define POE_5 10

#define CAN_TX 13
#define CAN_RX 14

#define TX_1 33
#define RX_1 34

#define SPI_MISO 35
#define SPI_MOSI 36
#define SPI_CS 37
#define SPI_CLK 38

SPISettings ks8567SPISettings(1000000, MSBFIRST, SPI_MODE0); // 1 MHz, MSB first, Mode 0

void KSZ8567_WriteRegister(uint16_t reg, uint8_t data);
uint8_t KSZ8567_ReadRegister(uint16_t reg);
void blinkLed(void *parameter);

void setup() 
{
  Serial.begin(115200);
  Serial.println("Code Start Here");
  pinMode(LED5,OUTPUT);
  pinMode(LED6,OUTPUT);
  digitalWrite(LED5,LOW);
  digitalWrite(LED6,LOW);
  
  pinMode(POE_1,OUTPUT);
  pinMode(POE_2,OUTPUT);
  pinMode(POE_3,OUTPUT);
  pinMode(POE_4,OUTPUT);
  pinMode(POE_5,OUTPUT);
  digitalWrite(POE_1,LOW);
  digitalWrite(POE_2,LOW);
  digitalWrite(POE_3,LOW);
  digitalWrite(POE_4,LOW);
  digitalWrite(POE_5,LOW);

  // create task to blink led
  xTaskCreate(
    blinkLed,          // Function that should be called
    "Blink LED",       // Name of the task (for debugging)
    1000,              // Stack size (bytes)
    NULL,              // Parameter to pass
    1,                 // Task priority
    NULL               // Task handle
  );

  // SPI.begin(SPI_CLK, SPI_MISO, SPI_MOSI, SPI_CS);
  SPI.begin(SPI_CLK,SPI_MISO,SPI_MOSI);
  SPI.beginTransaction(ks8567SPISettings);
  pinMode(SPI_CS, OUTPUT);
  digitalWrite(SPI_CS, HIGH);

  delay(2000);
  Serial.println("Read KSZ ID");
  uint8_t chipID = KSZ8567_ReadRegister(0x0002);
  Serial.print("Chip ID: 0x");
  Serial.println(chipID, HEX);
}


void loop() 
{

}


void blinkLed(void *parameter)
{
  while(1)
  {
    digitalWrite(LED6,LOW);
    digitalWrite(LED5,HIGH);
    delay(250);
    digitalWrite(LED5,LOW);
    digitalWrite(LED6,HIGH);
    delay(250);
  }
}

void KSZ8567_WriteRegister(uint16_t reg, uint8_t data) {
    uint32_t command = 0x60000000 | (reg << 5); // Construct command: 010 + reg + data
    uint8_t spi_tx[4]

    spi_tx[0] = (command >> 24) & 0xFF; // Command MSB
    spi_tx[1] = (command >> 16) & 0xFF; // High Address
    spi_tx[2] = (command >> 8) & 0xFF;  // Low Address
    spi_tx[3] = command & 0xFF;         // Data
    spi_tx[4] = data;

    // Begin SPI transaction
    digitalWrite(SPI_CS, LOW);  // Pull CS low to select the KSZ8567
    // SPI.beginTransaction(ks8567SPISettings);

    // Send the 4 bytes
    for (int i = 0; i < 5; i++) {
        SPI.transfer(spi_tx[i]);
    }

    // SPI.endTransaction();
    digitalWrite(SPI_CS, HIGH); // Pull CS high to deselect the KSZ8567
}

// Read from KSZ8567 register
uint8_t KSZ8567_ReadRegister(uint16_t reg) {
    uint32_t command = 0x60000000 | (reg << 5); // Construct command: 011 + reg
    uint8_t spi_tx[5];
    uint8_t spi_rx[5];

    // Serial.print("command");
    // Serial.print(command,BIN);
    // Serial.print(" ");
    // Serial.println(command,HEX);

    spi_tx[0] = (command >> 24) & 0xFF; // Command MSB
    spi_tx[1] = (command >> 16) & 0xFF; // High Address
    spi_tx[2] = (command >> 8) & 0xFF;  // Low Address
    spi_tx[3] = (command) & 0xFF;                   // Dummy byte for turnaround and read
    spi_tx[4] = 0x00;                   // Dummy byte for turnaround and read
    
    // Begin SPI transaction
    digitalWrite(SPI_CS, LOW);  // Pull CS low to select the KSZ8567

    // Send the 4 bytes and receive the response
    for (int i = 0; i < 5; i++) {
      // Serial.println(spi_tx[i],BIN);
        spi_rx[i] = SPI.transfer(spi_tx[i]);
    }
    // Serial.print("data :");
    // Serial.print(spi_rx[0],HEX);
    // Serial.print(" ");
    // Serial.print(spi_rx[1],HEX);
    // Serial.print(" ");
    // Serial.print(spi_rx[2],HEX);
    // Serial.print(" ");
    // Serial.print(spi_rx[3],HEX);
    // Serial.print(" ");
    // Serial.println(spi_rx[4],HEX);

    // SPI.endTransaction();
    digitalWrite(SPI_CS, HIGH); // Pull CS high to deselect the KSZ8567

    // The received data is in spi_rx[3] (last byte contains the register value)
    return spi_rx[4];
}