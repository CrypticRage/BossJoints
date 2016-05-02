#pragma once

#include <Core/Utils.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>

using namespace adsk::core;
using namespace adsk::fusion;

class FilletBase
{
private:
    double m_diameter;
    Ptr<Point3D> m_centerPoint;

public:
    FilletBase(
        const double diameter, const Ptr<Point3D>& centerPoint
    );
    FilletBase();
    ~FilletBase();

    double diameter() const;
    void setDiameter(const double diameter);

    void setCenterPoint(const Ptr<Point3D>& centerPoint);
    Ptr<Point3D> centerPoint() const;

    void drawSketch(const Ptr<Sketch>& sketch);
};

inline void FilletBase::setCenterPoint(const Ptr<Point3D>& centerPoint)
{
    m_centerPoint = centerPoint->copy();
}

inline Ptr<Point3D> FilletBase::centerPoint() const
{
    return m_centerPoint->copy();
}

inline double FilletBase::diameter() const
{
    return m_diameter;
}

inline void FilletBase::setDiameter(const double diameter)
{
    m_diameter = diameter;
}
