#ifndef SATELLITE_H_
#define SATELLITE_H_
#include "Color.h"
#include "TargetArea.h"
#include "CRegion.h"
#include "RelationOperator.h"
#include "DateTime.h"
#include "TrackPoint.h"
#include "MyPoint.h"
#include "coreLib.h"
#include "orbitLib.h"
#include <vector>
#include <tuple>

using namespace std;
enum PathMode
{
  Day,
  Night,
  Both
};
class Satellite
{
protected:
  string m_SatName;

  Color m_DisplayColor;
  int m_StepInSec; //正常步长,20s

  RelationOperator m_RelOpera;

  string m_NoardID;
  string m_LastError = "";

  virtual double GetHeight() { return 600.0; }
  void Init();

private:
  string m_line0;
  string m_line1;
  string m_line2;
  double m_SatHeight;

  double GetSatHeight() { return 600.0; }
  double TheltaG(DateTime *dt);

  void MulMatrix3p1(double mx[][3],double my[], double res[]);
  void RotateX(double coord[], double AngleRad, double res[]);
  void RotateY(double coord[], double AngleRad, double res[]);
  void RotateZ(double coord[], double AngleRad, double res[]);
  void ECRtoBL(double ecr[], double res[]);
  void GetSensorPointsECI(const Sensor *sen, double r[], double v[],double res[]);
  void IntersectSolution(double v[], double r[],double res[]);
  void CrossProduct(double v1[], double v2[],double res[]);
  void Yuzishi(double array[][3],double res[][3]);
  double MOD3p3(double m[][3]);
  void ComputeReo(double r[], double v[],double res[][3]);
  void MatrixReverse(double m[][3],double res[][3]);
  cEci GetSatPosEci(DateTime Dt, string line0, string line1, string line2);
  vector<CRegion> SensorInRegionWithStep(vector<CRegion> InputRegions, int StepTimeInSec, TargetArea *area);

public:
  Satellite(string SatId, string SatName);
  ~Satellite();

  void SetCurTLE(string line0, string line1, string line2)
  {
    m_line0 = line0;
    m_line1 = line1;
    m_line2 = line2;
  }
  string getLastError() { return m_LastError; }

  string getNoardID() { return m_NoardID; }

  string getSatName() { return m_SatName; }
  void setSatName(string value) { m_SatName = value; }

  Color getDisplayColor() { return m_DisplayColor; }
  void setDisplayColor(Color value) { m_DisplayColor = value; }

  void GetSensorPointsBLH(const Sensor *sen,DateTime dt, double r[], double v[],double res[]);

  MyPoint GetSatPosGeo(DateTime Dt);
  MyPoint GetSatPosEci(DateTime Dt);
  vector<TrackPoint> ComputeTrack(DateTime StartTime, DateTime EndTime, int StepTimeInSec);
  vector<CRegion> SensorInRegion(vector<Sensor> *SenList,long StartTime, long EndTime, TargetArea *area);
};
#endif
