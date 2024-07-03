 String millis2time(){ // преобразование миллисекунд в вид д/ч/м/с
  
         int times =(millis()/1000);
         int days =  (times/3600)/24;
         int timehour =(((times)  % 86400L) / 3600); // часы
        if ( ((times % 3600) / 60) < 10 ) {
         int timehour = 0;
               }
         int timeminuts=((times  % 3600) / 60); // минуты
         if ( (times % 60) < 10 ) {
         int timeminuts = 0;
             }
         int timeseconds=(times % 60); // секунды
       String Time= String(days)+":"+String(twoDigits(timehour))+":"+String(twoDigits(timeminuts))+":"+String(twoDigits(timeseconds));
       return Time;
     }

 String twoDigits(int digits){
             if(digits < 10) {
          String i = '0'+String(digits);
          return i;
         }
          else {
        return String(digits);
         }
      }

void time_work(){
   if (captivePortal()) {  
    return;
  }
  String header;
  if (validateToken()){
   String outjson  ="{";
          outjson += "\"time\":\""+millis2time()+"\",";
          outjson += "\"chanel1\":";
  if(fan_on){
          outjson  += "\"On\",";
     }else{
          outjson  += "\"Off\",";
           }
          outjson += "\"temp\":";
          outjson += "\""+String(temp)+"\",";
          outjson += "\"hum\":";
          outjson += "\""+String(hum)+"\",";
          outjson += "\"level1\":";
          outjson += "\""+String(fan_on)+"\",";
          outjson += "\"water\":";
          outjson += "\""+water_l()+"\",";
          outjson += "\"MQTT\":\""+MQTT_status()+"\"";
          outjson += "}";
     server.send(200, "text", outjson);   
  }   
}

String water_l(){
  return String(frequency);
}

String JSON_DATA(){
     String outjson  = "{";
            outjson += "\"c\":";
     if(fan_on){
            outjson  += "\"On\",";
        }else{
            outjson  += "\"Off\",";
           }
          outjson += "\"temp\":";
          outjson += "\""+String(temp)+"\",";
          outjson += "\"hum\":";
          outjson += "\""+String(hum)+"\",";
          outjson += "\"a\":";
          outjson += "\""+String(settings.auto_en)+"\",";
          outjson += "\"h_on\":";
          outjson += "\""+String(settings.auto_on)+"\",";
          outjson += "\"h_off\":";
          outjson += "\""+String(settings.auto_off)+"\",";
          outjson += "\"fan_level\":";
          outjson += "\""+String(frequency)+"\"";
          outjson += "}";  
     return outjson;
}
