#ifndef TARGETAREA_H_
#define TARGETAREA_H_
#include "MyPoint.h"
#include "Color.h"
#include <string>
#include <vector>
using namespace std;

class TargetArea
{
  private:
    string m_Name, m_ID;
    vector<MyPoint> m_Geometry;
    Color *m_AreaColor;
    Color *m_OutLineColor;
    double m_OutLineWidth;
    bool m_Checked;
    // Add boundary properties for JavaScript
    double m_Left, m_Right, m_Top, m_Bottom;

    void GetRandomColor(Color **pColor, int Min, int Max, int Alpha);
    int GetRandomValue(int Min, int Max);
    void DefaultStyleInit();
    void VariablesInit();

  public:
    // Boundary getters and setters for JavaScript
    double getLeft() const { return m_Left; }
    double getRight() const { return m_Right; }
    double getTop() const { return m_Top; }
    double getBottom() const { return m_Bottom; }
    
    void setLeft(double val) { m_Left = val; }
    void setRight(double val) { m_Right = val; }
    void setTop(double val) { m_Top = val; }
    void setBottom(double val) { m_Bottom = val; }

    bool getChecked() const { return m_Checked; }
    void setChecked(bool checked) { m_Checked = checked; }

    Color *getOutLineColor() const { return m_OutLineColor; }
    void setOutLineColor(Color *c) { m_OutLineColor = c; }

    Color *getAreaColor() const { return m_AreaColor; }
    void setAreaColor(Color *c) { m_AreaColor = c; }

    double getOutLineWidth() const { return m_OutLineWidth; }
    void setOutLineWidth(double c) { m_OutLineWidth = c; }

    string getID() const { return m_ID; }

    string getName() const { return m_Name; }
    void setName(string s) { m_Name = s; }

    vector<MyPoint> getGeometry() const { return m_Geometry; }
    void setGeometry(vector<MyPoint> s) { m_Geometry = s; }

    TargetArea(string ID, string Name, vector<MyPoint> Geometry);
    TargetArea(string ID, string Name);
    TargetArea(string ID);
    TargetArea(); // Default constructor for JavaScript
    TargetArea(double left, double right, double top, double bottom); // Constructor with boundaries for JavaScript
    TargetArea(const TargetArea& other); // Copy constructor (Rule of Three: owns Color* members)
    TargetArea& operator=(const TargetArea& other); // Assignment operator
    ~TargetArea();
};
#endif