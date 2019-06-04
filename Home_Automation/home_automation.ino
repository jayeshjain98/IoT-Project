#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <dht11.h>

#include <Servo.h> 

int servoPin = D5;
Servo servo;
int powerPin = D2;
int angle = 0; 
int on_flag =0;
int off_flag = 0;

int relayFirstPin = D6;
int relaySecondPin = D7;

#define ON   0
#define OFF  1
#include <DHT.h>        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin 0
DHT dht(dht_dpin, DHTTYPE);


/* 
Define the Pin of Humidity Sensor LDR 
which is attached to A2 on Arduino
*/
//int dht11Pin= D3;   

int lightPin=A0;


/* Global Varibales to store the humidity value */
float humidity;

/* Global Varibales to store the humidity value */
float temperature;



/* Global Variable to access the library functions */
//dht11 myDHTSensor;
//#include <ArduinoJson.h>
HTTPClient http; 
 

 
 int data_flask;
 int power;
 int powerStatus;
 int intensity;

 
void setup () {
 
  Serial.begin(9600);
  WiFi.begin("Jayesh", "1234567890");
  pinMode(powerPin , INPUT);
   dht.begin();
      // Setting the Pin Mode for the Sensor attached, either INPUT or OUTPUT
//      pinMode(dht11Pin, INPUT); 
      pinMode(powerPin, INPUT);
      // Print the output on the Serial Monitor
      Serial.println("SMART ROOM PROJECT");
      
      pinMode(lightPin, INPUT);

      servo.attach(servoPin); 

      pinMode(relayFirstPin, OUTPUT);
      pinMode(relaySecondPin, OUTPUT);
  
      setRelayStatus(OFF, OFF);
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.println("Connecting..");
 
  }
  Serial.println("Connected to WiFi Network");
 
}
 
void loop() {
 
    
     humidity = dht.readHumidity();

        // Call the function to get the sensor value and store in our global variable
       temperature = dht.readTemperature();
        powerStatus = digitalRead(powerPin);
        Serial.print("power Status:");
        Serial.println(powerStatus);
        // Print the output on the Serial Monitor
        Serial.println("");
        Serial.println("******************************");
        
        Serial.print("Humidity in Room =  ");
        Serial.print(humidity);
        Serial.println(" % Relative Humidity");

        Serial.print("Temparature of the Room =  ");
        Serial.print(temperature);
        Serial.println(" degree Celsius");

        int lightStatus= analogRead(lightPin);
        int intensity= lightStatus*(5.0/1023.0)*1000;
        intensity/=10;
        Serial.print("light intensity:");
        Serial.println(intensity);


        Serial.println("******************************");
        Serial.println("");

          if(data_flask == 1)
        {
                  Serial.println("------------------------");

          Serial.println("WELCOME! YOU ARE HOME");
        if (intensity>250&& intensity<380)
        {
          Serial.println("opening curtains");
          servo.write(180);
        }
        else
        {
          Serial.println("closing curtains");
          servo.write(0);
        }
        if(temperature>20)
        {
          Serial.println("Temprature is high AC is ON");
          setRelayStatus(ON, ON);
        }
        else
        {
          Serial.println("Normal temperature AC is OFF");
          setRelayStatus(OFF, OFF);

        }
        delay(2000);
        }
        else
        {
          Serial.println("NOBODY IS HOME POWER IS CUT OFF");
          setRelayStatus(OFF, OFF);
          servo.write(0);

          
        }
        Serial.println("-----------------------------");
        // Delay of 5 sec to take the sensor value again

postToFlaskServer();
    getDataFromFlask();
        
 
  delay(5000);    //Send a request every 10 seconds
 
}



void postToFlaskServer()
{

if (WiFi.status() == WL_CONNECTED) {
  String data = "";
      data = "";
      data += "{";
      data += "\"powerStatus\":\""+String(powerStatus)+"\",";   // Humidity  
      data += "\"intensity\":\""+String(intensity)+"\",";// Power Status ( ON=1 or OFF=0 ) 
      data += "\"temprature\":\""+String(temperature)+"\","; // Temprature
      data += "\"humidity\":\""+String(humidity)+"\"";    //Humidity
      data += "}";
  Serial.println(data);

    //Declare object of class HTTPClient
 
    http.begin("http://192.168.43.233:80/requestType");      //Specify request destination
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    

 
    int httpCode = http.POST(data);   //Send the request
    String payload = http.getString();                                        //Get the response payload
 
    Serial.println(httpCode);   //Print HTTP return code
    Serial.println(payload);    //Print request response payload
 
    http.end();  //Close connection
}
else {
 
    Serial.println("Error in WiFi connection");
 
  }
  
}


void getDataFromFlask ()
{ 

  if (on_flag == 0  && powerStatus == 1)
  {
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
      //Declare an object of class HTTPClient
 
    http.begin("http://192.168.43.233:80/requestType"); //Specify request destination
 
    int httpCode = http.GET(); //Send the request
    Serial.println(httpCode);
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString(); //Get the request response payload
       data_flask = payload.toInt();
       if (data_flask ==1)
       {
        on_flag =1;
       }
     
      Serial.println(payload);             //Print the response payload
 
    }else Serial.println("An error ocurred");
 
    http.end();   //Close connection
 
  }
  }
  else if (off_flag == 0 && powerStatus == 0)
  {
    if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
      //Declare an object of class HTTPClient
 
    http.begin("http://192.168.43.233:80/requestType"); //Specify request destination
 
    int httpCode = http.GET(); //Send the request
    Serial.println(httpCode);
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString(); //Get the request response payload
       data_flask = payload.toInt();
        if (data_flask ==0)
       {
        off_flag =1;
       }
     
      Serial.println(payload);             //Print the response payload
 
    }else Serial.println("An error ocurred");
 
    http.end();   //Close connection
 
  }
  }
  
}

/*********** Humidity Read function*********/


//float getDhtHumidity(int dht11Pin)
//{   
//         // Read the sensor value using the dht library read function 
//        myDHTSensor.read(dht11Pin);
//
//        // After reading the value which is stored in the library variable
//        return (float)myDHTSensor.humidity;
//}
//
//float getDhtTemperature(int dht11Pin)
//{    
//         // Read the sensor value using the dht library read function 
//        //myDHTSensor.read(dht11Pin);
//
//        // After reading the value which is stored in the library variable
//        return (float)myDHTSensor.temperature;
//}

void setRelayStatus( unsigned char status_1,  unsigned char status_2)
{
  digitalWrite(relayFirstPin, status_1);
  digitalWrite(relaySecondPin, status_2);
}



/*
      DHT11 Sensor 
      
      Humidity Range: 20-90% RH
      Humidity Accuracy: Â±5% RH
      Temperature Range: 0-50 Â°C
      Temperature Accuracy: Â±2% Â°C
      Operating Voltage: 3V to 5.5V

*/    

