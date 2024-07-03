/* Устройство для управления вытяжным вентилятором
 *  © CYBEREX Tech, 2024
 * 
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <ESP8266SSDP.h>
#include <PubSubClient.h>
#include "DHTesp.h"
#include <AM2302-Sensor.h>
#include <Wire.h>
#include "const_js.h"



#define on_off_fan    14                    // Пин управления питанием
#define DHT22_pin     13                    // Пин датчика температуры и влажности
#define DHT22_vin     5                     // Пин подачи питания на датчик
#define zero_sensor   12                    // Датчик нуля 
#define on_off_ind    4                     // Индикатор включения вентилятора
#define buzer         16                    // Пищалка
DHTesp dht;

// WEBs
ESP8266WebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);
IPAddress apIP(10, 10, 20, 1);
IPAddress netMsk(255, 255, 255, 0);
ESP8266HTTPUpdateServer httpUpdater;

// DNS server
const byte             DNS_PORT = 53;
DNSServer              dnsServer;

// Текущий статус WLAN
unsigned int status = WL_IDLE_STATUS;

// Переменные для хранения статуса сетевого подключения
boolean connect;
boolean wi_fi_st;

boolean stat_reboot, led_stat, stat_wifi, fan_on, stat, annonce_mqtt_discovery; 

// Переменные используемые для CaptivePortal
const char *myHostname  = "Cyberex_SmartFAN";            // Имя создаваемого хоста Cyberex_SmartFAN.local 
const char *softAP_ssid = "CYBEREX-SmartFAN";            // Имя создаваемой точки доступа Wi-Fi

String version_code = "CF-1.2.07.24";

float hum = 0.0;
float temp = 0.0;
float auto_on_hum;
//timers
uint32_t low_t, med_t, prevMills, lastMsg, impOnDelay, reboot_t, lastConnectTry, auto_check_time;
float frequency = 0;
//int fq_count;

int count_rf = 0;
//int level;

// Структура для хранения токенов сессий 
struct Token {
    String value;
    long created;
    long lifetime;
};

Token   tokens[20];                    // Длина массива структур хранения токенов 

#define TOKEN_LIFETIME 6000000         // время жизни токена в секундах

#define MAX_STRING_LENGTH 30           // Максимальное количество символов для хранения в конфигурации


// Структура для хранения конфигурации устройства
struct {
     boolean mqtt_en;
     int     mqtt_time;
     int     statteeprom; 
     int     counter_coeff;
     float   total;
     char    mySSID[MAX_STRING_LENGTH];
     char    myPW[MAX_STRING_LENGTH];
     char    mqtt_serv[MAX_STRING_LENGTH];
     String  mqtt_user;
     String  mqtt_passw;
     String  mqtt_id;
     String  mqtt_topic;
     String  passwd; 
     float   auto_off; 
     float   auto_on; 
     boolean auto_en;  
  } settings;

  int fan_level = 0;
  int set_delay = 5000;

void setup() {
    EEPROM.begin(sizeof(settings));                                 // Инициализация EEPROM в соответствии с размером структуры конфигурации      
    pinMode(on_off_fan, OUTPUT);
    pinMode(on_off_ind, OUTPUT);
    pinMode(buzer,      OUTPUT);
    pinMode(DHT22_vin,  OUTPUT);
    digitalWrite(DHT22_vin, HIGH);
    beep_buz(millis(), true);
    dht.setup(DHT22_pin, DHTesp::DHT22);
    read_config();                                                  // Чтение конфигурации из EEPROM
    check_clean();                                                  // Провека на запуск устройства после первичной прошивки
     if(String(settings.passwd) == ""){   
        settings.passwd = "admin";                                  // Если  переменная settings.passwd пуста, то назначаем пароль по умолчанию
     }
     //if(settings.counter_coeff > 0 && settings.counter_coeff <= 100 ){
     //     fan_level = settings.counter_coeff;
     // }else{
     //     fan_level = 0;
    //  }
     //level = map (fan_level, 0, 100, 1, set_delay);
     
     
     WiFi.mode(WIFI_STA);                                           // Выбираем режим клиента для сетевого подключения по Wi-Fi                
     WiFi.hostname("Smart FAN [CYBEREX TECH]");                    // Указываеем имя хоста, который будет отображаться в Wi-Fi роутере, при подключении устройства
     if(WiFi.status() != WL_CONNECTED) {                            // Инициализируем подключение, если устройство ещё не подключено к сети
           WiFi.begin(settings.mySSID, settings.myPW);
      }

     for(int x = 0; x < 160; x ++){                                  // Цикл ожидания подключения к сети (80 сек)
          if (WiFi.status() == WL_CONNECTED){ 
                stat_wifi = true;
                break;
           }
               delay(500); 
      
          }

     if(WiFi.status() != WL_CONNECTED) {                             // Если подключение не удалось, то запускаем режим точки доступа Wi-Fi для конфигурации устройства
            WiFi.mode(WIFI_AP_STA);
            WiFi.softAPConfig(apIP, apIP, netMsk);
            WiFi.softAP(softAP_ssid);
            stat_wifi = false;
        }
        
        delay(2000);
        // Запускаем DNS сервер
        dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
        dnsServer.start(DNS_PORT, "*", apIP);
        // WEB страницы
        server.on("/", page_process);
        server.on("/login", handleLogin);
        server.on("/script.js", reboot_devsend);
        server.on("/style.css", css);
        server.on("/index.html", HTTP_GET, [](){
        server.send(200, "text/plain", "IoT Radiation Sensor"); });
        server.on("/description.xml", HTTP_GET, [](){SSDP.schema(server.client());});
        server.on("/inline", []() {
        server.send(200, "text/plain", "this works without need of authentification");
        });
        server.onNotFound(handleNotFound);
        // Здесь список заголовков для записи
        const char * headerkeys[] = {"User-Agent", "Cookie"} ;
        size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
        // Запускаем на сервере отслеживание заголовков 
        server.collectHeaders(headerkeys, headerkeyssize);
        server.begin();
        connect = strlen(settings.mySSID) > 0;                                            // Статус подключения к Wi-Fi сети, если есть имя сети
        SSDP_init();
        attachInterrupt(digitalPinToInterrupt(zero_sensor), FQ_counter, FALLING);        // Задействуем аппаратное прерывание для измерения импульсов нулевого датчика
              
}

void loop() {
        portals();
        dnsServer.processNextRequest();
        server.handleClient();  
        reboot_dev_delay();
        get_DHT22_data();
        MQTT_send(); 
        auto_process();
}

void reboot_devsend(){
   server.send(200, "text", reb_d);
}
