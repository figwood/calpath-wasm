#include "Sensor.h"
#include "Color.h"
Sensor::Sensor(string satId, string sensorName)
{
    Init();
    m_SatId = satId;
    m_SenName = sensorName;
}
Sensor::Sensor()
{
    Init();
}

Sensor::Sensor(string satId,int senId,string satName,string sensorName,double initAngle, double sideAngle, double obsAngle)
{
    Init();
    m_SatId = satId; // Satellite ID
    m_SatName = satName; // Satellite name
    m_SenId = senId; // Sensor ID
    m_InitAngle = initAngle;    // Sensor installation angle
    m_CurSideAngle = sideAngle; // Side swing angle
    m_ObsAngle = obsAngle;      // Observation angle
    m_SenName = sensorName;
}

Sensor::~Sensor()
{
}

void Sensor::Init()
{
    m_SatId = "";
    m_SenId = -1; // Sensor ID
    m_SenName = "";
    m_SatName = "";
    m_CountryName = "";
    m_Width = -1;
    m_ObsAngle = -1; // Observation angle
    m_CurSideAngle = 0; // Current side swing angle
    m_Resolution = 0;
    m_RightSideAngle = 0; // Right side swing angle
    m_LeftSideAngle = 0;  // Left side swing angle
    m_InitAngle = 0;      // Sensor installation angle
}