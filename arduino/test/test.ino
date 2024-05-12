/**
 * @brief Test suite Integration and Unit Tests
 */

/* === Includes ============================================================ */

// External Libraries
#include <M5Core2.h>
#include <FastLED.h>
#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX

#include <WiFi.h>
#include <PubSubClient.h>

// Custom Libraries
#include "../main/tof.h"
#include "../main/led.h"

/* === MQTT ================================================================ */

WiFiClient mqtt_client;
PubSubClient client(mqtt_client);

/**
 * mqtt_callback()
 * @brief Callback function to retrieve mqtt data from specified topic and prints
 *      to the M5Core2 LCD screen
*/
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    M5.Lcd.printf("[%s]\n", topic);

    for (int i = 0; i < length; i++) {
        M5.Lcd.print((char)payload[i]);
    }

    M5.Lcd.println();
}

/**
 * wifi_reConnect()
 * @brief Reconnects the M5Core2 to the specified WiFi network
 * @ref CSSE4011 Lecture 4
*/
void wifi_reConnect() {

    while (!client.connected()) {

        M5.Lcd.print("Attempting MQTT connection...");

        // Connect to the wifi client and publish to the specified topic
        if (client.connect(String(NodeID).c_str())) {
            M5.Lcd.printf("\nSuccessfully connected\n");
            // Once connected, publish an announcement to the topic.

            client.publish(mqtt_topic, "M5Core2 connected");
            // ... and resubscribe.
            client.subscribe(mqtt_topic);
        } else {
            // If unable to connect, try again in 5 seconds
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}

/**
 * wifi_init()
 * @brief Initialise the wifi connection to the MQTT broker and display connection
 *      status on the M5Core2 LCD screen
 * @ref CSSE4011 Lecture 4
*/
void wifi_init() {

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    M5.Lcd.setCursor(0, 50);
    M5.Lcd.println("Trying to connect to: ");
    M5.Lcd.println(ssid);

    // Retry connection every 200ms
    while (WiFi.status() != WL_CONNECTED) {
        delay(200);
        M5.Lcd.print("*");
    }
    // Connected. Report configuration
    M5.Lcd.println();
    M5.Lcd.print("IP address: ");
    IPAddress ip = WiFi.localIP();
    M5.Lcd.println(ip);
    NodeID = ip[3];
    M5.Lcd.print("Node ID: ");
    M5.Lcd.println(ip[3]);
}

/* === Functions =========================================================== */

void setup() {
  wifi_init();
}

void loop() {
  //
}