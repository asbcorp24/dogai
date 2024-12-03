#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Dogbsh";
const char* password = "123456789";

const int ledPin = 2;    // Встроенный светодиод на большинстве плат ESP32
const int buzzerPin = 4; // Пин для пьезоизлучателя
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
AsyncWebServer server(80);
unsigned long ledOffTime = 0; // Время, когда нужно выключить светодиод
bool isLedOn = false;         // Флаг состояния светодиода

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  digitalWrite(buzzerPin, LOW);

  WiFi.softAP(ssid, password);
  if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
    Serial.println("Ошибка настройки статического IP");
  }
  IPAddress IP = WiFi.softAPIP();
  Serial.begin(115200);
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/led", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("state")) {
      String state = request->getParam("state")->value();
      if (state == "1") {
        digitalWrite(ledPin, HIGH);
        digitalWrite(buzzerPin, HIGH);
        isLedOn = true;
        ledOffTime = millis() + 5000; // Установить время выключения через 5 секунд
        request->send(200, "text/plain", "LED and Buzzer are ON");
      } else {
        digitalWrite(ledPin, LOW);
        digitalWrite(buzzerPin, LOW);
        isLedOn = false;
        ledOffTime = 0;
        request->send(200, "text/plain", "LED and Buzzer are OFF");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
}

void loop() {
  // Проверяем, нужно ли выключить светодиод и пьезоизлучатель
  if (isLedOn && millis() > ledOffTime) {
    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    isLedOn = false;
  }
}
