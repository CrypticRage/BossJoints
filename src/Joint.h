#pragma once

#include <Core/Utils.h>
#include <Core/Application/ValueInput.h>

#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepEdge.h>
#include <Fusion/BRep/BRepEdges.h>
#include <Fusion/BRep/BRepVertex.h>
#include <Fusion/BRep/BRepBody.h>

#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/Sketches.h>
#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/SketchCurves.h>
#include <Fusion/Sketch/SketchLines.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchLineList.h>
#include <Fusion/Sketch/SketchPoint.h>

#include <Fusion/Components/Component.h>

#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>
#include <Fusion/Features/ExtrudeFeatureInput.h>
#include <Fusion/Features/RectangularPatternFeature.h>
#include <Fusion/Features/RectangularPatternFeatures.h>
#include <Fusion/Features/RectangularPatternFeatureInput.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/SurfaceEvaluator.h>

#include "Debug.h"
#include "Util.h"

using namespace adsk::core;
using namespace adsk::fusion;

class Joint
{
protected:
    Joint();

    Ptr<BRepFace> m_plane;
    Ptr<BRepEdge> m_edge;
    double m_matThickess;
    double m_wiggleRoom;
    double m_toolDiameter;

    unsigned int m_toothCount;
    double m_toothWidth;
    unsigned int m_gapCount;
    double m_gapWidth;
    double m_gapSpacing;

    Ptr<Vector3D> m_thicknessVector;
    Ptr<Vector3D> m_edgeVector;
    Ptr<Point3D> m_edgeStartPoint;
    Ptr<Point3D> m_edgeEndPoint;

    Ptr<Profile> m_borderProfile;
    Ptr<SketchLine> m_midLine;

    bool extrudeProfiles(const Ptr<Sketch>& sketch);
    bool createGapPattern(const Ptr<Component>& comp, const Ptr<ObjectCollection>& gapFeatures, int count);
    void updateToothWidth();
    void updateGapWidth();

public:
    static Joint *create(
        const Ptr<BRepFace>& plane, const Ptr<BRepEdge>& edge, const std::string& style
    );

    ~Joint();

    virtual bool extrude() = 0;
    virtual bool sketch() = 0;
    
    void createBorderSketch();

    Ptr<BRepFace> plane() const;
    void setPlane(const Ptr<BRepFace>& plane);

    Ptr<BRepEdge> edge() const;
    void setEdge(const Ptr<BRepEdge>& edge);

    double matThickness() const;
    void setMatThickness(const double matThickness);

    double wiggleRoom() const;
    void setWiggleRoom(const double wiggleRoom);

    double toolDiameter() const;
    void setToolDiameter(const double toolDiameter);

    unsigned int toothCount() const;
    virtual void setToothCount(unsigned int toothCount) = 0;

    unsigned int gapCount() const;
    virtual void setGapCount(unsigned int gapCount) = 0;
};

inline Ptr<BRepFace> Joint::plane() const
{
    return m_plane;
}

inline void Joint::setPlane(const Ptr<BRepFace>& plane)
{
    m_plane = plane;
}

inline Ptr<BRepEdge> Joint::edge() const
{
    return m_edge;
}

inline void Joint::setEdge(const Ptr<BRepEdge>& edge)
{
    m_edge = edge;
    m_edgeStartPoint = m_edge->startVertex()->geometry();
    m_edgeEndPoint = m_edge->endVertex()->geometry();
    m_edgeVector = m_edgeStartPoint->vectorTo(m_edgeEndPoint);
}

inline double Joint::matThickness() const
{
    return m_matThickess;
}

inline void Joint::setMatThickness(double matThickness)
{
    if (matThickness < 0.0)
        return;

    m_matThickess = matThickness;
}

inline double Joint::wiggleRoom() const
{
    return m_wiggleRoom;
}

inline void Joint::setWiggleRoom(double wiggleRoom)
{
    m_wiggleRoom = wiggleRoom;
}

inline double Joint::toolDiameter() const
{
    return m_toolDiameter;
}

inline void Joint::setToolDiameter(const double toolDiameter)
{
    m_toolDiameter = toolDiameter;
}

inline unsigned int Joint::toothCount() const
{
    return m_toothCount;
}

inline unsigned int Joint::gapCount() const
{
    return m_gapCount;
}

inline void Joint::updateToothWidth()
{
    if ((m_toothCount <= 0) && (m_gapCount <= 0))
        return;

    m_toothWidth = m_edgeVector->length() / (double)(m_toothCount + m_gapCount);
}

inline void Joint::updateGapWidth()
{
    m_gapWidth = m_toothWidth;
}
