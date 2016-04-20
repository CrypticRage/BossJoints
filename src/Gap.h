#pragma once

#include <Core/Utils.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>

using namespace adsk::core;
using namespace adsk::fusion;

class Gap
{
private:
    Ptr<Point3D> m_refPoint;
    Ptr<Vector3D> m_widthVector;
    Ptr<Vector3D> m_thicknessVector;

public:
    Gap(const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& widthVector, const Ptr<Vector3D>& thicknessVector);
    ~Gap();

    void sketch(const Ptr<Sketch>& sketch);
};
