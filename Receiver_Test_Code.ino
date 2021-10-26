/*
    NRF24L01 // Arduino Pro Mini
    
    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D7
    CSN    ->   D8
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12
    
This Code receiver 4 Channel and PWM Output D3
Please, like share and subscribe : https://www.youtube.com/channel/UCbQR6fCLCjeRXikEqxksrPg
*/


#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h> 

const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //  this code is the same as in the transmitter
RF24 radio(7, 8);  // CE and CSN

// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
  byte ch7;
  byte ch8;
};

Received_data received_data;

Servo channel_1;
Servo channel_2;


//Channel signal on pins D9 and D4
const int channel_3 = 9;
const int channel_4 = 4;

//Motor Driver signal on pins D5 and D6
int FWD = 5;    
int RWD = 6;   

int throttle;
int Speed = 0;

int ch1_value = 1;
int ch2_value = 1;
int ch3_value = 1;
int ch4_value = 0;
int ch5_value = 0;
int ch6_value = 0;
int ch7_value = 1;
int ch8_value = 0;


void reset_the_Data() 
{
  
  // 'safe' values to use when NO radio input is detected
  received_data.ch1 = 255;      //Throttle (channel 1) to 255
  received_data.ch2 = 255;
  received_data.ch3 = 0;
  received_data.ch4 = 0;
  received_data.ch5 = 0;
  received_data.ch6 = 1;
  received_data.ch7 = 0;
  received_data.ch8 = 0;
  
}



/**************************************************/

void setup()
{
  //Attach the servo signal on pins from D3 and D2
  channel_1.attach(3);
  channel_2.attach(2);

  //Attach the Motor Driver signal on pins from D9 and D4
  pinMode(channel_3, OUTPUT);
  pinMode(channel_4, OUTPUT);
  pinMode(FWD, OUTPUT);
  pinMode(RWD, OUTPUT);


  
  //We reset the received values
  reset_the_Data();

  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeIn);
  radio.startListening();
}

/**************************************************/

unsigned long lastRecvTime = 0;

//We create the function that will read the data each certain time
void receive_the_data()
{
    while(radio.available()){
      radio.read(&received_data, sizeof(Received_data));
      lastRecvTime = millis(); //Here we receive the data
      }   
}

/**************************************************/

void loop()
{
  //Receive the radio data
  receive_the_data();

  //This small if will reset the data if signal is lost for 1 sec.
  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    reset_the_Data();
    //Go up and change the initial values if you want depending on
    //your aplications. Put 0 for throttle in case of drones so it won't
    //fly away
  } 
  
throttle = received_data.ch1;
if(throttle >= 127){
   
    Speed = map(throttle , 127, 255, 0, 255);
    digitalWrite(FWD, LOW);
    analogWrite(RWD, Speed);
    
  }
  else { 
   
    Speed = map(throttle,127,0, 0, 255);
    analogWrite(FWD, Speed);
    digitalWrite(RWD, LOW);
    
  }

  ch1_value = map(received_data.ch1,0,255,1000,2000);
  ch2_value = map(received_data.ch2,0,255,1000,2000);

  //Creathe the PWM signals
  channel_1.writeMicroseconds(ch1_value);  
  channel_2.writeMicroseconds(ch2_value);     

}//Loop end 
