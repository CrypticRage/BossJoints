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
#include "Constants.h"

using namespace adsk::core;
using namespace adsk::fusion;

namespace BossJoints
{
    class Joint
    {
    protected:
        Joint();

        Ptr<BRepFace> m_plane;
        Ptr<BRepEdge> m_edge;

        double m_centerOffset;
        double m_width;
        double m_fullEdgeWidth;
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
        bool createGapPattern(
            const Ptr<Component>& comp, const Ptr<ObjectCollection>& gapFeatures,
            Ptr<Vector3D> dirVector, int count
        );
        void updateToothWidth();
        void updateGapWidth();
        void updateEdgeVector();

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

        double width() const;
        void setWidth(const double width);

        double centerOffset() const;
        void setCenterOffset(const double centerOffset);

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
}

inline Ptr<BRepFace> BossJoints::Joint::plane() const
{
    return m_plane;
}

inline void BossJoints::Joint::setPlane(const Ptr<BRepFace>& plane)
{
    m_plane = plane;
}

inline Ptr<BRepEdge> BossJoints::Joint::edge() const
{
    return m_edge;
}

inline void BossJoints::Joint::setEdge(const Ptr<BRepEdge>& edge)
{
    m_edge = edge;
    m_edgeStartPoint = m_edge->startVertex()->geometry();
    m_edgeEndPoint = m_edge->endVertex()->geometry();
    m_edgeVector = m_edgeStartPoint->vectorTo(m_edgeEndPoint);

    m_width = m_edgeVector->length();
    m_fullEdgeWidth = m_edgeVector->length();

    m_centerOffset = m_fullEdgeWidth / 2;
}

inline double BossJoints::Joint::centerOffset() const
{
    return m_centerOffset;
}

inline void BossJoints::Joint::updateEdgeVector()
{
    if ((m_centerOffset == 0.0) || (m_fullEdgeWidth == 0))
        return;

    Ptr<Vector3D> centerOffsetVector = m_edgeVector->copy();
    centerOffsetVector->normalize();
    centerOffsetVector->scaleBy(m_centerOffset);

    Ptr<Vector3D> halfWidthVector = m_edgeVector->copy();
    halfWidthVector->normalize();
    halfWidthVector->scaleBy(m_width / 2);

    Ptr<Vector3D> negHalfWidthVector = halfWidthVector->copy();
    negHalfWidthVector->scaleBy(-1.0);

    m_edgeStartPoint = m_edge->startVertex()->geometry();
    m_edgeStartPoint->translateBy(centerOffsetVector);
    m_edgeStartPoint->translateBy(negHalfWidthVector);

    m_edgeEndPoint = m_edge->startVertex()->geometry();
    m_edgeEndPoint->translateBy(centerOffsetVector);
    m_edgeEndPoint->translateBy(halfWidthVector);

    m_edgeVector = m_edgeStartPoint->vectorTo(m_edgeEndPoint);
}

inline void BossJoints::Joint::setCenterOffset(double centerOffset)
{
    if (m_edgeVector == nullptr)
        return;

    double test = m_edgeVector->length();

    if ((centerOffset < 0.0) || (centerOffset >= m_fullEdgeWidth))
        return;

    m_centerOffset = centerOffset;

    updateEdgeVector();
}

inline double BossJoints::Joint::width() const
{
    return m_width;
}

inline void BossJoints::Joint::setWidth(double width)
{
    if (width < 0.0)
        return;

    m_width = width;

    updateEdgeVector();
}

inline double BossJoints::Joint::matThickness() const
{
    return m_matThickess;
}

inline void BossJoints::Joint::setMatThickness(double matThickness)
{
    if (matThickness < 0.0)
        return;

    m_matThickess = matThickness;
}

inline double BossJoints::Joint::wiggleRoom() const
{
    return m_wiggleRoom;
}

inline void BossJoints::Joint::setWiggleRoom(double wiggleRoom)
{
    m_wiggleRoom = wiggleRoom;
}

inline double BossJoints::Joint::toolDiameter() const
{
    return m_toolDiameter;
}

inline void BossJoints::Joint::setToolDiameter(const double toolDiameter)
{
    m_toolDiameter = toolDiameter;
}

inline unsigned int BossJoints::Joint::toothCount() const
{
    return m_toothCount;
}

inline unsigned int BossJoints::Joint::gapCount() const
{
    return m_gapCount;
}

inline void BossJoints::Joint::updateToothWidth()
{
    if ((m_toothCount <= 0) && (m_gapCount <= 0))
        return;

    m_toothWidth = m_edgeVector->length() / (double)(m_toothCount + m_gapCount);
}

inline void BossJoints::Joint::updateGapWidth()
{
    m_gapWidth = m_toothWidth;
}
