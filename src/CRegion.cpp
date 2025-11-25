#include "CRegion.h"
#include "DateTime.h"
#include "Color.h"
CRegion::CRegion()
{
    Init();
}
CRegion::~CRegion()
{
    // delete m_StartTime;
    // delete m_StopTime;
    // delete m_RegionColor;
}
void CRegion::Init()
{
    m_StartTime = &DateTime::UtcToday();
    m_StopTime = &DateTime::UtcToday();
    m_pGeometry = vector<MyPoint>(0);
    // Initialize Sensor using default constructor
    m_Sensor = Sensor("", "");
}

void CRegion::insertPoints(double lon1, double lat1, double lon2, double lat2)
{
    MyPoint p1(lon1, lat1, 0);
    MyPoint p2(lon2, lat2, 0);
    if (m_pGeometry.size() == 0)
    {
    return;
    }else{
        int index=(m_pGeometry.size()-1)/2;
        m_pGeometry.insert(m_pGeometry.begin() + index, p1);
        m_pGeometry.insert(m_pGeometry.begin() + index + 1, p2);
    }
}