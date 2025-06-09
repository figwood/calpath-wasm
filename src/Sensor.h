#ifndef SENSOR_H_
#define SENSOR_H_
#include "Color.h"
#include <string>
using namespace std;
class Sensor
{
private:
  string m_SatId, m_SenName, m_SatName;
  int m_SenId; // 传感器ID
  string m_CountryName;
  double m_Width;
  double m_ObsAngle; //观测角
  double m_CurSideAngle; //当前的侧摆角
  double m_Resolution;
  double m_RightSideAngle; //向右侧摆的角度
  double m_LeftSideAngle;  //向左侧摆的角度
  double m_InitAngle;      //载荷安装角
  string m_HexColor; //十六进制颜色值
  void Init();

public:
  Sensor(string satId, string sensorName);
  Sensor();
  Sensor(string satId,int senId,string satName,string sensorName, double initAngle, double sideAngle, double obsAngle); // 三参数构造函数
  ~Sensor();

  double getInitAngle() const { return m_InitAngle; }
  double getSideAngle() const { return m_CurSideAngle; }
  
  // 设置方法
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
  
  // 向后兼容 - 用于 GetSensorPointsECI 等方法
  double getCurSideAngle() const { return m_CurSideAngle; }
  void setCurSideAngle(double value) { m_CurSideAngle = value; }
};
#endif