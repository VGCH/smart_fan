void indata(){
     if (captivePortal()) { 
    return;
  }
  if (!validateToken()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
    if (server.hasArg("switch")){  
    String msg;   
      if(server.arg("switch")=="on"){
        settings.mqtt_en = true;
        save_config();
        msg +="<text>Передача данных включена!</text>";
         String input = settings.mqtt_serv;
         int colonIndex = input.indexOf(':');
         String ipAddress;
         String port;

        if (colonIndex != -1) {
             ipAddress = input.substring(0, colonIndex);
             port = input.substring(colonIndex + 1);
          }
        client.setServer(ipAddress.c_str(), port.toInt());
        }else if(server.arg("switch")=="off"){
        settings.mqtt_en = false;
        save_config();
        msg +="<text>Передача данных выключена!</text>";
    } 
    server.send(200, "text/html", msg);
  }
   if (server.hasArg("switch_auto")){  
    String msg;   
      if(server.arg("switch_auto")=="on"){
        settings.auto_en = true;
        save_config();
        msg +="<text>Передача данных включена!</text>";
        }else if(server.arg("switch_auto")=="off"){
        settings.auto_en = false;
        save_config();
        msg +="<text>Передача данных выключена!</text>";
    } 
    server.send(200, "text/html", msg);
    if(settings.mqtt_en){ MQTT_send_data("jsondata", JSON_DATA());}
  }

 

 if(server.hasArg("IPMQTT") && server.hasArg("PASSWORD")){  
    strncpy(settings.mqtt_serv, server.arg("IPMQTT").c_str(), MAX_STRING_LENGTH);
    settings.mqtt_user     = String(server.arg("USERNAME"));                        // Логин mqtt
    settings.mqtt_passw    = String(server.arg("PASSWORD"));                        // Пароль mqtt
    settings.mqtt_id       = String(server.arg("ID"));                              // Идентификатор mqtt 
    settings.mqtt_topic    = String(server.arg("topicname"));                       // Корень mqtt 
    settings.mqtt_time     = server.arg("times").toInt();                           // Переодичность обмена mqtt .toInt();
    save_config();
    
    String data = "";
    data += "<text> IP адрес сервера MQTT: <b>"+String(settings.mqtt_serv)+"</b></text><br>";
    data += "<text> Логин: <b>"+settings.mqtt_user+"</b></text><br>";
    data += "<text> Идентификатор устройства: <b>"+settings.mqtt_id+"</b></text><br>";
    data += "<text> Имя корня топика: <b>"+settings.mqtt_topic+"</b></text><br>";
    //data += "<text> Периодичность обмена(с): <b>"+String(settings.mqtt_time)+"</b></text><br>";
    data += "<text><b>Данные успешно сохранены!</b></text>";
    server.send(200, "text/html", data);
  }
  
   if(server.hasArg("NEWPASSWORD")){  
       if(server.arg("NEWPASSWORD") != ""){
           settings.passwd = String(server.arg("NEWPASSWORD")); 
           save_config();
    }
   
    String data = "";
    data += "<text><b>Новый пароль успешно сохранён!</b></text>";
    server.send(200, "text/html", data);
  }
     if(server.hasArg("on_hum") && server.hasArg("off_hum")){  
       if(server.arg("on_hum") != "" && server.arg("off_hum") != "" ){
           settings.auto_off = server.arg("off_hum").toFloat(); 
           settings.auto_on  = server.arg("on_hum").toFloat();
           save_config();
    }
   
    String data = "";
    data += "<text><b>Данные автоматизации успешно сохранены!</b></text>";
    server.send(200, "text/html", data);
  }
 /* if (server.hasArg("mode")){     
     String moder = server.arg("mode");
     settings.counter_coeff = moder.toInt();
     fan_level = moder.toInt();
     level = map (fan_level, 0, 100, 1, set_delay);
     save_config();
     String data  = "<text><b>Данные успешно сохранены!</b></text><br>";
     
     server.send(200, "text/html", data);
  }*/
}
