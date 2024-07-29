#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   // Use the WiFiMulti library 

// ADC Data
int ADC_value = 0;

// Kalman filter variables
double Q = 0.1;   // Process noise covariance
double R = 5;     // Measurement noise covariance
double P = 1;     // Error covariance
double K = 0;     // Kalman gain
double X = 0;     // Estimated value

const char* host = "192.168.4.1";    // The server URL /IP is about to be connected
const int httpPort = 80;               // About to connect to the server port

ESP8266WiFiMulti wifiMulti;     // Create an ESP8266WiFiMulti object with the name 'wifiMulti'

// Kalman filter function
double kalmanFilter(double measurement) {
  // Prediction update
  P = P + Q;
  
  // Measurement update
  K = P / (P + R);
  X = X + K * (measurement - X);
  P = (1 - K) * P;
  
  return X;
}

void wifiClientRequest(){
  WiFiClient client;  
 
  // Read the simulated input values:
  ADC_value = analogRead(A0);
  Serial.printf("A:%d\n",ADC_value);

  // Apply Kalman filter to ADC value
  double filtered_ADC_value = kalmanFilter(ADC_value);

  Serial.printf("B:%d\n",(int)filtered_ADC_value);
  // Put the data information that needs to be sent into the client request
  String url = "/sendData?data=" + String(filtered_ADC_value);
         
  // Creates a string to use in HTTP requests
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";
                        
  Serial.print("Connecting to "); 
  Serial.print(host); 
  
  if (client.connect(host, httpPort)) {  // If the connection fails, the serial output informs the user and then returns to the loop
    Serial.println(" Success");
    
    client.print(httpRequest);          // Sends an HTTP request to the server
    Serial.println("Sending request: ");// Output HTTP request information content through the serial port for reference
    Serial.println(httpRequest);        
  } else {
    Serial.println(" failed");
  }
  
  client.stop();                         
}

void setup() {
  Serial.begin(115200);                  // Start serial communication
  
  wifiMulti.addAP("ESP8266_HOTSPOT", "12345678"); // Enter a list of WiFi ids and passwords you want to connect to here
  
  while (wifiMulti.run() != WL_CONNECTED) { // Try a wifi connection.
    delay(250);
    Serial.print('.');
  }
 
  // After successful WiFi connection, the serial port monitor will output the successful connection information
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Output the connected WiFi name through the serial port monitor
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Output the IP of ESP8266-NodeMCU through the serial port monitor

}

void loop() {
  // send data
  wifiClientRequest();
  // Enter deep sleep mode and wake up after 5 seconds
  Serial.println("Entering deep sleep...");
  ESP.deepSleep(5e6); // The parameter units are microseconds, with 5e6 representing 5 seconds
  delay(50);
}
