// Load Wi-Fi library
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <DHT.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define echoPin 18 // attach pin D2 Arduino to pin Echo of HC-SR04
#define trigPin 5 //attach pin D3 Arduino to pin Trig of HC-SR04

#define DHTPIN 19     // Digital pin connected to the DHT sensor
#define DHTTYPE    DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE);

// defines variables
long duration; // variable for the duration of sound wave travel
int distance; // variable

int tank_bar;
// Replace with your network credentials
const char* ssid = "project";
const char* password = "project1234";

const int ledPin =  15;      // the number of the LED pin

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

const int oneWireBus = 4;
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();
  dht.begin();

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT);

  pinMode(ledPin, OUTPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  lcd.setCursor(0, 0);
  lcd.print("IP address");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(4000);
  lcd.setCursor(0, 0);
  lcd.print("SMART TANK LEVEL");
  lcd.setCursor(0, 1);
  lcd.print("MONITOR SYSTEM");
  delay(4000);
  lcd.clear();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  float h = dht.readHumidity();
 
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  float temperatureF = sensors.getTempFByIndex(0);

  lcd.setCursor(0, 0);
  lcd.print(temperatureC);
  lcd.setCursor(6, 0);
  lcd.print("H");
  lcd.print(h);

  lcd.setCursor(0, 1);
  if (distance <= 5) {
    lcd.print("PUMP STATUS: OFF");
  } else if (distance > 5 && distance <= 7) {
    lcd.print("PUMP STATUS: OFF");
  } else if (distance > 7 && distance <= 9) {
    lcd.print("PUMP STATUS: OFF");
  } else if (distance > 9 && distance <= 11) {
    lcd.print("PUMP STATUS: ON");
  } else if (distance > 11 && distance <= 13) {
    lcd.print("PUMP STATUS: ON");
  } else if (distance > 13 && distance <= 15) {
    lcd.print("PUMP STATUS: ON");
  } else if (distance > 15 && distance <= 17) {
    lcd.print("PUMP STATUS: ON ");
  } else if (distance > 17 && distance <= 19) {
    lcd.print("PUMP STATUS: ON ");
  } else if (distance > 19 && distance < 21) {
    lcd.print("PUMP STATUS: ON ");
  } else if (distance >= 21) {
    lcd.print("PUMP STATUS: ON ");
  }

  // Clears the trigPin condition
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin HIGH (ACTIVE) for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2; // Speed of sound wave divided by 2 (go and back)
  // Displays the distance on the Serial Monitor


  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html> <html> <head> <title>Project</title> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.1.2/css/bootstrap.min.css\" integrity=\"sha384-Smlep5jCw/wG7hdkwQ/Z5nLIefveQRIY9nfy6xoR1uRYBtpZgI6339F5dgvm/e9B\" crossorigin=\"anonymous\"> <meta http-equiv=\"refresh\" content=\"1\"> <style type=\"text/css\"> .progress-bar-vertical { width: 300px; min-height: 400px; display: flex; align-items: flex-end; margin-right: 20px; float: left; } .progress-bar-vertical .progress-bar { width: 100%; height: 0; -webkit-transition: height 0.6s ease; -o-transition: height 0.6s ease; transition: height 0.6s ease; } </style> </head> <body style=\"width: 100vw;height: 100vh;\"> <div class=\"jumbotron bg-primary text-light\"> <div> <h1>Smart Tank Management System</h1><hr> dashboard </div> </div> <div class=\"container\"><div class=\"card\"> <div class=\"card-header bg-primary text-light\"> Tank Level </div> <div class=\"card-body\"> <div class=\"progress progress-bar-vertical\"> <div class=\"progress-bar progress-bar-success progress-bar-striped active\" role=\"progressbar\" aria-valuenow=\"100\" aria-valuemin=\"0\" aria-valuemax=\"100\" style=\"height:");
            if (distance <= 5) {
              digitalWrite(ledPin, LOW);
              client.println("100%;\"><span class=\"\">100% Full</span> ");
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-success\">FULL</span> </li> <li>Pump Status : OFF</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 5 && distance <= 7) {
              client.println("90%;\"><span class=\"\">90% Full</span>");
              digitalWrite(ledPin, LOW);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-success\">FULL</span></li> <li>Pump Status : OFF</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 7 && distance <= 9) {
              client.println("80%;\"><span class=\"\">80% Full</span>");
              digitalWrite(ledPin, LOW);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-success\">FULL</span> </li> <li>Pump Status : OFF</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 9 && distance <= 11) {
              client.println("70%;\"><span class=\"\">70% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-info\">MEDIUM</span> </li> <li>Pump Status : OFF</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 11 && distance <= 13) {
              client.println("60%;\"><span class=\"\">60% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-info\">MEDIUM</span> </li> </li> <li>Pump Status : OFF</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 13 && distance <= 15) {
              client.println("50%;\"><span class=\"\">50% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-info\">MEDIUM</span> </li> </li> <li>Pump Status : OFF</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 15 && distance <= 17) {
              client.println("40%;\"><span class=\"\">40% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-info\">MEDIUM</span> </li> </li> <li>Pump Status : ON</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 17 && distance <= 19) {
              client.println("30%;\"><span class=\"\">30% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-info\">MEDIUM</span> </li> </li> <li>Pump Status : ON</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance > 19 && distance < 21) {
              client.println("20%;\"><span class=\"\">20% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-danger\">PARTIALLY EMPTY</span> </li> </li> <li>Pump Status : ON</li> <li>Tank Height : 21 CM</li> </ul> </div> ");
            } else if (distance >= 21) {
              client.println("10%;\"><span class=\"\">10% Full</span>");
              digitalWrite(ledPin, HIGH);
              client.println("</div> </div> <ul> <li>Water Level :");
              client.println(distance);
              client.println("CM <span class=\"text-danger\">PARTIALLY EMPTY</span> </li> </li> <li>Pump Status : ON</li> <li>Tank Height : 21 CM</li> </ul> </div>");
            }


            client.println("<div style=\"margin-top: 30px;\" class=\"card\"> <div class=\"card-header bg-warning text-light\"> Water Temperature | Humidity</div> <div class=\"card-body\"> <h1> Temp: ");
            client.println(temperatureC);
            client.println("</h1>");
            client.println("<div class=\"card-body\"> <h1> Humidity: "); 
            client.println(h);
            client.println("</h1> </div>");
            client.println("</div> </div></div> </div>");
            client.println("</body> </html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}