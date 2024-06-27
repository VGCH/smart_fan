void zero_time_fq(){

  int Htime=pulseIn(zero_sensor, HIGH);    // прочитать время высокого логического уровня
  int Ltime=pulseIn(zero_sensor, LOW);     // прочитать время низкого логического уровня
  int Ttime = Htime+Ltime;
  frequency=1000000/Ttime;  // получение частоты из Ttime в микросекундах
  
}
