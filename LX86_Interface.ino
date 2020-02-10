#include <LiquidCrystal.h>

// Arduino Dot Matrix
// Provide an interface to a parallel port dot matrix printer (Epson LX-86) via USB serial. 


// Parallel port pins 
#define STROBE_PIN 22
#define DATA_0 23
#define DATA_1 24
#define DATA_2 25
#define DATA_3 26
#define DATA_4 27
#define DATA_5 28
#define DATA_6 29
#define DATA_7 30
#define ACK_PIN 31
#define BUSY_PIN 32
#define PE_PIN 33
#define ERR_PIN 34
#define RST_PIN 35

int displayStatus = 0;
#define PRINTER_READY 0
#define PAPER_OUT 1
#define PRINTER_ERROR 2


class Printer
{
  public:
    bool paperOut; 
    bool error;
    
    Printer()
    {
      paperOut = false;
      error = false;
    }
    
    int init()
    {
      // Set I/O pins:
      pinMode(STROBE_PIN,  OUTPUT);
      pinMode(DATA_0,      OUTPUT); 
      pinMode(DATA_1,      OUTPUT); 
      pinMode(DATA_2,      OUTPUT); 
      pinMode(DATA_3,      OUTPUT); 
      pinMode(DATA_4,      OUTPUT); 
      pinMode(DATA_5,      OUTPUT); 
      pinMode(DATA_6,      OUTPUT); 
      pinMode(DATA_7,      OUTPUT); 
      pinMode(ACK_PIN,     INPUT); 
      pinMode(BUSY_PIN,    INPUT);
      pinMode(PE_PIN,      INPUT); 
      pinMode(ERR_PIN,     INPUT); 
      pinMode(RST_PIN,     OUTPUT);
      
      // Set pins that need to be held high
      digitalWrite(STROBE_PIN, HIGH);
      digitalWrite(RST_PIN, HIGH);

      delay(100);
      
      return 0; 
    }
    void reset()
    {
      digitalWrite(RST_PIN, LOW);
      delayMicroseconds(100);
      digitalWrite(RST_PIN, HIGH); 
    }
    int printChar(char input)
    {
      // Check if there are any error conditions 

      setByte(input);
      delayMicroseconds(1000);
      
      // Blip the strobe pin 
      strobe();
      delay(1);
      
      // Wait for ACK for a bit
      int timeout = 300;
      int startTime = millis(); 
      while(millis() < startTime + timeout); 
      {
        if(!digitalRead(ACK_PIN))
          return 0;
      }
      return 1; 
    }
    
    int print(char *input)
    {
      int i; 
      
      for(i = 0; input[i] != '\0'; i++)
      {
        printChar(input[i]);
      }
    }
    
    // Should probably move the printer specific methods into a child class
    int letterQuality()
    {
      printChar(0x1B);
      printChar('x');
      printChar(0x01); 
    }
    int draftQuality()
    {
      // TODO
    }
    int lineFeed()
    {
      return printChar(0xA); 
    }
    int formFeed()
    {
      return printChar(0xC);
    }
    
  private:
    int setByte(byte input) 
    {
      // don't touch the data lines if the printer is showing busy
      if (!digitalRead(BUSY_PIN))
        return 1;
        
      // Set the pins using bitmasks
      digitalWrite(DATA_0, input & 0x01);
      digitalWrite(DATA_1, input & 0x02);
      digitalWrite(DATA_2, input & 0x04);
      digitalWrite(DATA_3, input & 0x08);
      digitalWrite(DATA_4, input & 0x10);
      digitalWrite(DATA_5, input & 0x20);
      digitalWrite(DATA_6, input & 0x40);
      digitalWrite(DATA_7, input & 0x80);
      
      return 0; 
    }
    void strobe()
    {
      digitalWrite(STROBE_PIN, LOW);
      delayMicroseconds(3);
      digitalWrite(STROBE_PIN, HIGH);
    }
    int readStatus()
    {
      paperOut = digitalRead(PE_PIN); 
      if(paperOut) return 1;
      error = !digitalRead(ERR_PIN);
      if(error) return 1;
      
      return 0; 
    }
};

// Printer
Printer printer;

// DFRobot LCD shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(9600);
  printer.init();
  
  delay(500);
}

char incomingByte;

void loop()
{
  if(Serial.available())
  {
    incomingByte = Serial.read();
    
    printer.printChar(incomingByte);
  }
}
