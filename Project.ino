#include <DHT.h>
#include <WiFiNINA.h>
#include <WiFiClient.h>

// DHT22 setup
#define DHTPIN 2          // Pin connected to the DHT22 data line
#define DHTTYPE DHT22     // DHT22 sensor
DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT sensor

// Soil moisture sensor
const int soilMoisturePin = A0;  // Pin connected to the soil moisture sensor
int soilMoistureLevel = 0;       // Variable to store soil moisture level

// Wi-Fi credentials
const char* ssid = "Akshit";      
const char* password = "12345678"; 

// IFTTT Webhooks settings
const char* iftttKey = "gEfv5pj5o9fp8e4MSJqK-nPOGGCJMrJN3_W-WBXjfW3";  
const char* eventName = "soil_moisture_alert";     
const char* server = "maker.ifttt.com";            // IFTTT server URL

WiFiClient client; // Create a Wi-Fi client object

void setup() {
  Serial.begin(9600); // Start serial communication at 9600 baud rate
  dht.begin();        // Initialize the DHT sensor

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Read temperature and humidity from DHT22 sensor
  float temperature = dht.readTemperature(); // Read temperature in Celsius
  float humidity = dht.readHumidity();       // Read humidity percentage

  // Validate sensor readings (check for NaN values)
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!"); // Print error if readings are invalid
  } else {
    // Print temperature and humidity readings to the Serial Monitor
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C | Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Read soil moisture level
  soilMoistureLevel = analogRead(soilMoisturePin); // Read analog value from soil moisture sensor
  Serial.print("Soil Moisture Level: ");
  Serial.println(soilMoistureLevel);

  // Check for low soil moisture and send IFTTT notification if below threshold
  if (soilMoistureLevel < 1500) {  // Example threshold value for low moisture
    sendIFTTTNotification(temperature, humidity, soilMoistureLevel);
  }

  delay(5000);  // Wait 5 seconds before next reading
}

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password); // Start Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Wait for Wi-Fi to connect
    Serial.print("."); // Print dots to indicate connection progress
  }
  Serial.println(" Connected to Wi-Fi!");
}

// Function to send a notification to IFTTT with temperature, humidity, and soil moisture values
void sendIFTTTNotification(float temperature, float humidity, int soilMoisture) {
  if (WiFi.status() == WL_CONNECTED) { // Ensure Wi-Fi is connected
    // Construct the URL for the IFTTT Webhooks request with values
    String url = "/trigger/" + String(eventName) + "/with/key/" + String(iftttKey) +
                 "?value1=" + String(temperature) +
                 "&value2=" + String(humidity) +
                 "&value3=" + String(soilMoisture);

    // Connect to the IFTTT server
    if (client.connect(server, 80)) {
      // Send the HTTP GET request
      client.print("GET " + url + " HTTP/1.1\r\n");
      client.print("Host: " + String(server) + "\r\n");
      client.print("Connection: close\r\n\r\n");
      client.stop(); // Close the connection
      Serial.println("IFTTT notification sent!");
    } else {
      Serial.println("Failed to connect to IFTTT server"); // Print error if connection fails
    }
  } else {
    Serial.println("Wi-Fi not connected. Unable to send notification."); // Print error if Wi-Fi is disconnected
  }
}
