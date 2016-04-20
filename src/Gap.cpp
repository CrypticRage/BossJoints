#include "Gap.h"

Gap::Gap(const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& widthVector, const Ptr<Vector3D>& thicknessVector)
{
    m_refPoint = refPoint;
    m_widthVector = widthVector;
    m_thicknessVector = thicknessVector;
}

Gap::~Gap()
{
}

// create the profile of all the gaps in the joint
void Gap::sketch(const Ptr<Sketch>& sketch)
{
    Ptr<Point3D> p1 = m_refPoint;
    Ptr<Point3D> p2 = m_refPoint->copy();
    
    p2->translateBy(m_widthVector);
    p2->translateBy(m_thicknessVector);

    sketch->sketchCurves()->sketchLines()->addTwoPointRectangle(p1, p2);
}
