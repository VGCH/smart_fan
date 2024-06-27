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
       
     // client.setServer(ipAddress.c_str(), port.toInt());
      client.setServer(Remote_MQTT_IP, port.toInt());
      client.setCallback(callback);
           if(client.connected()){
          count_rf = 0;
                 
                 client.publish(top.c_str(), data.c_str());
                 client.subscribe(subscr.c_str());  
                 //sendMQTTTemperatureDiscoveryMsg(top);
           }else{
              count_rf++;
              if (client.connect(settings.mqtt_id.c_str(), settings.mqtt_user.c_str(), settings.mqtt_passw.c_str())){           
                    client.publish(top.c_str(), data.c_str());
                    client.subscribe(subscr.c_str());
                    //sendMQTTTemperatureDiscoveryMsg(top);
                }else{
                  if(count_rf > 2){
                     WiFi.disconnect();
                     WiFi.begin(settings.mySSID, settings.myPW);
                     count_rf = 0;
                }
            }
        }
     
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
