#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Stepper.h>// stepper motor driver library

// -----------------------------------------  Instrument control part start  -----------------------------------------
const int stepsPerRevolution = 600;   // The number of steps it takes to turn a circle

Stepper myStepper(stepsPerRevolution, 2, 0, 1, 3);
// -----------------------------------------  Instrument control part end  -----------------------------------------

// Create an ESP8266WebServer object listening on port 80
ESP8266WebServer server(80);

// Define the SSID and password of the hotspot
const char *ssid = "ESP8266_HOTSPOT";
const char *password = "12345678";

// Set a fixed IP address
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// String data used to receive
String receivedData = "";

bool update_flag = false;
int new_data,old_data;

void setup(void){
  //  Serial.begin(115200);          // DEBUG -- Start serial communication

  // Configure a fixed IP address
  WiFi.softAPConfig(local_IP, gateway, subnet);
  // Start WiFi hotspot
  WiFi.softAP(ssid, password);

  // Processing server receives data function
  server.on("/sendData", handleSendData);        
  server.begin();                            // Starting the web service

  for(int i = 0;i < 3;i++)
  {
    digitalWrite(2,HIGH);
    delay(300);
    digitalWrite(2,LOW);
    delay(300);
  }

  myStepper.setSpeed(50); // Set rotation speed
  myStepper.step(-stepsPerRevolution);  // Move the meter pointer to the 0 scale
  delay(100);
}
 
void loop(void){
  server.handleClient();                    // Check for HTTP server access

  new_data = receivedData.toInt();
  // The pointer position is changed only when there are data updates
  if(update_flag == true)
  {
    // The new data is greater than what is currently displayed on the meter, and the pointer moves clockwise in new_data-CurrentPointerData steps
    // The new data is less than the current data displayed on the meter, and the pointer moves counterclockwise in the number of steps CurrentPointerData-new_data
    myStepper.step((new_data - old_data));
    //Serial.println("Remove.");
    //Serial.println(new_data - old_data);
    old_data = new_data;
    update_flag = false;
  }
}
 
void handleSendData(){
  receivedData = server.arg("data");       // Retrieve the string data sent by the client
  server.send(200, "text/plain", "Data received"); // Sending an HTTP response
  //Serial.println("Received data: " + receivedData); // Print the received data
  update_flag = true;
}
