/*
  Web client

  This sketch connects to a website (http://api.airvisual.com)
  using the WiFi module.

  This example is written for a network using WPA encryption. For
  WEP or WPA, change the WiFi.begin() call accordingly.

  This sketch is written for a network using WPA encryption. For
  WEP or WPA, change the WiFi.begin() call accordingly.


  created 13 July 2010
  by dlf (Metodo2 srl)
  modified 31 Dec 2024
  by MI van Leersum

  Find the full UNO R4 WiFi Network documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#wi-fi-web-client
*/


#include "WiFiS3.h"
#include "arduino_secrets.h"
#include "locations.h" 

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(54,92,111,164);  // numeric IP for IQ AIR (no DNS)

//Documentation for IQAir API can be found here: https://api-docs.iqair.com/?version=latest
char server[] = "api.airvisual.com";    // name address for IQ AIR (using DNS)
char method[] = "GET";
char apiKey[] = SECRET_APIKEY;

int IQAirApiResult[NumberOfLocations]; // Array that stores the return values for all locations
bool locationChanged = true; // Indication to deflate and inflate to preset pressure

unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = ApiRequestInterval * 1000L; // delay between updates, in milliseconds
bool firstRun = true;

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

/* -------------------------------------------------------------------------- */
void setup() {
/* -------------------------------------------------------------------------- */  
  // Initalize all outputs to Low. Just to be sure...
  analogWrite(pump1,0);
  analogWrite(pump2,0);
  digitalWrite(valve1,LOW);
  digitalWrite(valve2,LOW);
  digitalWrite(valve3,LOW);
  
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    
    // wait 10 seconds for connection and blink LED to indicate WIFI setup:
    for (int i = 0; i <=4; i++){
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);                       // wait for a second
    }
  }
  
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
  //httpRequest(0); // To start with get the first request 
}

/* -------------------------------------------------------------------------- */
String read_response() {
/* -------------------------------------------------------------------------- */    
  String resp;
  while (client.available()) {
    resp = client.readStringUntil('\r');
    if (debug){
      Serial.print(resp);
    }
  } 
  return resp; 
}

/* -------------------------------------------------------------------------- */
void loop() {
/* -------------------------------------------------------------------------- */  
  if ((millis() - lastConnectionTime > postingInterval) || firstRun ){
    firstRun = false;
    // index keeps track of what location we are requesting
    bool dataProcessed = true; // Start with a http api request
    int index = 0;
  while (index < NumberOfLocations){
    //if ((millis() - lastConnectionTime > postingInterval) && dataProcessed) {
    if (dataProcessed) {
      httpRequest(index);
      dataProcessed = false;
    } 
    while (client.available()) {
      // if there's incoming data from the net connection do the following:  
      int apiGetVal = (getValueFromJson(read_response(), SearchKey).toInt());
      //if (apiGetVal > 0){
      //Serial.print(Location[index][0]+" ");
      IQAirApiResult[index] = apiGetVal;
      Serial.println();
      //Serial.println(IQAirApiResult[index]); 
      if(debug){
        for (int i = 0; i < NumberOfLocations; i++){
          Serial.print(Location[i][0]+" ");
          Serial.println(IQAirApiResult[i]);
        }
      }
      dataProcessed = true;
      index++;
    }
  }
  // note the time that the connection was made:
  lastConnectionTime = millis();
  }
  
  // Main loop to run the Air Program here.....
  for (int j = 0; j < NumberOfIntervals; j++){
    //Serial.println("AirLoop :" + IQAirApiResult[selectedLocation] );
    if ((IQAirApiResult[selectedLocation] >= AirIntervals[j][0]) && (IQAirApiResult[selectedLocation] <= AirIntervals[j][1])){
      if (locationChanged){   // If new location is selected, reset to default.
        Serial.println("Reset air program to initial state.");
        digitalWrite(LED_BUILTIN, HIGH);  // Visual indication of reset on
        deflate(AirIntervals[j][2]); 
        inflate(AirIntervals[j][2]); 
        digitalWrite(LED_BUILTIN, LOW);  // Visual indication of reset off
        locationChanged = false;
      } 
      //RunAirProg: inflate, pause1, deflate, pause2
      runAirProgram(AirIntervals[j][3],AirIntervals[j][4],AirIntervals[j][5],AirIntervals[j][6]); 
    }  
  }
}

/* -------------------------------------------------------------------------- */
void runAirProgram(int infl,int p1, int defl, int p2){
/* -------------------------------------------------------------------------- */
  if (debug){Serial.println("Running air program for location: " + Location[selectedLocation][0] + " with air quality: " + IQAirApiResult[selectedLocation]);}
  inflate(infl);
  if (debug){Serial.println("pause1");}
  delay(p1);
  deflate(defl);
  if (debug){Serial.println("pause2");}
  delay(p2);
}

/* -------------------------------------------------------------------------- */
void inflate(int Time){
/* -------------------------------------------------------------------------- */
  if (debug){Serial.println("inflate");}
  digitalWrite(valve1, LOW); // Turn the valve to inflate
  analogWrite(pump1, 254);   // Turn the pump to max
  delay(Time);
  analogWrite(pump1, 0);     // Turn the pump off
}

/* -------------------------------------------------------------------------- */
void deflate(int Time){
/* -------------------------------------------------------------------------- */
  if (debug){Serial.println("deflate");}
  analogWrite(pump1, 0);   // Turn the pump off
  digitalWrite(valve1, HIGH); // Turn the valve to deflate
  delay(Time);
  digitalWrite(valve1, LOW); // Turn the valve to deflate
}

/* -------------------------------------------------------------------------- */
String getValueFromJson(String response,String key){
/* -------------------------------------------------------------------------- */
  int start = (response.indexOf(key));
  String value = (response.substring(start+key.length()+2,start+key.length()+5));
  //If third character is a "," then we need 2 digits not 3
  if (value.substring(2) == ","){
    value = value.substring(0,2);  
  }
  return value;
}

// this method makes a HTTP connection to the server:
/* -------------------------------------------------------------------------- */
void httpRequest(int i) {
/* -------------------------------------------------------------------------- */  
  // close any connection before send a new request.
  // This will free the socket on the NINA module
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    //Serial.println("connected to server");
    // Make a HTTP request:
    client.print(method);
    client.print(" ");
    //client.print(resource1);
    client.println("/v2/city?city="+Location[i][0]+"&state="+Location[i][1]+"&country="+Location[i][2]+"&key="+apiKey);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.print(server);
    client.println("Connection: close");
    client.println();
    // note the time that the connection was made:
    // lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}

/* -------------------------------------------------------------------------- */
void printWifiStatus() {
/* -------------------------------------------------------------------------- */  
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
