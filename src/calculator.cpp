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

        // Convert to absolute time
        time_t ttNow = time(0);
        cJulian jul(ttNow);
        
        // Create DateTime object using current time
        tm* timeinfo = gmtime(&ttNow);
        DateTime curTime(timeinfo->tm_year + 1900, 
                       timeinfo->tm_mon + 1,
                       timeinfo->tm_mday,
                       timeinfo->tm_hour,
                       timeinfo->tm_min,
                       timeinfo->tm_sec);
        
        // Calculate time difference
        mins = (jul.Date() - satorbit.Epoch().Date()) * 1440;
        // Calculate xyz coordinates
        cEciTime eci = satorbit.PositionEci(mins);

        // Convert to latitude and longitude
        cGeoTime geo(eci);
        // Convert longitude to range -180 to 180
        lon = geo.LongitudeDeg();
        lon = lon > 180 ? lon - 360 : lon;
        lat = geo.LatitudeDeg();
        alt = geo.AltitudeKm();

        cVector pos = eci.Position();
        cVector vel = eci.Velocity();

        // Create and return track point
        return TrackPoint(curTime, lon, lat, alt, 
                         pos.m_x, pos.m_y, pos.m_z, 
                         vel.m_x, vel.m_y, vel.m_z);
    }
};

// Emscripten bindings
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

    // Add Sensor class bindings
    emscripten::class_<Sensor>("Sensor")
        .constructor<>()
        .constructor<string,int,string,string,double, double, double>()
        .function("getInitAngle", &Sensor::getInitAngle)
        .function("getSideAngle", &Sensor::getCurSideAngle)
        .function("getObsAngle", &Sensor::getObsAngle)
        .function("getWidth", &Sensor::getWidth)        // Add method to get width
        .function("getSenName", &Sensor::getSenName)    // Add method to get sensor name
        .function("getHexColor", &Sensor::getHexColor)
        .function("setInitAngle", &Sensor::setInitAngle)
        .function("setSideAngle", &Sensor::setCurSideAngle)
        .function("setObsAngle", &Sensor::setObsAngle)
        .function("setHexColor", &Sensor::setHexColor)
        .function("setWidth", &Sensor::setWidth);
    
    // Add Color class bindings
    emscripten::class_<Color>("Color")
        .constructor<>()
        .constructor<int, int, int>()
        .function("getR", &Color::getRed)
        .function("getG", &Color::getGreen)
        .function("getB", &Color::getBlue);
    
    // Add CRegion class bindings
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
    
    // Add TargetArea class bindings
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
    
    // Bind vector<Sensor>
    emscripten::register_vector<Sensor>("VectorSensor");
    
    // Bind vector<CRegion>
    emscripten::register_vector<CRegion>("VectorCRegion");

    // Bind vector<MyPoint>
    emscripten::register_vector<MyPoint>("VectorMyPoint");
    
    // Bind vector<TrackPoint>
    emscripten::register_vector<TrackPoint>("VectorTrackPoint");

    emscripten::class_<Calculator>("Calculator")
        .constructor<>()
        .function("calPath", &Calculator::calPath)
        .function("ComputeTrack", &Calculator::ComputeTrack)
        .function("SensorInRegion", &Calculator::SensorInRegion);
}
