String getDateTimenow() {
  String year, month, day;
  String hour, minute, second;
  time_t unix;
  if (rtcStatus) {
    DateTime now = rtc.now();
    year = now.year();
    month = now.month();
    if (month.toInt() < 10)
      month = "0" + month;
    day = now.day();
    if (day.toInt() < 10)
      day = "0" + day;
    hour = now.hour();
    if (hour.toInt() < 10)
      hour = "0" + hour;
    minute = now.minute();
    if (minute.toInt() < 10)
      minute = "0" + minute;
    second = now.second();
    if (second.toInt() < 10)
      second = "0" + second;
    unix = now.unixtime();
  } else {
    year = "9999";
    month = "99";
    day = "99";
    hour = "99";
    minute = "99";
    second = "99";
    unix = 0;
  }
  
  sdatenowVal = day + "/" + month;// + "/" + year;
  stimenowVal = hour + ":" + minute;// + ":" + second;
  unsigned long secondss = (unsigned long) unix;
  sunixnowVal = String(secondss);
}
