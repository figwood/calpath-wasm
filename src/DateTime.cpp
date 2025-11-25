#include "DateTime.h"

#include <ctime>
#include <iomanip>
#include <sstream>

namespace {

constexpr long long kSecondsPerMinute = 60;
constexpr long long kSecondsPerHour = 60 * kSecondsPerMinute;
constexpr long long kSecondsPerDay = 24 * kSecondsPerHour;

long long days_from_civil(int year, unsigned month, unsigned day)
{
    year -= static_cast<int>(month <= 2);
    const long long era = (year >= 0 ? year : year - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(year - era * 400);
    const unsigned doy = (153 * (month > 2 ? month - 3 : month + 9) + 2) / 5 + day - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + static_cast<long long>(doe) - 719468;
}

void civil_from_days(long long days, int &year, unsigned &month, unsigned &day)
{
    days += 719468;
    const long long era = (days >= 0 ? days : days - 146096) / 146097;
    const unsigned doe = static_cast<unsigned>(days - era * 146097);
    const unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    const long long y = static_cast<long long>(yoe) + era * 400;
    const unsigned doy = doe - (yoe * 365 + yoe / 4 - yoe / 100);
    const unsigned mp = (5 * doy + 2) / 153;
    day = doy - (153 * mp + 2) / 5 + 1;
    month = mp + (mp < 10 ? 3 : -9);
    year = static_cast<int>(y + (month <= 2));
}

long long timestamp_from_components(int year, int month, int day, int hour, int minute, int second)
{
    const long long days = days_from_civil(year, static_cast<unsigned>(month), static_cast<unsigned>(day));
    long long result = days * kSecondsPerDay;
    result += static_cast<long long>(hour) * kSecondsPerHour;
    result += static_cast<long long>(minute) * kSecondsPerMinute;
    result += static_cast<long long>(second);
    return result;
}

void components_from_timestamp(long long timestamp,
                               int &year,
                               int &month,
                               int &day,
                               int &hour,
                               int &minute,
                               int &second)
{
    long long days = timestamp / kSecondsPerDay;
    long long remainder = timestamp % kSecondsPerDay;
    if (remainder < 0)
    {
        remainder += kSecondsPerDay;
        --days;
    }

    unsigned uMonth;
    unsigned uDay;
    civil_from_days(days, year, uMonth, uDay);
    month = static_cast<int>(uMonth);
    day = static_cast<int>(uDay);

    hour = static_cast<int>(remainder / kSecondsPerHour);
    remainder %= kSecondsPerHour;
    minute = static_cast<int>(remainder / kSecondsPerMinute);
    second = static_cast<int>(remainder % kSecondsPerMinute);
}

} // namespace

DateTime::DateTime()
{
    Init(1970, 1, 1, 0, 0, 0, 0);
}

DateTime::DateTime(long timestamp)
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    components_from_timestamp(timestamp, year, month, day, hour, minute, second);
    Init(year, month, day, hour, minute, second, 0);
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

DateTime &DateTime::UtcNow()
{
    static DateTime now;
    const std::time_t current = std::time(nullptr);
    now = DateTime(static_cast<long>(current));
    return now;
}

DateTime &DateTime::UtcToday()
{
    static DateTime today;
    const std::time_t current = std::time(nullptr);
    today = DateTime(static_cast<long>(current));
    return today;
}

std::string DateTime::toString() const
{
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << m_year << '-'
        << std::setw(2) << m_month << '-'
        << std::setw(2) << m_day << ' '
        << std::setw(2) << m_hour << ':'
        << std::setw(2) << m_min << ':'
        << std::setw(2) << m_sec;
    return oss.str();
}

DateTime DateTime::ToUniversalTime() const
{
    return *this;
}

long DateTime::ToTimestamp() const
{
    return static_cast<long>(timestamp_from_components(m_year, m_month, m_day, m_hour, m_min, m_sec));
}

time_t DateTime::ToTime_t() const
{
    return static_cast<time_t>(ToTimestamp());
}

std::tm DateTime::ToTm() const
{
    std::tm t{};
    t.tm_year = m_year - 1900;
    t.tm_mon = m_month - 1;
    t.tm_mday = m_day;
    t.tm_hour = m_hour;
    t.tm_min = m_min;
    t.tm_sec = m_sec;
    t.tm_isdst = 0;
    return t;
}

bool DateTime::operator<=(const DateTime &dt) const
{
    return ToTimestamp() <= dt.ToTimestamp();
}

DateTime DateTime::AddSeconds(int seconds) const
{
    const long long updated = static_cast<long long>(ToTimestamp()) + static_cast<long long>(seconds);
    DateTime result(static_cast<long>(updated));
    result.m_millisec = m_millisec;
    return result;
}