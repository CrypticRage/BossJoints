#include "Gap.h"

Gap::Gap(const Ptr<Point3D>& refPoint, const Ptr<Vector3D>& halfWidthVector, const Ptr<Vector3D>& thicknessVector, double wiggleRoom)
{
    m_refPoint = refPoint;
    m_halfWidthVector = halfWidthVector;
    m_thicknessVector = thicknessVector;
    m_wiggleRoom = wiggleRoom;

    m_negHalfWidthVector = m_halfWidthVector->copy();
    m_negHalfWidthVector->scaleBy(-1.0);

    m_wiggleRoomVector = m_halfWidthVector->copy();
    m_wiggleRoomVector->normalize();
    m_wiggleRoomVector->scaleBy(wiggleRoom);

    m_negWiggleRoomVector = m_wiggleRoomVector->copy();
    m_negWiggleRoomVector->scaleBy(-1.0);
}

Gap::~Gap()
{
}

// create the profile of all the gaps in the joint
void Gap::sketch(const Ptr<Sketch>& sketch)
{
    Ptr<Point3D> p1 = m_refPoint->copy();
    p1->translateBy(m_negHalfWidthVector);

    Ptr<Point3D> p2 = m_refPoint->copy();
    p2->translateBy(m_halfWidthVector);

    Ptr<Point3D> p3 = m_refPoint->copy();
    p3->translateBy(m_thicknessVector);

    Ptr<SketchLineList> baseLines = sketch->sketchCurves()->sketchLines()->addThreePointRectangle(
        sketch->modelToSketchSpace(p1),
        sketch->modelToSketchSpace(p2),
        sketch->modelToSketchSpace(p3)
    );

    for (unsigned int i = 0; i < baseLines->count(); i++)
    {
        Ptr<SketchLine> line = baseLines->item(i);
        line->isConstruction(true);
    }

    p1->translateBy(m_negWiggleRoomVector);
    p2->translateBy(m_wiggleRoomVector);

    sketch->sketchCurves()->sketchLines()->addThreePointRectangle(
        sketch->modelToSketchSpace(p1),
        sketch->modelToSketchSpace(p2),
        sketch->modelToSketchSpace(p3)
    );
}
