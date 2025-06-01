#include <DHT.h>

#define DHTPIN 17       // DHT 資料接腳為 GPIO17
#define DHTTYPE DHT11   
DHT dht(DHTPIN, DHTTYPE);

#define RELAY_PIN 16    // Relay 在 GPIO16

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // 預設 Relay 關閉
}

void loop() {
  float temperature = dht.readTemperature();  // 讀取溫度 (Celsius)
  float humidity = dht.readHumidity();        // 讀取濕度 (%)

  // 檢查讀值是否有效
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("讀取 DHT 感測器失敗！");
    return;
  }

  Serial.print("溫度: ");
  Serial.print(temperature);
  Serial.print(" °C  濕度: ");
  Serial.print(humidity);
  Serial.println(" %");

  // 判斷條件並控制繼電器
  if (temperature > 30 && humidity < 90) {
    digitalWrite(RELAY_PIN, LOW);  // 啟動繼電器
  } else {
    digitalWrite(RELAY_PIN, HIGH);   // 關閉繼電器
  }

  delay(2000); // 每兩秒讀取一次
}
