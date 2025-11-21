#include <emscripten/bind.h>
#include "Satellite.h"
#include "JulianConvert.h"
#include "TrackPoint.h"
#include "CRegion.h"
#include "TargetArea.h"
#include <string>
#include <vector>
#include <cmath>

using namespace Zeptomoby::OrbitTools;

class Calculator
{
public:
    vector<TrackPoint> ComputeTrack(string SatID, string SatName, string line1, string line2,
        long StartTime, long EndTime,int StepTimeInSec){
        Satellite sat(SatID, SatName);
        sat.SetCurTLE("", line1, line2);
        return sat.ComputeTrack(StartTime, EndTime,StepTimeInSec);
    }
    vector<CRegion> SensorInRegion(string SatID, string SatName, string line1, string line2,
        vector<Sensor> SenList, long StartTime, long EndTime, TargetArea area)
    {
        Satellite sat(SatID, SatName);
        sat.SetCurTLE("", line1, line2);
        return sat.SensorInRegion(&SenList, StartTime, EndTime, &area);
    } 

    TrackPoint calPath(string line1, string line2)
    {
        string line0="";
        cTle sattle(line0, line1, line2);
        cOrbit satorbit(sattle);
        double lat, lon, alt, mins;

        // 换算成绝对时间
        time_t ttNow = time(0);
        cJulian jul(ttNow);
        
        // 使用当前时间创建 DateTime 对象
        tm* timeinfo = gmtime(&ttNow);
        DateTime curTime(timeinfo->tm_year + 1900, 
                       timeinfo->tm_mon + 1,
                       timeinfo->tm_mday,
                       timeinfo->tm_hour,
                       timeinfo->tm_min,
                       timeinfo->tm_sec);
        
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

        cVector pos = eci.Position();
        cVector vel = eci.Velocity();

        // 创建并返回轨道点
        return TrackPoint(curTime, lon, lat, alt, 
                         pos.m_x, pos.m_y, pos.m_z, 
                         vel.m_x, vel.m_y, vel.m_z);
    }
};

// 使用 Emscripten 绑定
EMSCRIPTEN_BINDINGS(calculator_module)
{
    emscripten::class_<MyPoint>("MyPoint")
        .constructor<>()
        .function("getX", &MyPoint::getX)
        .function("getY", &MyPoint::getY)
        .function("getZ", &MyPoint::getZ)
        .function("setX", &MyPoint::setX)
        .function("setY", &MyPoint::setY)
        .function("setZ", &MyPoint::setZ);

    emscripten::class_<DateTime>("DateTime")
        .constructor<int, int, int>()
        .constructor<int, int, int, int, int, int>()
        .function("getYear", &DateTime::getYear)
        .function("getMonth", &DateTime::getMonth)
        .function("getDay", &DateTime::getDay)
        .function("getHour", &DateTime::getHour)
        .function("getMinute", &DateTime::getMinute)
        .function("getSecond", &DateTime::getSecond)
        .function("toString", &DateTime::toString);

    emscripten::class_<TrackPoint>("TrackPoint")
        .constructor<DateTime, double, double, double>()
        .constructor<DateTime, double, double, double, double, double, double, double, double, double>()
        .function("getTime", &TrackPoint::getTime)
        .function("setTime", &TrackPoint::setTime)
        .function("getEciPoint", &TrackPoint::getEciPoint)
        .function("setEciPoint", &TrackPoint::setEciPoint)
        .function("getBlhPoint", &TrackPoint::getBlhPoint)
        .function("setBlhPoint", &TrackPoint::setBlhPoint)
        .function("getVel", &TrackPoint::getVel)
        .function("setVel", &TrackPoint::setVel);

    // 添加 Sensor 类绑定
    emscripten::class_<Sensor>("Sensor")
        .constructor<>()
        .constructor<string,int,string,string,double, double, double>()
        .function("getInitAngle", &Sensor::getInitAngle)
        .function("getSideAngle", &Sensor::getCurSideAngle)
        .function("getObsAngle", &Sensor::getObsAngle)
        .function("getWidth", &Sensor::getWidth)        // 添加获取宽度的方法
        .function("getSenName", &Sensor::getSenName)    // 添加获取传感器名称的方法
        .function("getHexColor", &Sensor::getHexColor)
        .function("setInitAngle", &Sensor::setInitAngle)
        .function("setSideAngle", &Sensor::setCurSideAngle)
        .function("setObsAngle", &Sensor::setObsAngle)
        .function("setHexColor", &Sensor::setHexColor)
        .function("setWidth", &Sensor::setWidth);
    
    // 添加 Color 类绑定
    emscripten::class_<Color>("Color")
        .constructor<>()
        .constructor<int, int, int>()
        .function("getR", &Color::getRed)
        .function("getG", &Color::getGreen)
        .function("getB", &Color::getBlue);
    
    // 添加 CRegion 类绑定
    emscripten::class_<CRegion>("CRegion")
        .constructor<>()
        .function("getStartTimestamp", &CRegion::getStartTimestamp)
        .function("getStopTimestamp", &CRegion::getStopTimestamp)
        .function("getSenName", &CRegion::getSenName)
        .function("getSenId", &CRegion::getSenId)
        .function("getSatId", &CRegion::getSatId)
        .function("getSatName", &CRegion::getSatName)
        .function("getHexColor", &CRegion::getHexColor)
        .function("getpGeometry", &CRegion::getpGeometry);
    
    // 添加 TargetArea 类绑定
    emscripten::class_<TargetArea>("TargetArea")
        .constructor<>()
        .constructor<double, double, double, double>()
        .function("getLeft", &TargetArea::getLeft)
        .function("getRight", &TargetArea::getRight)
        .function("getTop", &TargetArea::getTop)
        .function("getBottom", &TargetArea::getBottom)
        .function("setLeft", &TargetArea::setLeft)
        .function("setRight", &TargetArea::setRight)
        .function("setTop", &TargetArea::setTop)
        .function("setBottom", &TargetArea::setBottom);
    
    // 绑定 vector<Sensor>
    emscripten::register_vector<Sensor>("VectorSensor");
    
    // 绑定 vector<CRegion>
    emscripten::register_vector<CRegion>("VectorCRegion");

    // 绑定 vector<MyPoint>
    emscripten::register_vector<MyPoint>("VectorMyPoint");
    
    // 绑定 vector<TrackPoint>
    emscripten::register_vector<TrackPoint>("VectorTrackPoint");

    emscripten::class_<Calculator>("Calculator")
        .constructor<>()
        .function("calPath", &Calculator::calPath)
        .function("ComputeTrack", &Calculator::ComputeTrack)
        .function("SensorInRegion", &Calculator::SensorInRegion);
}
