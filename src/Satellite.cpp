#include "Satellite.h"
#include "Logger.h"
#include "JulianConvert.h"
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;

const double R = 6378.15;
Satellite::Satellite(string SatId, string SatName)
{
  m_SatName = SatName;
  m_NoardID = SatId;
}
Satellite::~Satellite()
{
}
void Satellite::Init()
{
}

double Satellite::TheltaG(DateTime *dt)
{
  JulianConvert jul;
  return jul.ToGMST(dt);
}

void Satellite::GetSensorPointsECI(const Sensor *sen, double r[], double v[], double res[])
{
  // Z-axis vector in sensor coordinate system
  // (,,)
  // Convert Z-axis coordinates to satellite body coordinate system based on sensor installation parameters and current side swing angle
  // Only consider installation angle in side swing direction, rotate around X-axis, positive to the right
  // Temporary modification: added negative sign because installation angle is opposite to calculation result
  double satz[3] = {0};
  double scale[3] = {0, 0, 1};
  RotateX(scale, -sen->getInitAngle() * M_PI / 180, satz);
  // double[] satz = RotateX(0, 0, 1, 0);
  // Some angles
  double SenCurAngle = sen->getCurSideAngle() * M_PI / 180;
  // Roughly calculate the sensor's observation angle
  double ObsAngle = sen->getObsAngle() > 0 ? sen->getObsAngle() : atan(sen->getWidth() / (GetSatHeight() * 2));
  double SenHalfAngle = (ObsAngle / 2) * M_PI / 180;
  // SenHalfAngle = 10 * Math.PI / 180;
  // SenCurAngle = 20 * Math.PI / 180;

  // Convert satellite body coordinate system to orbital coordinate system based on satellite attitude
  // Currently only consider side swing, rotate around X-axis, negative when swinging to the right
  // Rotation of sensor Z-axis, negative to the right
  double satsidez[3] = {0};
  scale[0] = satz[0];
  scale[1] = satz[1];
  scale[2] = satz[2];
  RotateX(scale, -SenCurAngle, satsidez);
  // Rotate the sensor's Z-axis left and right to obtain left and right observation vectors
  double sl[3] = {0};
  scale[0] = satsidez[0];
  scale[1] = satsidez[1];
  scale[2] = satsidez[2];

  RotateX(scale, SenHalfAngle, sl);

  double sr[3] = {0};
  RotateX(scale, -SenHalfAngle, sr);

  // Convert orbital coordinate system to ECI based on satellite position and velocity
  // Calculate transformation matrix Reo
  double Reo[3][3] = {0};
  ComputeReo(r, v, Reo);

  // Use Reo to convert sl[] and sr[] to ECI
  double sleci[3] = {0};
  double sreci[3] = {0};
  MulMatrix3p1(Reo, sl, sleci);
  MulMatrix3p1(Reo, sr, sreci);

  // Intersect observation vectors with Earth
  double pleci[3] = {0};
  IntersectSolution(sleci, r, pleci);
  double preci[3] = {0};
  IntersectSolution(sreci, r, preci);

  // Set return values
  res[0] = pleci[0];
  res[1] = pleci[1];
  res[2] = pleci[2];
  res[3] = preci[0];
  res[4] = preci[1];
  res[5] = preci[2];
}

void Satellite::IntersectSolution(double v[], double r[], double res[])
{
  // Sphere equation
  // x^2+y^2+z^2=1
  // Line equation
  // x-rx   y-ry   z-rz
  //---- = ---- = ---- = k
  //  vx     vy     vz
  // Combining these equations gives:
  //(vx^2+vy^2+vz^2)k^2 + 2(vx*rx+vy*ry+vz*rz)k + (rx^2+r^2+rz^2-1)=0
  // A=(vx^2+vy^2+vz^2); B=2(vx*rx+vy*ry+vz*rz); C=(rx^2+r^2+rz^2-1)
  double A = v[0] * v[0] + v[1] * v[1] + v[2] * v[2], B = 2 * (v[0] * r[0] + v[1] * r[1] + v[2] * r[2]),
         C = r[0] * r[0] + r[1] * r[1] + r[2] * r[2] - 1;
  double k, delta = B * B - 4 * A * C;
  if (delta < 0)
  {
    log_message(LOG_ERROR, "Error solving equation: delta < 0");
    return;
  }
  // To find the point on the sphere closer to (rx,ry,rz), use + if B>0, otherwise use -
  if (B > 0)
  {
    k = (-B + sqrt(delta)) / (2 * A);
  }
  else
  {
    k = (-B - sqrt(delta)) / (2 * A);
  }

  res[0] = r[0] + k * v[0];
  res[1] = r[1] + k * v[1];
  res[2] = r[2] + k * v[2];
}

void Satellite::GetSensorPointsBLH(const Sensor *sen, DateTime dt, double r[], double v[], double res[])
{
  // Calculate ECI coordinates of sensor observation range
  double seneci[6] = {0};
  GetSensorPointsECI(sen, r, v, seneci);

  // Separate left and right points
  double pleci[3] = {seneci[0], seneci[1], seneci[2]};
  double preci[3] = {seneci[3], seneci[4], seneci[5]};
  // Convert ECI to ECR based on time
  double thetaG = TheltaG(&dt);
  double plecr[3] = {0};
  double precr[3] = {0};
  RotateZ(pleci, -thetaG, plecr);
  RotateZ(preci, -thetaG, precr);

  // Convert ECR to BLH
  double left[2] = {0};
  ECRtoBL(plecr, left);
  double right[2] = {0};
  ECRtoBL(precr, right);

  res[0] = left[0];
  res[1] = left[1];
  res[2] = right[0];
  res[3] = right[1];
}

void Satellite::MulMatrix3p1(double mx[][3], double my[], double res[])
{
  res[0] = mx[0][0] * my[0] + mx[0][1] * my[1] + mx[0][2] * my[2];
  res[1] = mx[1][0] * my[0] + mx[1][1] * my[1] + mx[1][2] * my[2];
  res[2] = mx[2][0] * my[0] + mx[2][1] * my[1] + mx[2][2] * my[2];
}

void Satellite::RotateX(double coord[], double AngleRad, double res[])
{
  double mRx[3][3] = {{1, 0, 0},
                      {0, cos(AngleRad), -sin(AngleRad)},
                      {0, sin(AngleRad), cos(AngleRad)}};
  return MulMatrix3p1(mRx, coord, res);
}
void Satellite::RotateY(double coord[], double AngleRad, double res[])
{
  double mRy[3][3] = {{cos(AngleRad), 0, sin(AngleRad)},
                      {0, 1, 0},
                      {-sin(AngleRad), 0, cos(AngleRad)}};
  return MulMatrix3p1(mRy, coord, res);
}
void Satellite::RotateZ(double coord[], double AngleRad, double res[])
{
  double mRz[3][3] = {{cos(AngleRad), -sin(AngleRad), 0},
                      {sin(AngleRad), cos(AngleRad), 0},
                      {0, 0, 1}};
  return MulMatrix3p1(mRz, coord, res);
}

void Satellite::ECRtoBL(double ecr[], double res[])
{
  // Convert ECR to BLH
  double lon_left = atan(ecr[1] / ecr[0]) * 180 / M_PI;
  double lat_left = atan(ecr[2] / (sqrt(ecr[0] * ecr[0] + ecr[1] * ecr[1]))) * 180 / M_PI;

  // Longitude range is -180~180, while atan range is -90~90, so longitude needs to be adjusted
  if (ecr[0] < 0)
  {
    lon_left = ecr[1] < 0 ? lon_left - 180 : lon_left + 180;
  }

  res[0] = lon_left;
  res[1] = lat_left;
}

void Satellite::CrossProduct(double v1[], double v2[], double res[])
{
  double i, j, k;
  i = v1[1] * v2[2] - v1[2] * v2[1];
  j = v1[2] * v2[0] - v1[0] * v2[2];
  k = v1[0] * v2[1] - v1[1] * v2[0];

  res[0] = i;
  res[1] = j;
  res[2] = k;
}

void Satellite::Yuzishi(double array[][3], double res[][3])
{
  double y00, y01, y02, y10, y11, y12, y20, y21, y22;
  // 00
  res[0][0] = array[1][1] * array[2][2] - array[1][2] * array[2][1];
  // 01 -
  res[1][0] = -(array[1][0] * array[2][2] - array[1][2] * array[2][0]);
  // 02
  res[2][0] = array[1][0] * array[2][1] - array[2][0] * array[1][1];
  // 10 -
  res[0][1] = -(array[0][1] * array[2][2] - array[0][2] * array[2][1]);
  // 11
  res[1][1] = array[0][0] * array[2][2] - array[0][2] * array[2][0];
  // 12 -
  res[2][1] = -(array[0][0] * array[2][1] - array[0][1] * array[2][0]);
  // 20
  res[0][2] = array[0][1] * array[1][2] - array[0][2] * array[1][1];
  // 21 -
  res[1][2] = -(array[0][0] * array[1][2] - array[0][2] * array[1][0]);
  // 22
  res[2][2] = array[0][0] * array[1][1] - array[0][1] * array[1][0];
}

double Satellite::MOD3p3(double m[][3])
{
  return m[0][0] * m[1][1] * m[2][2] +
         m[1][0] * m[2][1] * m[0][2] +
         m[2][0] * m[1][2] * m[0][1] -
         m[0][2] * m[1][1] * m[2][0] -
         m[1][2] * m[2][1] * m[0][0] -
         m[2][2] * m[1][0] * m[0][1];
}

void Satellite::ComputeReo(double r[], double v[], double res[][3])
{
  // Normalize
  double modr = sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);
  double modv = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  double nr[3] = {r[0] / modr, r[1] / modr, r[2] / modr};
  double nv[3] = {v[0] / modv, v[1] / modv, v[2] / modv};

  // Calculate H
  double h[3] = {0};
  CrossProduct(nr, nv, h);

  // Roe[0]
  double Roe0[3] = {0};
  CrossProduct(h, nr, Roe0);

  double Roe[3][3] = {{Roe0[0], Roe0[1], Roe0[2]},
                      {-h[0], -h[1], -h[2]},
                      {-nr[0], -nr[1], -nr[2]}};
  // Calculate inverse
  MatrixReverse(Roe, res);
}

void Satellite::MatrixReverse(double m[][3], double res[][3])
{
  double mod = MOD3p3(m);
  Yuzishi(m, res);

  if (mod == 0)
  {
    log_message(LOG_ERROR, "Error computing matrix determinant: mod = 0");
    return;
  }

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      res[i][j] /= mod;
    }
  }
}

// compute with vector
vector<TrackPoint> Satellite::ComputeTrack(DateTime StartTime, DateTime EndTime, int StepTimeInSec)
{
  double R = 6378.15;

  // Predefine vector size to avoid reallocation
  int size = 1 + (EndTime.ToTimestamp() - StartTime.ToTimestamp()) / (StepTimeInSec);
  vector<TrackPoint> TpList;
  TpList.reserve(size); // Allocate space in advance

  // Start calculation, prepare TLE data
  if (m_line1.empty())
  {
    m_LastError = "TLE data read error!";
    log_message(LOG_ERROR, "TLE data read error - line1 is empty");
    return TpList;
  }

  cTle sattle(m_line0, m_line1, m_line2);
  cOrbit satorbit(sattle);

  // Prepare calculation time
  DateTime tmpTime = StartTime;

  // Cache orbital epoch calculation result
  double epoch_date = satorbit.Epoch().Date();

  // Preallocate variables
  double lat, lon, alt, mins;

  for (int i = 0; i < size; i++)
  {
    // Convert to absolute time
    cJulian jul(tmpTime.ToTime_t());
    // Calculate time difference
    mins = (jul.Date() - epoch_date) * 1440;
    // Calculate xyz coordinates
    cEciTime eci = satorbit.PositionEci(mins);

    // Convert to latitude and longitude
    cGeoTime geo(eci);
    // Convert longitude to range -180 to 180
    lon = geo.LongitudeDeg();
    lon = lon > 180 ? lon - 360 : lon;
    lat = geo.LatitudeDeg();
    alt = geo.AltitudeKm();

    // Construct TrackPoint and add directly to vector to avoid copying
    TpList.emplace_back(tmpTime, lon, lat, alt,
                        eci.Position().m_x / R, eci.Position().m_y / R, eci.Position().m_z / R,
                        eci.Velocity().m_x / R, eci.Velocity().m_y / R, eci.Velocity().m_z / R);

    // Add next time interval to tmpTime
    tmpTime = tmpTime.AddSeconds(StepTimeInSec);
  }
  return TpList;
}

vector<CRegion> Satellite::SensorInRegionWithStep(vector<CRegion> InputRegions, int StepTimeInSec,
                                                  TargetArea *area)
{
  // Start timing
  clock_t step_start_time = clock();

  // Preallocate space, assuming output is at least as large as input
  vector<CRegion> regionList;
  regionList.reserve(InputRegions.size());

  RelationOperator rop;

  // Cache target area geometry to avoid repeated access
  const vector<MyPoint> &areaGeometry = area->getGeometry();

  // Cache a common region point set to reduce memory allocation
  vector<MyPoint> regionPoints;
  regionPoints.reserve(5); // We know each region has 5 points

  for (int i = 0; i < InputRegions.size(); i++)
  {
    const CRegion &inputRegion = InputRegions[i]; // Use reference to avoid copying
    // Directly use sensor embedded in region, no longer need GetSensorByName
    const Sensor &sen = inputRegion.getSensor();

    // Check if sensor is valid
    if (sen.getSenName().empty())
    {
      stringstream ss;
      ss << "Sensor not found or invalid: " << inputRegion.getSenName();
      log_message(LOG_WARN, ss.str());
      continue;
    }

    CRegion region;
    region.setSensor(sen); // Set sensor

    DateTime startTime = inputRegion.getStartTime();
    DateTime stopTime = inputRegion.getStopTime();

    // Calculate track points in advance for reuse, avoid multiple calculations
    vector<TrackPoint> TrackPoints = ComputeTrack(startTime, stopTime, StepTimeInSec);

    DateTime lastStopTime = stopTime.AddSeconds(-StepTimeInSec);
    DateTime curTime = startTime;
    int index = 0;

    // Preallocate result arrays to reduce reallocation
    double res1[4] = {0};
    double res2[4] = {0};

    while (curTime <= lastStopTime)
    {
      if (index + 1 >= TrackPoints.size())
      {
        log_message(LOG_ERROR, "Index out of range in track points");
        break;
      }

      const TrackPoint &tp1 = TrackPoints[index];
      double r1[] = {tp1.getEciPoint().getX(), tp1.getEciPoint().getY(), tp1.getEciPoint().getZ()};
      double v1[] = {tp1.getVel().getX(), tp1.getVel().getY(), tp1.getVel().getZ()};

      const TrackPoint &tp2 = TrackPoints[index + 1];
      double r2[] = {tp2.getEciPoint().getX(), tp2.getEciPoint().getY(), tp2.getEciPoint().getZ()};
      double v2[] = {tp2.getVel().getX(), tp2.getVel().getY(), tp2.getVel().getZ()};

      GetSensorPointsBLH(&sen, curTime, r1, v1, res1);
      GetSensorPointsBLH(&sen, curTime.AddSeconds(StepTimeInSec), r2, v2, res2);

      // Reuse regionPoints to avoid creating new objects each loop iteration
      regionPoints.clear();
      // Right first then left, counterclockwise
      regionPoints.push_back(MyPoint(res1[2], res1[3], 0));
      regionPoints.push_back(MyPoint(res2[2], res2[3], 0));
      regionPoints.push_back(MyPoint(res2[0], res2[1], 0));
      regionPoints.push_back(MyPoint(res1[0], res1[1], 0));
      regionPoints.push_back(MyPoint(res1[2], res1[3], 0));

      if (rop.AreaInsectArea(regionPoints, areaGeometry))
      {
        if (region.getpGeometry().size() == 0)
        {
          region.setStartTime(new DateTime(curTime));
          region.setStopTime(new DateTime(curTime.AddSeconds(StepTimeInSec)));
          region.setpGeometry(regionPoints); // This will copy regionPoints
        }
        else if (region.getStopTime().ToTimestamp() == curTime.ToTimestamp())
        {
          region.setStopTime(new DateTime(curTime.AddSeconds(StepTimeInSec)));
          // merge points
          region.insertPoints(res2[2], res2[3], res2[0], res2[1]);
        }
        else
        {
          // Region ends, add to list
          regionList.push_back(region);
          // create new region
          region = CRegion();
          region.setSensor(sen); // Set sensor
          // Set new start and end time
          region.setStartTime(new DateTime(curTime));
          region.setStopTime(new DateTime(curTime.AddSeconds(StepTimeInSec)));
          region.setpGeometry(regionPoints);
        }
      }
      else if (region.getpGeometry().size() > 0)
      {
        // Add last region
        regionList.push_back(region);
        // create new region
        region = CRegion();
        region.setSensor(sen); // Set sensor
      }

      curTime = curTime.AddSeconds(StepTimeInSec);
      index++;
    }

    if (region.getpGeometry().size() > 0)
    {
      // create new region
      regionList.push_back(region);
    }
  }

  // Compress vector capacity to actual size
  vector<CRegion>(regionList).swap(regionList);

  // End timing
  clock_t step_end_time = clock();
  double step_elapsed_time = (double)(step_end_time - step_start_time) / CLOCKS_PER_SEC;
  
  stringstream ss;
  ss << fixed << setprecision(3)
     << "Step " << StepTimeInSec << "s calculation completed. "
     << "Processed " << InputRegions.size() << " input regions, "
     << "generated " << regionList.size() << " output regions. "
     << "Time elapsed: " << step_elapsed_time << "s";
  log_message(LOG_INFO, ss.str());

  return regionList;
}

vector<CRegion> Satellite::SensorInRegion(vector<Sensor> *SenList, long StartTime, long EndTime, TargetArea *area)
{
  // Start timing
  clock_t start_time = clock();

  // Pre-allocate space to reduce reallocation
  vector<CRegion> regionList;
  regionList.reserve(SenList->size());
  
  log_message(LOG_INFO, "Starting satellite path planning calculation...");

  stringstream ss;
  ss << "Start time: " << StartTime << ", End time: " << EndTime;
  log_message(LOG_INFO, ss.str());
  
  for (const Sensor &sensor : *SenList)
  {
    stringstream sensorInfo;
    sensorInfo << "Sensor: " << sensor.getSenName() 
               << ", Installation angle: " << sensor.getInitAngle()
               << sensor.getObsAngle();
    log_message(LOG_INFO, sensorInfo.str());
  }
  // Create DateTime objects once and reuse
  DateTime *startDateTime = new DateTime(StartTime);
  DateTime *endDateTime = new DateTime(EndTime);

  // for each senname, init regionList
  for (int i = 0; i < SenList->size(); i++)
  {
    const Sensor &sensor = SenList->at(i);
    CRegion region;
    region.setSensor(sensor);           // Set sensor object
    region.setStartTime(startDateTime); // Use shared DateTime object
    region.setStopTime(endDateTime);    // Use shared DateTime object
    regionList.push_back(region);
  }

  int steps[3] = {120, 20, 1};
  for (int i = 0; i < 3; i++)
  {
    clock_t step_start = clock();
    
    stringstream stepStart;
    stepStart << "Starting calculation with step " << steps[i] << "s...";
    log_message(LOG_INFO, stepStart.str());

    // Passing reference might reduce memory usage, but ensure SensorInRegionWithStep doesn't modify original regionList
    // Keep as is, since function design may assume value passing
    regionList = SensorInRegionWithStep(regionList, steps[i], area);

    // Memory management: if regionList is large, may need to compress capacity
    if (i < 2)
    {                                               // Last iteration doesn't need this
      vector<CRegion>(regionList).swap(regionList); // Compress capacity to size
    }

    clock_t step_end = clock();
    double step_time = (double)(step_end - step_start) / CLOCKS_PER_SEC;
    
    stringstream stepEnd;
    stepEnd << fixed << setprecision(3)
            << "Step " << steps[i] << "s completed. "
            << "Result regions: " << regionList.size() 
            << ", Time elapsed: " << step_time << "s";
    log_message(LOG_INFO, stepEnd.str());
  }

  // End timing
  clock_t end_time = clock();
  double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  
  stringstream summary;
  summary << fixed << setprecision(3)
          << "Satellite path planning completed. Total time: " << total_time << "s";
  log_message(LOG_INFO, summary.str());

  // Note: Not releasing startDateTime and endDateTime here, as they may be used by CRegion objects
  // If CRegion internally copies DateTime objects, they should be released here
  // delete startDateTime;
  // delete endDateTime;

  return regionList;
}
