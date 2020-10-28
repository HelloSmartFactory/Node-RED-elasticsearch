#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// For random generation of client ID.
static const char alphanum[] ="0123456789"
                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                              "abcdefghijklmnopqrstuvwxyz"; 
unsigned long prev;
int fail_count = 0;
WiFiClient espClient;
PubSubClient client(espClient);
char jsonOutput[128];

//====================================================
const char *ssid = "yourSSID";
const char *password = "yourPW";
const char* mqtt_server = "youerIPAddress ex192.168.1.168";
const int port = 1883;

const char* Topic = "TwoData";//OneData

unsigned long interval = 300000;//5min

const char* device = "yourdevicename";

//小文字のみ
const char* sensor_a = "yoursensorname";
const char* sensor_b = "yoursensorname";

const char* mqtt_username = "";
const char* mqtt_password = "";

//====================================================




void setup()
{
  Serial.begin(115200);
  while (!Serial);
  Serial.println();

  setup_wifi();
  client.setServer(mqtt_server, port);
  client.setCallback(callback);

  delay(1000);
  Serial.print(" Topic:");
  Serial.println(Topic);
  Serial.print(" interval:");
  Serial.println(interval);
  prev = 0;
}


void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    setup_wifi();
    prev = millis();
  }
  if (!client.connected()) {
    reconnect();
    prev = millis();
  }
  client.loop();

  if ((millis() - prev) >= interval) {
    float a = 1.0;
    float b = 2.0;
    publishJsonTwoData(a,sensor_a,b,sensor_b);
    /*  Topic = OneData
     *  publishJsonOneData(a,sensor_a);
     *  publishJsonOneData(b,sensor_b);
     */
    prev += interval;
  }
  delay(1);
}


//送るデータによってfloatから変更する
//JSON生成してPublishする
void publishJsonOneData(float sensor_data_One,String sensor_One) {
  const size_t CAPACITY = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<CAPACITY> doc;
  JsonObject object = doc.to<JsonObject>();
  object["device"] = device;
  object["sensor"] = sensor_One;
  object["data"] = sensor_data_One;
  serializeJson(doc, jsonOutput);
  //POSTする
  Serial.println(String(jsonOutput));
  client.publish(Topic, jsonOutput);
  fail_count = 0;
}

//送るデータによってfloatから変更する
//JSON生成してPublishする
void publishJsonTwoData(float sensor_data_One,String sensor_One,float sensor_data_Two,String sensor_Two) {
  const size_t CAPACITY = JSON_OBJECT_SIZE(7);
  StaticJsonDocument<CAPACITY> doc;
  JsonObject object = doc.to<JsonObject>();
  object["device"] = device;
  object["sensor_One"] = sensor_One;
  object["data_One"] = sensor_data_One;
  object["sensor_Two"] = sensor_Two;
  object["data_Two"] = sensor_data_Two;
  serializeJson(doc, jsonOutput);
  //POSTする
  Serial.println(String(jsonOutput));
  client.publish(Topic, jsonOutput);
  fail_count = 0;
}


void restart() {
  if (120 <= fail_count++) {
    ESP.restart();
  }
}


//WiFiの接続
void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
    restart();
  }
  Serial.println();
  Serial.println("WiFi connected");
  //IPaddressを出力
  Serial.print("IPaddress");
  Serial.println(WiFi.localIP());
}

//コールバック
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

//再接続
void reconnect() {
  char clientID[10];
  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect

    // Generate ClientID
    for (int i = 0; i < 8; i++) {
        clientID[i] = alphanum[random(51)];
    }
    Serial.print("clientID : ");
    Serial.println(clientID);
    
    if (client.connect(clientID, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
    delay(10);
  }
}
