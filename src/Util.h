#pragma once

#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Sketch.h>
#include <Fusion/Sketch/SketchLine.h>
#include <Fusion/Sketch/SketchPoint.h>

#include <Fusion/BRep/BRepFace.h>
#include <Fusion/BRep/BRepFaces.h>

#include <Fusion/Fusion/TimelineObject.h>
#include <Fusion/Components/Component.h>

#include <Fusion/Features/Features.h>
#include <Fusion/Features/ExtrudeFeature.h>
#include <Fusion/Features/ExtrudeFeatures.h>

#include <Core/Geometry/Point2D.h>
#include <Core/Geometry/Vector3D.h>
#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/BoundingBox3D.h>
#include <Core/Geometry/SurfaceEvaluator.h>

#include <Core/Application/ObjectCollection.h>

#include "Debug.h"

using namespace adsk::core;
using namespace adsk::fusion;

class Util
{
    public:
        static void printProfile(const Ptr<Profile> &profile);
        static void printProfiles(const Ptr<Sketch>& sketch);
        static bool isPointOnSurface(const Ptr<Point3D>& point, const Ptr<BRepFace>& plane);
        static Ptr<Vector3D> findScaleVector(const Ptr<SketchLine>& line, double length);
        static Ptr<ObjectCollection> filterMatchingFeatures(const Ptr<Component>& comp, const Ptr<BoundingBox3D>& box);
        static bool profileBoxesEqual(const Ptr<Profile> &p1, const Ptr<Profile> &p2);
        static Ptr<ObjectCollection> stripBorderProfile(const Ptr<Sketch>& sketch, const Ptr<Profile> &borderProfile);
};
