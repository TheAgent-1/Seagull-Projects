#include "Arduino.h"
#include "WString.h"
class ExtraData {
public:
  float Speed;
  double Lat;
  double Long;

  String GPS_Year;
  String GPS_Month;
  String GPS_Day;

  String GPS_Hour;
  String GPS_Minute;
  String GPS_Second;

public:
  String Date_string() {
    return String(GPS_Day) + ":" + String(GPS_Month) + ":" + String(GPS_Year);
  }

  String Time_string_HMS_edited(int TZ) {
    // Convert UTC time to your local timezone (for example, UTC+5)
    int localHour = (GPS_Hour.toInt() + TZ) % 24;  // Replace 5 with your UTC offset
    if (localHour < 0) localHour += 24;            // Handle negative values
    String amPm = (localHour < 12) ? "AM" : "PM";

    if (localHour > 12) {
      localHour -= 12;
    } else if (localHour == 0) {
      localHour = 12;
    }

    // Convert hour, minute, and second to strings and add leading zeros if necessary
    String hourStr = (localHour < 10) ? "0" + String(localHour) : String(localHour);
    String minuteStr = (GPS_Minute.toInt() < 10) ? "0" + String(GPS_Minute) : String(GPS_Minute);
    String secondStr = (GPS_Second.toInt() < 10) ? "0" + String(GPS_Second) : String(GPS_Second);

    return hourStr + ":" + minuteStr + ":" + secondStr;
  }

  String DateTimeString() {
    return Date_string() + "-" + Time_string_HMS_edited(12);
  }

  String SDData() {
    return DateTimeString() + ',' + String(Lat, 8) + ',' + String(Long, 8) + ',' + String(Speed, 2);
  }

  
};