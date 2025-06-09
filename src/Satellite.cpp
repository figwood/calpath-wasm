#include "Satellite.h"
#include "JulianConvert.h"
#include <string>
#include <vector>
#include <cmath>
#include <ctime>
using namespace std;

const double R = 6378.15;
// YYYY-MM-DD HH24:MI:SS
const string BJTimeFormate = "yyyy-MM-dd HH:mm:ss"; // 北京时间的格式
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
  // 传感器坐标下的Z轴向量
  // （，，）
  // 根据传感器的安装参数和当前侧摆角将Z轴坐标转换成卫星本体坐标系
  // 只考虑侧摆方向的安装角，绕X轴旋转，向右为正
  // 临时修改，因安装角与计算结果正好相反，故加了个负号
  double satz[3] = {0};
  double scale[3] = {0, 0, 1};
  RotateX(scale, -sen->getInitAngle() * M_PI / 180, satz);
  // double[] satz = RotateX(0, 0, 1, 0);
  // 一些角度
  double SenCurAngle = sen->getCurSideAngle() * M_PI / 180;
  // 粗略计算载荷的观测角
  double ObsAngle = sen->getObsAngle() > 0 ? sen->getObsAngle() : atan(sen->getWidth() / (GetSatHeight() * 2));
  double SenHalfAngle = (ObsAngle / 2) * M_PI / 180;
  // SenHalfAngle = 10 * Math.PI / 180;
  // SenCurAngle = 20 * Math.PI / 180;

  // 根据卫星的姿态将卫星本体坐标系转换成轨道坐标系
  // 暂时只考虑侧摆，绕X轴旋转，向右侧摆为负
  // 载荷Z轴的旋转,向右为负
  double satsidez[3] = {0};
  scale[0] = satz[0];
  scale[1] = satz[1];
  scale[2] = satz[2];
  RotateX(scale, -SenCurAngle, satsidez);
  // 将载荷的Z轴向左右旋转，得出观测的左右侧向量
  double sl[3] = {0};
  scale[0] = satsidez[0];
  scale[1] = satsidez[1];
  scale[2] = satsidez[2];

  RotateX(scale, SenHalfAngle, sl);

  double sr[3] = {0};
  RotateX(scale, -SenHalfAngle, sr);

  // 根据卫星的位置和速度将轨道坐标系转换成ECI
  // 计算转换矩阵Reo
  double Reo[3][3] = {0};
  ComputeReo(r, v, Reo);

  // 用Reo将sl[]和sr[]转换至ECI
  double sleci[3] = {0};
  double sreci[3] = {0};
  MulMatrix3p1(Reo, sl, sleci);
  MulMatrix3p1(Reo, sr, sreci);

  // 观测向量与地球求交
  double pleci[3] = {0};
  IntersectSolution(sleci, r, pleci);
  double preci[3] = {0};
  IntersectSolution(sreci, r, preci);

  // 设定返回值
  res[0] = pleci[0];
  res[1] = pleci[1];
  res[2] = pleci[2];
  res[3] = preci[0];
  res[4] = preci[1];
  res[5] = preci[2];
}

void Satellite::IntersectSolution(double v[], double r[], double res[])
{
  // 圆的方程
  // x^2+y^2+z^2=1
  // 直线方程
  // x-rx   y-ry   z-rz
  //---- = ---- = ---- = k
  //  vx     vy     vz
  // 联立之后得如下方程：
  //(vx^2+vy^2+vz^2)k^2 + 2(vx*rx+vy*ry+vz*rz)k + (rx^2+r^2+rz^2-1)=0
  // A=(vx^2+vy^2+vz^2); B=2(vx*rx+vy*ry+vz*rz); C=(rx^2+r^2+rz^2-1)
  double A = v[0] * v[0] + v[1] * v[1] + v[2] * v[2], B = 2 * (v[0] * r[0] + v[1] * r[1] + v[2] * r[2]),
         C = r[0] * r[0] + r[1] * r[1] + r[2] * r[2] - 1;
  double k, delta = B * B - 4 * A * C;
  if (delta < 0)
  {
    cout << "error solving equation" << endl;
    return;
  }
  // 由于要求出球面上距(rx,ry,rz)点较近的点，因此如果B>0，则解取+号，否则取-号
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
  // 计算载荷观测范围的ECI坐标
  double seneci[6] = {0};
  GetSensorPointsECI(sen, r, v, seneci);

  // 分离出左右侧点
  double pleci[3] = {seneci[0], seneci[1], seneci[2]};
  double preci[3] = {seneci[3], seneci[4], seneci[5]};
  // 根据时间将ECI转换成ECR
  double thetaG = TheltaG(&dt);
  // cout<<"thetaG: "<<thetaG<<endl;
  double plecr[3] = {0};
  double precr[3] = {0};
  RotateZ(pleci, -thetaG, plecr);
  RotateZ(preci, -thetaG, precr);

  // ECR转换成BLH
  double left[2] = {0};
  ECRtoBL(plecr, left);
  double right[2] = {0};
  ECRtoBL(precr, right);

  res[0] = left[0];
  res[1] = left[1];
  res[2] = right[0];
  res[3] = right[1];
  // cout << "left: " << left[0] << " " << left[1] << " right: " << right[0] << " " << right[1] << endl;
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
  // ECR转换成BLH
  double lon_left = atan(ecr[1] / ecr[0]) * 180 / M_PI;
  double lat_left = atan(ecr[2] / (sqrt(ecr[0] * ecr[0] + ecr[1] * ecr[1]))) * 180 / M_PI;

  // 经度范围在-180~180之间，而atan的值域为-90~90，因此需要对经度进行调整
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
  // 单位化
  double modr = sqrt(r[0] * r[0] + r[1] * r[1] + r[2] * r[2]);
  double modv = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  double nr[3] = {r[0] / modr, r[1] / modr, r[2] / modr};
  double nv[3] = {v[0] / modv, v[1] / modv, v[2] / modv};

  // 求H
  double h[3] = {0};
  CrossProduct(nr, nv, h);

  // Roe[0]
  double Roe0[3] = {0};
  CrossProduct(h, nr, Roe0);

  double Roe[3][3] = {{Roe0[0], Roe0[1], Roe0[2]},
                      {-h[0], -h[1], -h[2]},
                      {-nr[0], -nr[1], -nr[2]}};
  // 求逆
  MatrixReverse(Roe, res);
}

void Satellite::MatrixReverse(double m[][3], double res[][3])
{
  double mod = MOD3p3(m);
  Yuzishi(m, res);

  if (mod == 0)
  {
    cout << "error computing mod!" << endl;
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

// compute with array, experimental
void Satellite::ComputeTrack(DateTime StartTime, DateTime EndTime, int StepTimeInSec, TrackPoint res[])
{
  double R = 6378.15;

  // 开始计算,准备TLE数据
  if (m_line1.empty())
  {
    m_LastError = "TLE数据读取出错！";
    return;
  }

  cTle sattle(m_line0, m_line1, m_line2);
  cOrbit satorbit(sattle);

  // 准备计算时刻
  DateTime tmpTime = StartTime;

  for (int i = 0; i < 25921; i++)
  {
    double lat, lon, alt, mins;

    // 换算成绝对时间
    cJulian jul(tmpTime.ToTime_t());
    // 计算时间差
    mins = (jul.Date() - satorbit.Epoch().Date()) * 1440;
    // 计算得出xyz坐标
    cEciTime eci = satorbit.PositionEci(mins);

    // 换成经纬度
    cGeoTime geo(eci);
    // 把经度坐标换成-180到180之间
    lon = geo.LongitudeDeg();
    lon = lon > 180 ? lon - 360 : lon;
    lat = geo.LatitudeDeg();
    alt = geo.AltitudeKm();

    TrackPoint tp(tmpTime, lon, lat, alt, eci.Position().m_x / R, eci.Position().m_y / R,
                  eci.Position().m_z / R, eci.Velocity().m_x / R, eci.Velocity().m_y / R,
                  eci.Velocity().m_z / R);

    res[i] = tp;
    // tmpTime加上下一时长
    tmpTime = tmpTime.AddSeconds(StepTimeInSec);
  }
}

// compute with vector
vector<TrackPoint> Satellite::ComputeTrack2(DateTime StartTime, DateTime EndTime, int StepTimeInSec)
{
  double R = 6378.15;

  // 预定义vector大小，避免重分配
  int size = 1 + (EndTime.ToTimestamp() - StartTime.ToTimestamp()) / (StepTimeInSec);
  vector<TrackPoint> TpList;
  TpList.reserve(size); // 提前分配空间

  // 开始计算,准备TLE数据
  if (m_line1.empty())
  {
    m_LastError = "TLE数据读取出错！";
    return TpList;
  }

  cTle sattle(m_line0, m_line1, m_line2);
  cOrbit satorbit(sattle);

  // 准备计算时刻
  DateTime tmpTime = StartTime;

  // 缓存轨道指数计算结果
  double epoch_date = satorbit.Epoch().Date();

  // 预分配变量
  double lat, lon, alt, mins;

  for (int i = 0; i < size; i++)
  {
    // 换算成绝对时间
    cJulian jul(tmpTime.ToTime_t());
    // 计算时间差
    mins = (jul.Date() - epoch_date) * 1440;
    // 计算得出xyz坐标
    cEciTime eci = satorbit.PositionEci(mins);

    // 换成经纬度
    cGeoTime geo(eci);
    // 把经度坐标换成-180到180之间
    lon = geo.LongitudeDeg();
    lon = lon > 180 ? lon - 360 : lon;
    lat = geo.LatitudeDeg();
    alt = geo.AltitudeKm();

    // 构造TrackPoint并直接添加到向量，避免复制
    TpList.emplace_back(tmpTime, lon, lat, alt,
                        eci.Position().m_x / R, eci.Position().m_y / R, eci.Position().m_z / R,
                        eci.Velocity().m_x / R, eci.Velocity().m_y / R, eci.Velocity().m_z / R);

    // tmpTime加上下一时长
    tmpTime = tmpTime.AddSeconds(StepTimeInSec);
  }
  return TpList;
}

vector<CRegion> Satellite::SensorInRegionWithStep(vector<CRegion> InputRegions, int StepTimeInSec,
                                                  TargetArea *area)
{
  // 开始计时
  clock_t step_start_time = clock();

  // 预分配空间，假设输出至少与输入一样多
  vector<CRegion> regionList;
  regionList.reserve(InputRegions.size());

  RelationOperator rop;

  // 缓存目标区域几何形状以避免重复访问
  const vector<MyPoint> &areaGeometry = area->getGeometry();

  // 缓存一个通用的区域点集合，减少内存分配
  vector<MyPoint> regionPoints;
  regionPoints.reserve(5); // 我们知道每个区域有5个点

  for (int i = 0; i < InputRegions.size(); i++)
  {
    const CRegion &inputRegion = InputRegions[i]; // 使用引用避免复制
    // 直接使用嵌入在区域中的传感器，不再需要 GetSensorByName
    const Sensor &sen = inputRegion.getSensor();

    // 检查传感器是否有效
    if (sen.getSenName().empty())
    {
      cout << "Sensor not found or invalid: " << inputRegion.getSenName() << endl;
      continue;
    }

    CRegion region;
    region.setSensor(sen); // 设置传感器

    DateTime startTime = inputRegion.getStartTime();
    DateTime stopTime = inputRegion.getStopTime();

    // 提前计算轨道点以便重用，避免多次计算
    vector<TrackPoint> TrackPoints = ComputeTrack2(startTime, stopTime, StepTimeInSec);

    DateTime lastStopTime = stopTime.AddSeconds(-StepTimeInSec);
    DateTime curTime = startTime;
    int index = 0;

    // 预分配结果数组以减少重分配
    double res1[4] = {0};
    double res2[4] = {0};

    while (curTime <= lastStopTime)
    {
      if (index + 1 >= TrackPoints.size())
      {
        cout << "index out of range" << endl;
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

      // 重用regionPoints，避免每次循环创建新对象
      regionPoints.clear();
      // 先右后左, 逆时针
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
          region.setpGeometry(regionPoints); // 这会复制regionPoints
        }
        else if (region.getStopTime().ToTimestamp() == curTime.ToTimestamp())
        {
          region.setStopTime(new DateTime(curTime.AddSeconds(StepTimeInSec)));
          // merge points
          region.insertPoints(res2[2], res2[3], res2[0], res2[1]);
        }
        else
        {
          // 区域结束，添加到列表
          regionList.push_back(region);
          // create new region
          region = CRegion();
          region.setSensor(sen); // 设置传感器
          // 设置新的开始和结束时间
          region.setStartTime(new DateTime(curTime));
          region.setStopTime(new DateTime(curTime.AddSeconds(StepTimeInSec)));
          region.setpGeometry(regionPoints);
        }
      }
      else if (region.getpGeometry().size() > 0)
      {
        // 添加最后一个区域
        regionList.push_back(region);
        // create new region
        region = CRegion();
        region.setSensor(sen); // 设置传感器
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

  // 压缩vector容量到实际大小
  vector<CRegion>(regionList).swap(regionList);

  // 结束计时
  clock_t step_end_time = clock();
  double step_elapsed_time = (double)(step_end_time - step_start_time) / CLOCKS_PER_SEC;
  cout << "步长 " << StepTimeInSec << " 区域计算完成，处理了 " << InputRegions.size()
       << " 个输入区域，生成了 " << regionList.size() << " 个输出区域，用时: "
       << step_elapsed_time << " 秒" << endl;

  return regionList;
}

vector<CRegion> Satellite::SensorInRegion(vector<Sensor> *SenList, long StartTime, long EndTime, TargetArea *area)
{
  // 开始计时
  clock_t start_time = clock();

  // 预先分配空间以减少重新分配
  vector<CRegion> regionList;
  regionList.reserve(SenList->size());
  cout << "开始计算卫星规划路径..." << endl;

  cout << "开始时间: " << StartTime << " 结束时间: " << EndTime << endl;
  for (const Sensor &sensor : *SenList)
  {
    cout << "传感器: " << sensor.getSenName() << " 安装角: " << sensor.getInitAngle()
         << " 观测角: " << sensor.getObsAngle() << endl;
  }
  // 创建一次DateTime对象并复用
  DateTime *startDateTime = new DateTime(StartTime);
  DateTime *endDateTime = new DateTime(EndTime);

  // for each senname, init regionList
  for (int i = 0; i < SenList->size(); i++)
  {
    const Sensor &sensor = SenList->at(i);
    CRegion region;
    region.setSensor(sensor);           // 设置传感器对象
    region.setStartTime(startDateTime); // 使用共享的DateTime对象
    region.setStopTime(endDateTime);    // 使用共享的DateTime对象
    regionList.push_back(region);
  }

  int steps[3] = {120, 20, 1};
  for (int i = 0; i < 3; i++)
  {
    clock_t step_start = clock();
    cout << "步长 " << steps[i] << " 开始计算..." << endl;

    // 传递引用可能会减少内存使用，但需要确保SensorInRegionWithStep函数不会修改原始regionList
    // 这里保持原样，因为函数设计可能假设值传递
    regionList = SensorInRegionWithStep(regionList, steps[i], area);

    // 内存管理：如果regionList很大，可能需要压缩容量
    if (i < 2)
    {                                               // 最后一次迭代不需要
      vector<CRegion>(regionList).swap(regionList); // 压缩capacity到size
    }

    clock_t step_end = clock();
    double step_time = (double)(step_end - step_start) / CLOCKS_PER_SEC;
    cout << "步长 " << steps[i] << " 计算完成，结果区域数量: " << regionList.size()
         << "，用时: " << step_time << " 秒" << endl;
  }

  // 结束计时
  clock_t end_time = clock();
  double total_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
  cout << "卫星路径规划完成，总用时: " << total_time << " 秒" << endl;

  // 注意：这里没有释放startDateTime和endDateTime，因为它们可能被CRegion对象使用
  // 如果CRegion内部会复制DateTime对象，则应该在这里释放
  // delete startDateTime;
  // delete endDateTime;

  return regionList;
}
