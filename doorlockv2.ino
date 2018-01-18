#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
const char* ssid     = "PunyaVirnie";  
const char* password = "12345678";

const char* host     = "api.feb-ubl.com"; // Your domain  
String path          = "/Api_Siater/v2/Json/M2.2.json";  
const int relay      = 4;
const int trigPin    = 12;
const int echoPin    = 13 ;
long duration, distance;
void setup() {  
  pinMode(relay, OUTPUT); 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(115200);

  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int wifi_ctr = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");  
  Serial.println("IP address: " + WiFi.localIP());
}
void loop() {
  delay(1000);
  jarak();
  Serial.println(distance);  
  Serial.println("connecting to ");
  Serial.println(host);
  WiFiClient client;
 
  if (distance <150) {
    digitalWrite(relay,HIGH);
    Serial.println("Kebuka");
    delay(500);
  }else{

  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + path + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: keep-alive\r\n\r\n");

   delay(500); 
  String section="header";
  while(client.available()){
    String line = client.readStringUntil('\r');
    // Serial.print(line);
    // we’ll parse the HTML body here
    if (section=="header") { // headers..
      Serial.print(".");
      if (line=="\n") { // skips the empty space at the beginning 
        section="json";
      }
    }
    else if (section=="json") {  // print the good stuff
      section="ignore";
      String result = line.substring(1);

      // Parse JSON
      int size = result.length() + 1;
      char json[size];
      result.toCharArray(json, size);
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& json_parsed = jsonBuffer.parseObject(json);
      if (!json_parsed.success())
      {
        Serial.println("parseObject() failed");
        return;
      }
      if (strcmp(json_parsed["light"], "on") == 0 ) {
        digitalWrite(relay, HIGH); 
        Serial.println("LED ON");
        
      }
      else {
        digitalWrite(relay, LOW);
        Serial.println("LED OFF");
      }
    }
  }
}
  if (distance >= 200 || distance <= 0){
    Serial.println("Out of range");
  }
}
