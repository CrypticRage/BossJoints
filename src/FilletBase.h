#pragma once

#include <Core/Utils.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/Matrix3D.h>
#include <Core/Geometry/Plane.h>
#include <Core/Geometry/SurfaceEvaluator.h>

#include <Fusion/BRep/BRepFace.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchPoints.h>

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
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

        bool onSurface(const Ptr<BRepFace>& face, const Ptr<Sketch>& sketch);
        void drawSketch(const Ptr<Sketch>& sketch);
    };
}

inline void BossJoints::FilletBase::setCenterPoint(const Ptr<Point3D>& centerPoint)
{
    m_centerPoint = centerPoint->copy();
}

inline Ptr<Point3D> BossJoints::FilletBase::centerPoint() const
{
    return m_centerPoint->copy();
}

inline double BossJoints::FilletBase::diameter() const
{
    return m_diameter;
}

inline void BossJoints::FilletBase::setDiameter(const double diameter)
{
    m_diameter = diameter;
}
