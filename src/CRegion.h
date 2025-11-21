#ifndef CREGION_H_
#define CREGION_H_
#include "MyPoint.h"
#include "DateTime.h"
#include "Color.h"
#include "Sensor.h"
#include <vector>
#include <string>
using namespace std;
class CRegion
{
  private:
    DateTime *m_StartTime, *m_StopTime;
    vector<MyPoint> m_pGeometry;
    Sensor m_Sensor; // 添加 Sensor 成员
    void Init();

  public:
    CRegion();
    ~CRegion();

    vector<MyPoint> getpGeometry() const { return m_pGeometry; }
    void setpGeometry(vector<MyPoint> s) { m_pGeometry = s; }

    string getSatName() const { return m_Sensor.getSatName(); }
    string getSenName() const { return m_Sensor.getSenName(); }
    DateTime getStartTime() const { return *m_StartTime; }
    long getStartTimestamp() const { return m_StartTime->ToTimestamp(); }
    void setStartTime(DateTime *c) { m_StartTime = c; }

    DateTime getStopTime() const { return *m_StopTime; }
    long getStopTimestamp() const { return m_StopTime->ToTimestamp(); }

    void setStopTime(DateTime *c) { m_StopTime = c; }

    double getWidth() const { return m_Sensor.getWidth(); }
    double getResolution() const { return m_Sensor.getResolution(); }
    double getSideAngle() const { return m_Sensor.getCurSideAngle(); }
    string getHexColor() const { return m_Sensor.getHexColor(); }
    const Sensor& getSensor() const { return m_Sensor; }
    Sensor& getSensor() { return m_Sensor; }
    void setSensor(const Sensor& sensor) { m_Sensor = sensor; }
    
    string getSatId() const { return m_Sensor.getSatId(); }
    int getSenId() const { return m_Sensor.getSenId(); }

    void insertPoints(double lon1, double lat1,double lon2, double lat2);
};

#endif