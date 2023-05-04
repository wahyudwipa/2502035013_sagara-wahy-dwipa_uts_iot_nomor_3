#include <Wire.h>
#include <BH1750.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi credentials
const char* ssid = "kerinci#60";
const char* password = "begawan01";

// MQTT credentials and settings
const char* mqtt_server = "broker.emqx.io";
const int mqtt_port = 1883;
const char* mqtt_username = "sagaraganteng";
const char* mqtt_password = "ujian";
const char* mqtt_subscribe_topic = "semangat";
const char* mqtt_publisher_topic = "emosi";

WiFiClient espClient;
PubSubClient client(espClient);

// Pin definitions
const int LIGHT_SENSOR_ADDRESS = 0x23; // I2C address of light sensor
const int LED_PIN = 2; // LED

// Sensor type definitions
const int LIGHT_SENSOR_TYPE = BH1750::ONE_TIME_HIGH_RES_MODE; // 1 lx resolution

// Sensor objects
BH1750 lightMeter; // I2C address 0x23

void setup() {
  Serial.begin(115200); // Initialize serial communication
  Wire.begin(21, 22); // default pin untuk SDA, SCL
  lightMeter.begin(); // light sensor aktif
  delay(100);  // menunggu sensor aktif
  pinMode(LED_PIN, OUTPUT); // untuk LED
  
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Menghubungkan ke WiFi..");
  }
  
  Serial.println("Terhubung ke jaringan WiFi");

  // Connect to MQTT broker
  client.setServer(mqtt_server, mqtt_port);
  
  while (!client.connected()) {
    Serial.println("Mencoba Terhubung ke MQTT broker...");
    
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("Terhubung ke MQTT broker");
    } else {
      Serial.print("Gagal ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  static unsigned long lastLightTime = 0; // This static variable is for the light to be read every 2 seconds

  // Take light reading every 3 seconds
  if (millis() - lastLightTime >= 3000) {
    uint16_t lux = lightMeter.readLightLevel(); // To
      Serial.print("Cahayanya: ");
  Serial.print(lux);
  Serial.println(" lux");
      // data akan dipublish ke sensor MQTT
  if (client.connected()) {
    client.connect("ESP32Client", mqtt_username, mqtt_password);
    client.loop();
    
  if (lux > 400) {
    digitalWrite(LED_PIN, HIGH);
    char buffer[10];
    snprintf(buffer, 10, "%d", lux);
    client.publish(mqtt_publisher_topic, ("Warning, 2502035013 " + String(buffer) + " lux").c_str());
  } if (lux < 400)
  {
    digitalWrite(LED_PIN, LOW);
    char buffer[10];
    snprintf(buffer, 10, "%d", lux);
    client.publish(mqtt_publisher_topic, ("Closed, 2502035013 " + String(buffer) + " lux").c_str());
  }
 
}
}
  }
 
  // Maintain MQTT connection
  /*if (!client.connected()) {
    client.connect("ESP32Client", mqtt_username, mqtt_password);
  }
  client.loop();

  delay(10);*/
