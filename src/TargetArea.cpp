#include "TargetArea.h"
#include "Logger.h"
#include "stdlib.h"
#include <cmath>
#include <time.h>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

TargetArea::TargetArea(string ID, string Name, vector<MyPoint> Geometry)
{
    m_Name = Name;
    m_ID = ID;
    m_Geometry = Geometry;
    VariablesInit();
    DefaultStyleInit();
}

TargetArea::TargetArea(string ID, string Name)
{
    m_Name = Name;
    m_ID = ID;
    VariablesInit();
    DefaultStyleInit();
}

TargetArea::TargetArea(string ID)
{
    m_Name = m_ID = ID;
    VariablesInit();
    DefaultStyleInit();
}

TargetArea::TargetArea()
{
    m_Name = m_ID = "";
    VariablesInit();
    DefaultStyleInit();
    m_Left = m_Right = m_Top = m_Bottom = 0.0;
}

TargetArea::TargetArea(double left, double right, double top, double bottom)
{
    m_Name = m_ID = "";
    VariablesInit();
    DefaultStyleInit();
    m_Left = left;
    m_Right = right;
    m_Top = top;
    m_Bottom = bottom;
    
    stringstream ss;
    ss << "Creating TargetArea - Boundaries: left=" << left << ", right=" << right 
       << ", top=" << top << ", bottom=" << bottom;
    log_message(LOG_INFO, ss.str());
    
    // Create geometry from the boundaries
    vector<MyPoint> points;
    // Create a rectangle using the boundaries (5 points to close the shape)
    points.push_back(MyPoint(left, bottom, 0)); // Bottom-left
    points.push_back(MyPoint(right, bottom, 0)); // Bottom-right
    points.push_back(MyPoint(right, top, 0)); // Top-right
    points.push_back(MyPoint(left, top, 0)); // Top-left
    points.push_back(MyPoint(left, bottom, 0)); // Back to bottom-left to close the shape
    m_Geometry = points;
    
    stringstream ss2;
    ss2 << "Created geometry with " << m_Geometry.size() << " points";
    log_message(LOG_INFO, ss2.str());
    
    for (int i = 0; i < m_Geometry.size(); i++) {
        stringstream pointInfo;
        pointInfo << "Point " << (i+1) << ": (" << m_Geometry[i].getX() << ", " 
                  << m_Geometry[i].getY() << ", " << m_Geometry[i].getZ() << ")";
        log_message(LOG_DEBUG, pointInfo.str());
    }
}
void TargetArea::VariablesInit()
{
    m_OutLineWidth = 1.0;
    m_Checked = false;
    m_Left = m_Right = m_Top = m_Bottom = 0.0;
}
void TargetArea::DefaultStyleInit()
{
    GetRandomColor(&m_AreaColor, 0, 255, 150);
    GetRandomColor(&m_OutLineColor, 0, 100, 255);
    m_OutLineWidth = 2;
}
void TargetArea::GetRandomColor(Color **pColor, int Min, int Max, int Alpha)
{
    int MinValue = fmax(0, Min);
    int MaxValue = fmin(255, Max);
    unsigned r = (unsigned)GetRandomValue(MinValue, MaxValue);
    unsigned g = (unsigned)GetRandomValue(MinValue, MaxValue);
    unsigned b = (unsigned)GetRandomValue(MinValue, MaxValue);
    *pColor = new Color((unsigned)Alpha, r, g, b);
}

int TargetArea::GetRandomValue(int Min, int Max)
{
    srand((unsigned)time(NULL)); // Initialize random seed
    return (rand() % (Max - Min + 1)) + Min;
}
TargetArea::~TargetArea()
{
    delete m_AreaColor;
    delete m_OutLineColor;
}