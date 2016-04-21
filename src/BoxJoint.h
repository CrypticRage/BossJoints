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
#include "Fillet.h"

using namespace adsk::core;
using namespace adsk::fusion;

class BoxJoint
{
private:
    Ptr<BRepFace> m_plane;
    Ptr<BRepEdge> m_edge;
    unsigned int m_toothCount;
    double m_matThickess;

    Ptr<Profile> m_borderProfile;
    Ptr<Sketch> m_gapSketch;
    Ptr<Sketch> m_filletSketch;
    Ptr<Point3D> m_backPoint;

    BoxJoint();
    ~BoxJoint();
    Ptr<Vector3D> findScaleVector(const Ptr<SketchLine>& line, double length);

    void checkSurfacePoint(const Ptr<Sketch>& sketch);

public:
    Ptr<BRepFace> plane() const;
    void setPlane(const Ptr<BRepFace>& plane);

    Ptr<BRepEdge> edge() const;
    void setEdge(const Ptr<BRepEdge>& edge);

    unsigned int toothCount() const;
    void setToothCount(unsigned int toothCount);

    double matThickness() const;
    void setMatThickness(double matThickness);

    bool extrudeProfiles(const Ptr<Sketch>& sketch);
    bool extrudeGaps();
    bool extrudeFillets();

    void createFilletSketch(const double toolDiameter);
    void createGapSketch();
    void createBorderSketch();

    static BoxJoint *create(
        const Ptr<BRepFace>& plane, const Ptr<BRepEdge>& edge,
        unsigned int toothCount, double matThickness
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
}

inline unsigned int BoxJoint::toothCount() const
{
    return m_toothCount;
}

inline void BoxJoint::setToothCount(unsigned int toothCount)
{
    m_toothCount = toothCount;
}

inline double BoxJoint::matThickness() const
{
    return m_matThickess;
}

inline void BoxJoint::setMatThickness(double matThickness)
{
    m_matThickess = matThickness;
}
