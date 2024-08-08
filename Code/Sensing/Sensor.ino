#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>   // Use the WiFiMulti library 

// ADC Data
int ADC_value = 0;
const int numSamples = 10;  // Number of samples for moving average
int samples[numSamples];
int sampleIndex = 0;
float sum = 0;  // Sum of the samples for calculating the moving average
float filteredValue = 0;

const char* host = "192.168.4.1";    // The server URL /IP is about to be connected
const int httpPort = 80;               // About to connect to the server port

ESP8266WiFiMulti wifiMulti;     // Create an ESP8266WiFiMulti object with the name 'wifiMulti'

void wifiClientRequest(){
  WiFiClient client;  
 
  // Read the simulated input values:
  ADC_value = analogRead(A0);

  // Update the moving average
  sum -= samples[sampleIndex];  // Remove the oldest sample from the sum
  samples[sampleIndex] = ADC_value;  // Add the new sample to the array
  sum += ADC_value;  // Add the new sample to the sum

  sampleIndex = (sampleIndex + 1) % numSamples;  // Move to the next sample index
  
  // Calculate the average
  filteredValue = sum / numSamples;

  // Print the raw ADC value and the filtered value to the Serial Monitor
  Serial.print("Raw Data Value A: ");
  Serial.println(ADC_value);
  Serial.print("Filtered Value B: ");
  Serial.println(filteredValue);

  // Put the data information that needs to be sent into the client request
  String url = "/sendData?data=" + String(filteredValue);
         
  // Creates a string to use in HTTP requests
  String httpRequest =  String("GET ") + url + " HTTP/1.1\r\n" +
                        "Host: " + host + "\r\n" +
                        "Connection: close\r\n" +
                        "\r\n";
                        
  Serial.print("Connecting to "); 
  Serial.print(host); 
  
  if (client.connect(host, httpPort)) {  //If the connection fails, the serial output informs the user and then returns to the loop
    Serial.println(" Success");
    
    client.print(httpRequest);          // Sends an HTTP request to the server
    Serial.println("Sending request: ");// Output HTTP request information content through the serial port for reference
    Serial.println(httpRequest);        
  } else{
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

  // Initialize samples array and sum
  for (int i = 0; i < numSamples; i++) {
    samples[i] = 0;
  }
}

void loop() {
  // send data
  wifiClientRequest();
  // Enter deep sleep mode and wake up after 5 seconds
  Serial.println("Entering deep sleep...");
  ESP.deepSleep(5e6); // The parameter units are microseconds, with 5e6 representing 5 seconds
  delay(50);
}
