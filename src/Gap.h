#pragma once

#include <Core/Utils.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLineList.h>

using namespace adsk::core;
using namespace adsk::fusion;

class Gap
{
private:
    Ptr<Point3D> m_refPoint;
    Ptr<Vector3D> m_halfWidthVector;
    Ptr<Vector3D> m_thicknessVector;
    double m_wiggleRoom;

    Ptr<Vector3D> m_negHalfWidthVector;
    Ptr<Vector3D> m_wiggleRoomVector;
    Ptr<Vector3D> m_negWiggleRoomVector;

public:
    Gap(const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& m_halfWidthVector, const Ptr<Vector3D>& thicknessVector, double wiggleRoom);
    ~Gap();

    void sketch(const Ptr<Sketch>& sketch);
};
