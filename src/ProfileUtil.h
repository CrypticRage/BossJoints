#pragma once

#include <Fusion/Sketch/Profiles.h>
#include <Fusion/Sketch/Profile.h>
#include <Fusion/Sketch/Sketch.h>

#include <Core/Geometry/Point3D.h>
#include <Core/Geometry/BoundingBox3D.h>

#include <Core/Application/ObjectCollection.h>

#include "Debug.h"

using namespace adsk::core;
using namespace adsk::fusion;

class ProfileUtil
{
    public:
        static void printProfile(const Ptr<Profile> &profile);
        static void printProfiles(const Ptr<Sketch>& sketch);
        static bool profileBoxesEqual(const Ptr<Profile> &p1, const Ptr<Profile> &p2);
        static Ptr<ObjectCollection> stripBorderProfile(const Ptr<Sketch>& sketch, const Ptr<Profile> &borderProfile);
};
