#ifndef RELATION_OPERATOR_H_
#define RELATION_OPERATOR_H_
#include "MyPoint.h"
#include <vector>
#include <tuple>
using namespace std;

class RelationOperator
{
  public:
    // Point in polygon
    bool PointInPolygon(MyPoint* P, vector<MyPoint> V);
    // Line intersects with polygon
    bool LineInsectArea(MyPoint* P0, MyPoint* P1, vector<MyPoint> V);

    bool LineInsectEnvelope(MyPoint* StartMyPoint, MyPoint* EndMyPoint, double Xmax, double Xmin, double Ymax, double Ymin);

    // Polygon intersects with polygon
    bool AreaInsectArea(vector<MyPoint> Area1, vector<MyPoint> Area2);

    // Get bounding rectangle boundary points
    bool GetEnvelopePoints(vector<MyPoint> Area, double& Xmax, double& Xmin, double& Ymax, double& Ymin);

    // Check if crossing 180 degree meridian
    bool IsOver180Degree(vector<MyPoint> Area);

  private:
    tuple<MyPoint*,MyPoint*> GetEdgePoints(MyPoint* p1, MyPoint* p2);

    void AdjustPointX(vector<MyPoint>* Area);
};

#endif