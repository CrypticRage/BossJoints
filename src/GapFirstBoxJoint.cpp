#include "GapFirstBoxJoint.h"

#include "Util.h"

using namespace BossJoints;

BossJoints::GapFirstBoxJoint::GapFirstBoxJoint()
{
}


BossJoints::GapFirstBoxJoint::~GapFirstBoxJoint()
{
}

bool BossJoints::GapFirstBoxJoint::sketch()
{
    bool isOk = false;

    Ptr<Component> comp = m_plane->body()->parentComponent();
    Ptr<Sketches> sketches = comp->sketches();
    
    m_edgeSketch = sketches->add(m_plane);
    isOk = m_edgeSketch->name("edge_profiles");
    if (!isOk) return false;
    m_edgeSketch->isComputeDeferred(true);

    m_midFilletSketch = sketches->add(m_plane);
    isOk = m_midFilletSketch->name("mid_fillet_profiles");
    if (!isOk) return false;
    m_midFilletSketch->isComputeDeferred(true);

    m_midGapSketch = sketches->add(m_plane);
    isOk = m_midGapSketch->name("mid_gap_profiles");
    if (!isOk) return false;
    m_midGapSketch->isComputeDeferred(true);

    createBorderSketch();
    createEdgeGapSketch(m_edgeSketch);
    createEdgeFilletSketch(m_edgeSketch);

    if (m_gapCount > 2)
    {
        createMidGapSketch(m_midGapSketch);
        createMidFilletSketch(m_midFilletSketch);
    }

    return true;
}

void BossJoints::GapFirstBoxJoint::createEdgeFilletSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;
    Ptr<Component> comp = m_plane->body()->parentComponent();

    Ptr<Point3D> refPoint = m_edgeStartPoint->copy();
    refPoint->translateBy(m_thicknessVector);

    // calculate wiggle room vectors
    Ptr<Vector3D> halfWiggleRoomVector = m_edgeVector->copy();
    halfWiggleRoomVector->normalize();
    halfWiggleRoomVector->scaleBy(m_wiggleRoom * 0.5);

    Ptr<Vector3D> negHalfWiggleRoomVector = halfWiggleRoomVector->copy();
    negHalfWiggleRoomVector->scaleBy(-1.0);

    // calculate gap vectors
    Ptr<Vector3D> gapWidthVector = m_edgeVector->copy();
    gapWidthVector->normalize();
    gapWidthVector->scaleBy(m_gapWidth + m_wiggleRoom / 2);

    Ptr<Vector3D> negGapWidthVector = gapWidthVector->copy();
    negGapWidthVector->scaleBy(-1.0);

    // calculate thickness vectors
    Ptr<Vector3D> negThicknessVector = m_thicknessVector->copy();
    negThicknessVector->scaleBy(-1.0);

    // start corner
    refPoint->translateBy(negHalfWiggleRoomVector);
    CornerFillet startCornerFillet(refPoint, gapWidthVector, negThicknessVector, m_toolDiameter, sketch);
    if (startCornerFillet.onSurface(m_plane, sketch))
    {
        startCornerFillet.drawSketch();
        Util::drawSurfaceOrientationVectors(m_plane, comp, startCornerFillet.centerPoint());
    }

    refPoint->translateBy(halfWiggleRoomVector);
    refPoint->translateBy(gapWidthVector);

    // start inside
    CornerFillet startInsideFillet(refPoint, negGapWidthVector, negThicknessVector, m_toolDiameter, sketch);
    startInsideFillet.drawSketch();

    refPoint = m_edgeEndPoint->copy();
    refPoint->translateBy(m_thicknessVector);
    refPoint->translateBy(halfWiggleRoomVector);

    // end corner
    CornerFillet endCornerFillet(refPoint, negGapWidthVector, negThicknessVector, m_toolDiameter, sketch);
    if (endCornerFillet.onSurface(m_plane, sketch))
    {
        endCornerFillet.drawSketch();
        Util::drawSurfaceOrientationVectors(m_plane, comp, endCornerFillet.centerPoint());
    }

    refPoint->translateBy(negHalfWiggleRoomVector);
    refPoint->translateBy(negGapWidthVector);

    // end inside
    CornerFillet endInsideFillet(refPoint, gapWidthVector, negThicknessVector, m_toolDiameter, sketch);
    endInsideFillet.drawSketch();
}

void BossJoints::GapFirstBoxJoint::createMidFilletSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    Ptr<Point3D> p3 = m_edgeStartPoint->copy();
    p3->translateBy(m_thicknessVector);

    // calculate wiggle room vectors
    Ptr<Vector3D> halfWiggleRoomVector = m_edgeVector->copy();
    halfWiggleRoomVector->normalize();
    halfWiggleRoomVector->scaleBy(m_wiggleRoom * 0.5);

    Ptr<Vector3D> negHalfWiggleRoomVector = halfWiggleRoomVector->copy();
    negHalfWiggleRoomVector->scaleBy(-1.0);

    // calculate tooth vectors
    Ptr<Vector3D> toothWidthVector = m_edgeVector->copy();
    toothWidthVector->normalize();
    toothWidthVector->scaleBy(m_toothWidth - m_wiggleRoom);

    // calculate gap vectors
    Ptr<Vector3D> gapWidthVector = m_edgeVector->copy();
    gapWidthVector->normalize();
    gapWidthVector->scaleBy(m_gapWidth + m_wiggleRoom);

    Ptr<Vector3D> negGapWidthVector = gapWidthVector->copy();
    negGapWidthVector->scaleBy(-1.0);

    Ptr<Point3D> refPoint = p3->copy();

    Ptr<Vector3D> negThicknessVector = m_thicknessVector->copy();
    negThicknessVector->scaleBy(-1.0);

    refPoint->translateBy(gapWidthVector);
    refPoint->translateBy(negHalfWiggleRoomVector);
    refPoint->translateBy(toothWidthVector);

    CornerFillet refLeftFillet(refPoint, gapWidthVector, negThicknessVector, m_toolDiameter, sketch);
    refLeftFillet.drawSketch();

    Ptr<Vector3D> slideVector = toothWidthVector->copy();
    slideVector->add(gapWidthVector);
    m_gapSpacing = slideVector->length();

    isOk = refPoint->translateBy(gapWidthVector);
    if (!isOk) return;

    CornerFillet refRightFillet(refPoint, negGapWidthVector, negThicknessVector, m_toolDiameter, sketch);
    refRightFillet.drawSketch();
}

void BossJoints::GapFirstBoxJoint::createMidGapSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    // calculate tooth vectors
    Ptr<Vector3D> toothVector = m_edgeVector->copy();
    isOk = toothVector->normalize();
    if (!isOk) return;
    isOk = toothVector->scaleBy(m_toothWidth);
    if (!isOk) return;

    // calculate gap vectors
    Ptr<Vector3D> halfGapVector = m_edgeVector->copy();
    isOk = halfGapVector->normalize();
    if (!isOk) return;
    isOk = halfGapVector->scaleBy(m_gapWidth / 2);
    if (!isOk) return;

    Ptr<Point3D> refPoint = m_edgeStartPoint->copy();
    isOk = refPoint->translateBy(halfGapVector);
    if (!isOk) return;
    isOk = refPoint->translateBy(halfGapVector);
    if (!isOk) return;
    isOk = refPoint->translateBy(toothVector);
    if (!isOk) return;
    isOk = refPoint->translateBy(halfGapVector);
    if (!isOk) return;

    m_refMidGap.setRefPoint(refPoint);
    m_refMidGap.setWiggleRoom(m_wiggleRoom);
    m_refMidGap.setHalfWidthVector(halfGapVector);
    m_refMidGap.setThicknessVector(m_thicknessVector);
    m_refMidGap.sketch(sketch);
}

void BossJoints::GapFirstBoxJoint::createEdgeGapSketch(const Ptr<Sketch>& sketch)
{
    bool isOk = false;

    // calculate tooth vectors
    Ptr<Vector3D> toothVector = m_edgeVector->copy();
    isOk = toothVector->normalize();
    if (!isOk) return;
    isOk = toothVector->scaleBy(m_toothWidth);
    if (!isOk) return;

    // calculate gap vectors
    Ptr<Vector3D> halfGapVector = m_edgeVector->copy();
    isOk = halfGapVector->normalize();
    if (!isOk) return;
    isOk = halfGapVector->scaleBy(m_gapWidth / 2);
    if (!isOk) return;

    Ptr<Vector3D> negHalfGapVector = halfGapVector->copy();
    isOk = negHalfGapVector->scaleBy(-1.0);
    if (!isOk) return;

    Ptr<Point3D> refPoint = m_edgeStartPoint->copy();
    isOk = refPoint->translateBy(halfGapVector);
    if (!isOk) return;

    Gap startPointGap, endPointGap;

    startPointGap.setRefPoint(refPoint);
    startPointGap.setWiggleRoom(m_wiggleRoom);
    startPointGap.setHalfWidthVector(halfGapVector);
    startPointGap.setThicknessVector(m_thicknessVector);
    startPointGap.sketch(sketch);

    refPoint = m_edgeEndPoint->copy();
    isOk = refPoint->translateBy(negHalfGapVector);
    if (!isOk) return;

    endPointGap.setRefPoint(refPoint);
    endPointGap.setWiggleRoom(m_wiggleRoom);
    endPointGap.setHalfWidthVector(halfGapVector);
    endPointGap.setThicknessVector(m_thicknessVector);
    endPointGap.sketch(sketch);
}

bool BossJoints::GapFirstBoxJoint::extrude()
{
    Ptr<Component> comp = m_plane->body()->parentComponent();

    extrudeProfiles(m_midGapSketch);
    extrudeProfiles(m_midFilletSketch);

    Ptr<ObjectCollection> midGapFeatures = Util::filterMatchingFeatures(comp, m_refMidGap.boundingBox());
    createGapPattern(comp, midGapFeatures, m_edgeVector, m_gapCount - 2);

    extrudeProfiles(m_edgeSketch);

    return true;
}
