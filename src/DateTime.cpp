#include "DateTime.h"
#include <ctime>

using namespace std;

// Portable implementation of timegm for converting UTC tm to time_t
// timegm is not available on all platforms (e.g., WASM/Emscripten)
static time_t portable_timegm(struct tm *tm) {
    time_t ret;
    char *tz;
    
    // Save current timezone
    tz = getenv("TZ");
    
    // Set timezone to UTC
    setenv("TZ", "", 1);
    tzset();
    
    // Convert using mktime (which now interprets as UTC)
    ret = mktime(tm);
    
    // Restore original timezone
    if (tz) {
        setenv("TZ", tz, 1);
    } else {
        unsetenv("TZ");
    }
    tzset();
    
    return ret;
}
DateTime::DateTime()
{
    Init(1970, 1, 1, 0, 0, 0, 0);
}
DateTime::DateTime(long timestamp)
{
    time_t t = static_cast<time_t>(timestamp);
    tm *ptm = gmtime(&t);  // Use gmtime for UTC
    m_year = ptm->tm_year + 1900;
    m_month = ptm->tm_mon + 1;
    m_day = ptm->tm_mday;
    m_hour = ptm->tm_hour;
    m_min = ptm->tm_min;
    m_sec = ptm->tm_sec;
    m_millisec = 0;
    m_timezone = 0;
}
DateTime::DateTime(int year, int month, int day)
{
    Init(year, month, day, 0, 0, 0, 0);
}
DateTime::DateTime(int year, int month, int day, int hour, int min, int sec)
{
    Init(year, month, day, hour, min, sec, 0);
}
DateTime::DateTime(int year, int month, int day, int hour, int min, int sec, int millisec)
{
    Init(year, month, day, hour, min, sec, millisec);
}
void DateTime::Init(int year, int month, int day, int hour, int min, int sec, int millisec)
{
    m_year = year;
    m_month = month;
    m_day = day;
    m_hour = hour;
    m_min = min;
    m_sec = sec;
    m_millisec = millisec;
}
DateTime &DateTime::getNow(bool useLocalTime)
{
    time_t ttNow = time(0);
    tm *ptmNow;

    if (useLocalTime)
    {
        ptmNow = localtime(&ttNow);
    }
    else
    {
        ptmNow = gmtime(&ttNow);
    }
    int Year = ptmNow->tm_year + 1900;
    int Month = ptmNow->tm_mon + 1;
    int Day = ptmNow->tm_mday;

    int Hour = ptmNow->tm_hour;
    int Min = ptmNow->tm_min;
    int Sec = ptmNow->tm_sec;

    static DateTime now(Year, Month, Day, Hour, Min, Sec);
    return now;
}
DateTime &DateTime::getToday(bool useLocalTime)
{
    time_t ttNow = time(0);
    tm *ptmNow;

    if (useLocalTime)
    {
        ptmNow = localtime(&ttNow);
    }
    else
    {
        ptmNow = gmtime(&ttNow);
    }
    int Year = ptmNow->tm_year + 1900;
    int Month = ptmNow->tm_mon + 1;
    int Day = ptmNow->tm_mday;

    int Hour = ptmNow->tm_hour;
    int Min = ptmNow->tm_min;
    int Sec = ptmNow->tm_sec;

    static DateTime now(Year, Month, Day, Hour, Min, Sec);
    return now;
}
DateTime &DateTime::UtcNow()
{
    return getNow(false);
}
DateTime &DateTime::Now()
{
    return getNow(true);
}
DateTime &DateTime::Today()
{
    return getToday(true);
}
DateTime &DateTime::UtcToday()
{
    return getToday(false);
}
string DateTime::toString()
{
    return to_string(m_year) + "-" + to_string(m_month) + "-" + to_string(m_day) +
           " " + to_string(m_hour) + ":" + to_string(m_min) + ":" + to_string(m_sec);
}
DateTime DateTime::ToUniversalTime()
{
    // convert to timestamp
    tm t = ToTm();

    // substract hours*timezone
    t.tm_hour -= m_timezone;
    time_t time_t_after = portable_timegm(&t);  // Use portable_timegm for UTC
    tm tm_after = *gmtime(&time_t_after);  // Use gmtime for UTC

    DateTime dt(tm_after.tm_year + 1900, tm_after.tm_mon + 1,
                tm_after.tm_mday, tm_after.tm_hour, tm_after.tm_min, tm_after.tm_sec, m_millisec);
    return dt;
}

long DateTime::ToTimestamp()
{
    tm Tm = ToTm();
    time_t t = portable_timegm(&Tm);  // Use portable_timegm for UTC
    long stamp = static_cast<long int>(t);
    return stamp;
}

tm DateTime::ToTm()
{
    tm t;
    t.tm_year = m_year - 1900;
    t.tm_mon = m_month - 1;
    t.tm_mday = m_day;
    t.tm_hour = m_hour;
    t.tm_min = m_min;
    t.tm_sec = m_sec;
    t.tm_isdst = 0;
    return t;
}

bool DateTime::operator<=(DateTime &dt)
{
    return ToTimestamp() <= dt.ToTimestamp();
}
time_t DateTime::ToTime_t()
{
    tm Tm = ToTm();
    time_t t = portable_timegm(&Tm);  // Use portable_timegm for UTC
    return t;
}

DateTime DateTime::AddSeconds(int seconds)
{
    tm t = ToTm();
    t.tm_sec += seconds;
    time_t time_t_after = portable_timegm(&t);  // Use portable_timegm for UTC
    tm tm_after = *gmtime(&time_t_after);  // Use gmtime for UTC

    DateTime dt(tm_after.tm_year + 1900, tm_after.tm_mon + 1,
                tm_after.tm_mday, tm_after.tm_hour, tm_after.tm_min, tm_after.tm_sec, m_millisec);
    return dt;
}
DateTime DateTime::AddMins(int mins)
{
    return AddSeconds(mins * 60);
}
DateTime DateTime::AddHours(int hours)
{
    return AddSeconds(hours * 3600);
}
DateTime DateTime::AddDays(int days)
{
    return AddSeconds(days * 24 * 3600);
}