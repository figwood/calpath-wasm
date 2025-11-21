#ifndef SENSOR_H_
#define SENSOR_H_
#include "Color.h"
#include <string>
using namespace std;
class Sensor
{
private:
  string m_SatId, m_SenName, m_SatName;
  int m_SenId; // Sensor ID
  string m_CountryName;
  double m_Width;
  double m_ObsAngle; // Observation angle
  double m_CurSideAngle; // Current side swing angle
  double m_Resolution;
  double m_RightSideAngle; // Right side swing angle
  double m_LeftSideAngle;  // Left side swing angle
  double m_InitAngle;      // Sensor installation angle
  string m_HexColor; // Hexadecimal color value
  void Init();

public:
  Sensor(string satId, string sensorName);
  Sensor();
  Sensor(string satId,int senId,string satName,string sensorName, double initAngle, double sideAngle, double obsAngle); // Three-parameter constructor
  ~Sensor();

  double getInitAngle() const { return m_InitAngle; }
  double getSideAngle() const { return m_CurSideAngle; }
  
  // Setter methods
  void setInitAngle(double value) { m_InitAngle = value; }
  void setSideAngle(double value) { m_CurSideAngle = value; }

  string getHexColor() const { return m_HexColor; }
  void setHexColor(string value) { m_HexColor = value; }
  
  string getSatId() { return m_SatId; }

  double getResolution() const { return m_Resolution; }
  void setResolution(double value) { m_Resolution = value; }

  double getObsAngle() const { return m_ObsAngle; }
  void setObsAngle(double value) { m_ObsAngle = value; }

  double getLeftSideAngle() const { return m_LeftSideAngle; }
  void setLeftSideAngle(double value) { m_LeftSideAngle = value; }

  double getRightSideAngle()const  { return m_RightSideAngle; }
  void setRightSideAngle(double value) { m_RightSideAngle = value; }

  string getSenName() const { return m_SenName; }
  void setSenName(string value) { m_SenName = value; }

  string getSatName() const { return m_SatName; }
  void setSatName(string value) { m_SatName = value; }

  string getSatId() const { return m_SatId; }
  void setSatId(string value) { m_SatId = value; }

  int getSenId() const { return m_SenId; }
  void setSenId(int value) { m_SenId = value; }

  double getWidth() const { return m_Width; }
  void setWidth(double value) { m_Width = value; }
  
  // Backward compatibility - for methods like GetSensorPointsECI
  double getCurSideAngle() const { return m_CurSideAngle; }
  void setCurSideAngle(double value) { m_CurSideAngle = value; }
};
#endif