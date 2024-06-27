void auto_config_page(){
 if (!validateToken()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    return;
  }
  String html = "<html><head><meta charset=\"UTF-8\"><title>Настройка автоматического режима</title>";
  html += "<link href=\"style.css\" rel=\"stylesheet\" type=\"text/css\" />";
  html += "</head><body>";
  html += "<h2>Настройка автоматического режима</h2>";
  html += "<form id=\"config-form\" >";
  html += "<div id=\"response\">";
  html += "<label for=\"number\">Уровень влажности для включения:</label>";
  html += "<input type=\"number\" id=\"on_hum\" name=\"on_hum\" placeholder=\"Пример: 60\" pattern=\"[0-9]{4,}\" min=\"20\" max=\"100\" value=\""+String(settings.auto_on)+"\" required><br>";
  html += "<label for=\"number\">Уровень влажности для выключения:</label>";
  html += "<input type=\"number\" id=\"off_hum\" name=\"off_hum\" placeholder=\"Пример: 40\" pattern=\"[0-9]{4,}\" min=\"20\" max=\"90\" value=\""+String(settings.auto_off)+"\" required><br><br>";
  html += "<input type=\"submit\" value=\"Изменить\">";
  html += "</div>";
  html += "</form><br>";
  html += "<center><br><a href=\"/\">Вернуться назад</a><br></center>";
  html += "<footer>© <b>CYBEREX TECH</b>, 2024. Версия микро ПО <b>"+version_code+"</b>.</footer></html>";
  html += aconf_js;
  html += "</body></html>";
  server.send(200, "text/html", html);
}
