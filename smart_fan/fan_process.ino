
IRAM_ATTR void FQ_counter() {                       // Обработка прерывания датчика нуля

  if(!stat && fan_on){
       digitalWrite(on_off_fan, HIGH);
       stat = true;
  }
  if(stat && !fan_on){
       digitalWrite(on_off_fan, LOW);
       stat = false;
  }  

}

void get_DHT22_data(){
      if(millis() - prevMills > 5000 ) {
          hum  = dht.getHumidity();
          temp = dht.getTemperature();
          if(String(hum) != "nan"){
             prevMills = millis();
          }else{
            if(millis() - prevMills >= 7000) { digitalWrite(DHT22_vin, LOW); }
            if(millis() - prevMills >= 9000) { 
               digitalWrite(DHT22_vin, HIGH);
               prevMills = millis();
              }
          }
      }
}


void powerS(){
  String response_message ="";
  if(fan_on){
      digitalWrite(on_off_ind, LOW);
      response_message += "<center><a href=\"#\" class=\"btn_off\" onclick=\"on_off()\">Включить вентилятор</a></center>";
      fan_on = false;
   }else{
      //digitalWrite(on_off_fan, HIGH);
      digitalWrite(on_off_ind, HIGH);
      response_message  += "<center><a href=\"#\" class=\"btn_on\" onclick=\"on_off()\">Выключить вентилятор</a></center>";
      fan_on = true;
  }
      
    // server.send(200, "text/html", response_message + "0" +radio_b());
     server.send(200, "text/html", response_message + "0 ytn" );
     if(settings.mqtt_en){ MQTT_send_data("jsondata", JSON_DATA());}
     beep_buz(millis(), true);
}

void auto_process(){                                                     // Автоматический режим управления вентилятором
  if (millis() - auto_check_time >= 1000 && settings.auto_en ) {
     if(settings.auto_on != settings.auto_off){
        if(settings.auto_on <= hum && !fan_on){ powerS(); }
        if(settings.auto_off >= hum && fan_on){ powerS(); }
      }
   auto_check_time = millis();
  }
     
}

/*void drive_procees(){ 
      if(fan_on && !digitalRead(zero_sensor)){           
          digitalWrite(on_off_fan, HIGH);
     }
      
          if (!fan_on && !digitalRead(zero_sensor)){
             digitalWrite(on_off_fan, LOW);
         }
         
 }*/

boolean beep_buz(uint32_t beeb_start, boolean beep){
           while(beep){
            if(millis() - beeb_start >= 10){digitalWrite(buzer, HIGH);}
            if(millis() - beeb_start >= 100){digitalWrite(buzer, LOW);}
            if(millis() - beeb_start >= 300){digitalWrite(buzer, HIGH);}
            if(millis() - beeb_start >= 100){digitalWrite(buzer, LOW); beep = false;}
            
           // if(millis() - beeb_start >= 550){digitalWrite(buzer, HIGH);}
           // if(millis() - beeb_start >= 750){digitalWrite(buzer, LOW);}
            //if(millis() - beeb_start >= 850){digitalWrite(buzer, HIGH);}
            //if(millis() - beeb_start >= 1050){digitalWrite(buzer, LOW); beep = false; }
               }
               return beep;
            }
                


void callback(char* topic, byte* payload, unsigned int length) {
           String message;
         for (int i = 0; i < length; i++) {
             message = message + (char)payload[i];
          }
          if(message != "On" || message != "Off"){
             int data = message.toInt();
             if(data == 0){
                 powerS();
             }
             if(data == 1){
                 settings.auto_en = true;
                 save_config();
                 if(settings.mqtt_en){ MQTT_send_data("jsondata", JSON_DATA());}
             }
             if(data == 2){
                 settings.auto_en = false;
                 save_config();
                 if(settings.mqtt_en){ MQTT_send_data("jsondata", JSON_DATA());}
             }
             
           }
 }
