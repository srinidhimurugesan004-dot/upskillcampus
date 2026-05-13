#include <WiFi.h>
#include <PubSubClient.h>


const char* ssid = "GALAXY A23";
const char* password = "12345678";

/* ThingsBoard MQTT Server */
const char* mqtt_server = "demo.thingsboard.io";

/* Device Access Token from ThingsBoard */
const char* token = "YOUR_DEVICE_TOKEN";

WiFiClient espClient;
PubSubClient client(espClient);

/* Pin Definitions */
#define PIR_PIN 13
#define BUZZER_PIN 12

/* Connect ESP32 to WiFi */
void setup_wifi()
{
    delay(10);

    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected");
    Serial.println(WiFi.localIP());
}

/* Reconnect MQTT */
void reconnect()
{
    while (!client.connected())
    {
        Serial.print("Connecting to ThingsBoard...");

        if (client.connect("ESP32_Client", token, NULL))
        {
            Serial.println("Connected");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Retry in 5 seconds");

            delay(5000);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    pinMode(PIR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, LOW);

    setup_wifi();

    client.setServer(mqtt_server, 1883);
}

void loop()
{
    if (!client.connected())
    {
        reconnect();
    }

    client.loop();

    int motion = digitalRead(PIR_PIN);

    /* Motion Detected */
    if (motion == HIGH)
    {
        Serial.println("Motion Detected!");

        digitalWrite(BUZZER_PIN, HIGH);

        String payload = "{\"motion\":1}";

        client.publish("v1/devices/me/telemetry", payload.c_str());

        delay(1000);
    }
    else
    {
        Serial.println("No Motion");

        digitalWrite(BUZZER_PIN, LOW);

        String payload = "{\"motion\":0}";

        client.publish("v1/devices/me/telemetry", payload.c_str());

        delay(1000);
    }
}
