#pragma once

#include <Core/Utils.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/BoundingBox3D.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLineList.h>

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
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

        Ptr<BoundingBox3D> m_boundingBox;

        void expandBoundingBox();

    public:
        Gap(const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& halfWidthVector, const Ptr<Vector3D>& thicknessVector, double wiggleRoom);
        Gap();
        ~Gap();

        void sketch(const Ptr<Sketch>& sketch);
        Ptr<BoundingBox3D> boundingBox();

        void setRefPoint(const Ptr<Point3D>& refPoint);
        void setHalfWidthVector(const Ptr<Vector3D>& halfWidthVector);
        void setThicknessVector(const Ptr<Vector3D>& thicknessVector);
        void setWiggleRoom(double wiggleRoom);
    };
}

inline void BossJoints::Gap::setRefPoint(const Ptr<Point3D>& refPoint)
{
    m_refPoint = refPoint->copy();
    m_boundingBox = BoundingBox3D::create(m_refPoint, m_refPoint);
}

inline void BossJoints::Gap::setHalfWidthVector(const Ptr<Vector3D>& halfWidthVector)
{
    m_halfWidthVector = halfWidthVector->copy();

    m_negHalfWidthVector = m_halfWidthVector->copy();
    m_negHalfWidthVector->scaleBy(-1.0);

    if (m_wiggleRoom <= 0.0)
        return;

    m_wiggleRoomVector = m_halfWidthVector->copy();
    m_wiggleRoomVector->normalize();
    m_wiggleRoomVector->scaleBy(m_wiggleRoom / 2);

    m_negWiggleRoomVector = m_wiggleRoomVector->copy();
    m_negWiggleRoomVector->scaleBy(-1.0);
}

inline void BossJoints::Gap::setThicknessVector(const Ptr<Vector3D>& thicknessVector)
{
    m_thicknessVector = thicknessVector->copy();
}

inline void BossJoints::Gap::setWiggleRoom(double wiggleRoom)
{
    m_wiggleRoom = wiggleRoom;
}
