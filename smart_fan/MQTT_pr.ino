// Функция отправки данных по протоколу MQTT
void MQTT_send(){
   client.loop();
   uint32_t nows = millis();
  if (nows - lastMsg > 30000 && settings.mqtt_en) {
    if(WiFi.status() == WL_CONNECTED) {
        MQTT_send_data("jsondata", JSON_DATA());
     }
   lastMsg = nows; 
  }
 }
void MQTT_send_data(String topic, String data){
         String root = settings.mqtt_topic;
         String top  = root +"/"+ topic;
         String subscr = root +"/control";
         String input = settings.mqtt_serv;
         int colonIndex = input.indexOf(':');
         String ipAddress;
         String port;
         IPAddress Remote_MQTT_IP;

        if (colonIndex != -1) {
             ipAddress = input.substring(0, colonIndex);
             port = input.substring(colonIndex + 1);
             WiFi.hostByName(ipAddress.c_str(), Remote_MQTT_IP);
          }
      
      client.setServer(Remote_MQTT_IP, port.toInt());
      client.setCallback(callback);
           if(client.connected()){                                                                                    // Вторичная отправка данных при подключенном брокере 
          count_rf = 0;
          send_mqtt(top, data, subscr);
           }else{
              count_rf++;
              if (client.connect(settings.mqtt_id.c_str(), settings.mqtt_user.c_str(), settings.mqtt_passw.c_str())){  // Первичная отправка данных, выполняестя попытка подключения к брокеру 
                    send_mqtt(top, data, subscr);          
                }else{
                  if(count_rf > 2){                                                                                    // Если были неудачные попытки подключения, то переподключаем Wi-fi
                     WiFi.disconnect();
                     WiFi.begin(settings.mySSID, settings.myPW);
                     count_rf = 0;
                }
            }
        }
     
}

void send_mqtt(String tops, String data, String subscr){
     // Анонсируем датчики и переключатели для Home Assistant [auto-discovery ]
     // Анонсируем элементы один раз при успешном подуключении и при запуске устройства
    if(!annonce_mqtt_discovery){
          String top      = settings.mqtt_topic +"/jsondata";
          String control  = settings.mqtt_topic +"/control";
          //Анонс датчика температуры
          String Playload_temp = "{\"device_class\": \"temperature\", \"name\": \"Smart Fan temp\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"°C\", \"value_template\": \"{{ value_json.temp}}\" }";
          client.publish("homeassistant/sensor/sf_temp/config", Playload_temp.c_str(), true);
          //Анонс датчика влажности
          String Playload_hum = "{\"device_class\": \"humidity\", \"name\": \"Smart Fan hum\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"%\", \"value_template\": \"{{ value_json.hum}}\" }";
          client.publish("homeassistant/sensor/sf_hum/config", Playload_hum.c_str(), true);
          //Анонс уставки влажности верх 
          String Playload_hum_up = "{\"device_class\": \"humidity\", \"name\": \"Smart Fan hum_on\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"%\", \"value_template\": \"{{ value_json.h_on}}\" }";
          client.publish("homeassistant/sensor/sf_hum_on/config", Playload_hum_up.c_str(), true);
          //Анонс уставки влажности низ 
          String Playload_hum_down = "{\"device_class\": \"humidity\", \"name\": \"Smart Fan hum_off\", \"state_topic\": \""+top+"\", \"unit_of_measurement\": \"%\", \"value_template\": \"{{ value_json.h_off}}\" }";
          client.publish("homeassistant/sensor/sf_hum_off/config", Playload_hum_down.c_str(), true);
          //Анонс переключателя On/Off вентилятора 
          String Playload_hum_on_off = "{\"name\": \"F OnOff\", \"command_topic\": \""+control+"\", \"state_topic\": \""+top+"\", \"payload_on\": \"0\", \"payload_off\": \"0\", \"state_on\": \"On\", \"state_off\": \"Off\", \"value_template\": \"{{ value_json.c }}\"}";
          client.publish("homeassistant/switch/sf_on_off/config", Playload_hum_on_off.c_str(), true);
          //Анонс переключателя авторежима вентилятора 
          String Playload_hum_auto = "{\"name\": \"F Auto\", \"command_topic\": \""+control+"\", \"state_topic\": \""+top+"\", \"payload_on\": \"1\", \"payload_off\": \"2\", \"state_on\": \"1\", \"state_off\": \"0\", \"value_template\": \"{{ value_json.a }}\"}";
          client.publish("homeassistant/switch/sfauto/config", Playload_hum_auto.c_str(), true);
          annonce_mqtt_discovery = true;
    }
    // Отправляем данные 
    client.publish(tops.c_str(), data.c_str());
    client.subscribe(subscr.c_str());
}

String MQTT_status(){
    String response_message = "";
    if(client.connected()){
         response_message = "подключен";
      }else{
         response_message = "отключен";
    }
    return response_message;
} 
