#ifndef DATETIME_H_
#define DATETIME_H_
#include <ctime>
#include <string>
class DateTime
{
private:
  int m_year;
  int m_month;
  int m_day;
  int m_hour;
  int m_min;
  int m_sec;
  int m_millisec;

  void Init(int year, int month, int day, int hour, int min, int sec, int millisec);

  std::tm ToTm() const;

public:
  static DateTime &UtcNow();
  static DateTime &UtcToday();
  int getYear() const { return m_year; }
  int getMonth() const { return m_month; }
  int getDay() const { return m_day; }
  int getHour() const { return m_hour; }
  int getMinute() const { return m_min; }
  int getSecond() const { return m_sec; }
  int getMillisec() const { return m_millisec; }
  DateTime ToUniversalTime() const;
  long ToTimestamp() const;
  time_t ToTime_t() const;
  std::string toString() const;
  DateTime AddSeconds(int seconds) const;

  DateTime();
  explicit DateTime(long timestamp);
  DateTime(int year, int month, int day);
  DateTime(int year, int month, int day, int hour, int min, int sec);
  DateTime(int year, int month, int day, int hour, int min, int sec, int millisec);

  bool operator<=(const DateTime &dt) const;
};
#endif