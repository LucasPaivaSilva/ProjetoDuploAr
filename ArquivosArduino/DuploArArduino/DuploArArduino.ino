const char* WIFI_SSID = "NomeDaRede";
const char* WIFI_PASSWORD = "SenhaDaRede";
const char* TELEGRAM_BOT_TOLKEN = "5156871866:AAH6-PqBXnS02jU8QYhif8mp1z-zy_25x9I";

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// Initialize Telegram BOT
#define BOTtoken TELEGRAM_BOT_TOLKEN  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "XXXXXXXXXX"
#define IgnoreChat true // set to true if you want to ignore the chat ID

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = 2;
bool ledState = LOW;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if ((chat_id != CHAT_ID)&&(!IgnoreChat)) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Olá!, " + from_name + ".\n";
      welcome += "Use os comandos a seguir para controlar o bot.\n\n";
      welcome += "/led_on para ligar o LED de Debug \n";
      welcome += "/led_off para desligar o LED de Debug \n";
      welcome += "/led_state para retornar o status do LED de Debug\n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED de Debug Ligado", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED de Debug Desligado", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/led_state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED esta ligado", "");
      }
      else{
        bot.sendMessage(chat_id, "LED esta desligado", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
  Serial.println("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
