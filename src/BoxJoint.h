#pragma once

#include <Core/Utils.h>
#include <Core/Application/ValueInput.h>

#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepBody.h>
#include <Fusion/BRep/BRepVertex.h>

#include <Core/Geometry/Point2D.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/SurfaceEvaluator.h>

#include <Fusion/Components/Component.h>

#include <Fusion/Fusion/ModelParameter.h>

#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>

#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchLineList.h>
#include <Fusion/Sketch/SketchPoint.h>
#include <Fusion/Sketch/SketchPoints.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchCircle.h>
#include <Fusion/Sketch/SketchCircles.h>
#include <Fusion/Sketch/SketchDimensions.h>
#include <Fusion/Sketch/SketchDimension.h>
#include <Fusion/Sketch/SketchDimensionList.h>
#include <Fusion/Sketch/GeometricConstraints.h>
#include <Fusion/Sketch/GeometricConstraintList.h>
#include <Fusion/Sketch/PerpendicularConstraint.h>
#include <Fusion/Sketch/SketchAngularDimension.h>
#include <Fusion/Sketch/SketchLinearDimension.h>
#include <Fusion/Sketch/CoincidentConstraint.h>

#include "ProfileUtil.h"
#include "Debug.h"
#include "Gap.h"
#include "FilletBase.h"
#include "CornerFillet.h"

using namespace adsk::core;
using namespace adsk::fusion;

class BoxJoint
{
private:
    enum class Style {
        StartWithGap,
        StartWithTooth
    };

    Ptr<BRepFace> m_plane;
    Ptr<BRepEdge> m_edge;
    double m_matThickess;
    double m_wiggleRoom;

    unsigned int m_toothCount;
    unsigned int m_gapCount;
    Style m_style;

    Ptr<Vector3D> m_thicknessVector;
    Ptr<Vector3D> m_edgeVector;
    Ptr<Point3D> m_edgeStartPoint;
    Ptr<Point3D> m_edgeEndPoint;

    Ptr<Profile> m_borderProfile;
    Ptr<Sketch> m_gapSketch;
    Ptr<Sketch> m_filletSketch;

    BoxJoint::BoxJoint();

    Ptr<Vector3D> findScaleVector(const Ptr<SketchLine>& line, double length);

public:
    ~BoxJoint();

    Ptr<BRepFace> plane() const;
    void setPlane(const Ptr<BRepFace>& plane);

    Ptr<BRepEdge> edge() const;
    void setEdge(const Ptr<BRepEdge>& edge);

    unsigned int toothCount() const;
    void setToothCount(unsigned int toothCount);

    unsigned int gapCount() const;
    void setGapCount(unsigned int gapCount);

    double matThickness() const;
    void setMatThickness(const double matThickness);

    double wiggleRoom() const;
    void setWiggleRoom(const double wiggleRoom);

    void setStyle(const std::string& style);

    bool isPointOnSurface(const Ptr<Point3D>& point, const Ptr<BRepFace>& plane);

    bool extrudeProfiles(const Ptr<Sketch>& sketch);
    bool extrudeGaps();
    bool extrudeFillets();

    void createFilletSketch(const double toolDiameter);
    void createGapSketch();
    void createBorderSketch();

    static BoxJoint *create(
        const Ptr<BRepFace>& plane, const Ptr<BRepEdge>& edge, double matThickness
    );
};

inline Ptr<BRepFace> BoxJoint::plane() const
{
    return m_plane;
}

inline void BoxJoint::setPlane(const Ptr<BRepFace>& plane)
{
    m_plane = plane;
}

inline Ptr<BRepEdge> BoxJoint::edge() const
{
    return m_edge;
}

inline void BoxJoint::setEdge(const Ptr<BRepEdge>& edge)
{
    m_edge = edge;
    m_edgeStartPoint = m_edge->startVertex()->geometry();
    m_edgeEndPoint = m_edge->endVertex()->geometry();
    m_edgeVector = m_edgeStartPoint->vectorTo(m_edgeEndPoint);
}

inline unsigned int BoxJoint::toothCount() const
{
    return m_toothCount;
}

inline void BoxJoint::setToothCount(unsigned int toothCount)
{
    if (toothCount == 0)
        return;

    m_toothCount = toothCount;

    if (m_style == Style::StartWithTooth)
    {
        m_gapCount = m_toothCount - 1;
    }
    else if (m_style == Style::StartWithGap)
    {
        m_gapCount = m_toothCount + 1;
    }
}

inline unsigned int BoxJoint::gapCount() const
{
    return m_gapCount;
}

inline void BoxJoint::setGapCount(unsigned int gapCount)
{
    if (gapCount == 0)
        return;

    m_gapCount = gapCount;

    if (m_style == Style::StartWithTooth)
    {
        m_toothCount = m_gapCount + 1;
    }
    else if (m_style == Style::StartWithGap)
    {
        m_toothCount = m_gapCount - 1;
    }
}

inline double BoxJoint::matThickness() const
{
    return m_matThickess;
}

inline void BoxJoint::setMatThickness(double matThickness)
{
    if (matThickness < 0.0)
        return;

    m_matThickess = matThickness;
}

inline double BoxJoint::wiggleRoom() const
{
    return m_wiggleRoom;
}

inline void BoxJoint::setWiggleRoom(double wiggleRoom)
{
    m_wiggleRoom = wiggleRoom;
}

inline void BoxJoint::setStyle(const std::string& style)
{
    if (style == "Start With Tooth")
    {
        m_style = Style::StartWithTooth;
        setToothCount(m_toothCount);
    }
    else if (style == "Start With Gap")
    {
        m_style = Style::StartWithGap;
        setGapCount(m_gapCount);
    }
}
