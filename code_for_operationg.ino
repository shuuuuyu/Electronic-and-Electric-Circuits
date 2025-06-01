/*
18~27度>60%
要測試的話20、21的帳密要改，30、31現在是我的
如果超出是一溫溼度傳telegram包括噴水
*/
#include "DHT.h"
#define dhtPin 16      //讀取DHT11 Data
#define dhtType DHT11 //選用DHT11   

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h> // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

int relay = 17;//繼電器17腳
const char* ssid = "WIFI-NTNU-401";
const char* password = "tahrd401";
String welcome = " ";
float h = 0.0;
float t = 0.0;
unsigned long lastAlertTime = 0;   // 上一次發送訊息的時間
unsigned long alertCooldown = 60000; // 冷卻時間（以毫秒計，這是 60 秒）
String currentChatId = "";
DHT dht(dhtPin, dhtType); // Initialize DHT sensor

#define CHAT_ID "7661979220"// Initialize Telegram BOT
#define BOTtoken "7965740720:AAHbP3xZQ_fzRbYmf_zOI5Qphi0Ex1Eyj6o"  // your Bot Token (Get from Botfather)
#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    currentChatId = chat_id;
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    if(text == "/start"){
      welcome = "歡迎" + bot.messages[i].from_name + "!\n";
      welcome += "我會幫您管理好您植物的溫溼度，並將訊息傳給您\n";
      welcome += "如果隨時想知道溫溼度可以輸入「溫溼度」";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "溫溼度") {
      String message = "溫度: " + String(t) + " ºC \n";
      message += "濕度: " + String (h) + " % \n";
      bot.sendMessage(chat_id, message, "");
    }  
  }
}


void setup() {
  Serial.begin(115200);//設定鮑率115200
  dht.begin();//啟動DHT  // DHT11-1b
  pinMode(relay, OUTPUT);
  digitalWrite(relay, LOW);

  #ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

/*  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif*/
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
  client.setInsecure();
}

void loop() {
  h = dht.readHumidity();//讀取濕度
  t = dht.readTemperature();//讀取攝氏溫度

  if (isnan(h) || isnan(t)) {
    Serial.println("無法從DHT傳感器讀取！");
    return;
  }

  //serial port
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("% \t");
  Serial.print("C: ");
  Serial.print(t);
  Serial.print("*C \t");

  //蘭花溫度18~27濕度>60%
  if ((t > 27.0 || h < 60.0) && (millis() - lastAlertTime > alertCooldown)) {
    String water = "感測到溫度:"+ String(t) + "度，濕度:" + String(h) + "%\n開啟噴水裝置";
    if (currentChatId != "") {
      bot.sendMessage(currentChatId, water, "");
    }
    digitalWrite(relay, HIGH);
    delay(500);
    digitalWrite(relay, LOW);
    delay(2000);
    lastAlertTime = millis(); // 更新上次發送時間
  }
  else if((t<18) && (millis() - lastAlertTime > alertCooldown)){
    String low = "感測到溫度:"+ String(t) + "度，濕度:" + String(h) + "%\n請注意植物環境溫度過低";
    if (currentChatId != "") {
      bot.sendMessage(currentChatId, low, "");
    }
    delay(2000);
    lastAlertTime = millis(); // 更新上次發送時間
  }

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  delay(5000);//延時5秒
}
//結束




  /*if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }*/

/*String getReadings(){

  t = dht.readTemperature();
  h = dht.readHumidity();
  String message = "溫度: " + String(temperature) + " ºC \n";
  message += "濕度: " + String (humidity) + " % \n";

  }

  return message;
}*/