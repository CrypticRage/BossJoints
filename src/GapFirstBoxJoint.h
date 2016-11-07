#pragma once

#include "Joint.h"
#include "CornerFillet.h"
#include "Gap.h"

namespace BossJoints
{
    class GapFirstBoxJoint : public Joint
    {
    private:
        Gap m_refMidGap;
        Ptr<Sketch> m_midGapSketch;
        Ptr<Sketch> m_midFilletSketch;
        Ptr<Sketch> m_edgeSketch;

        void createMidGapSketch(const Ptr<Sketch>& sketch);
        void createEdgeFilletSketch(const Ptr<Sketch>& sketch);
        void createMidFilletSketch(const Ptr<Sketch>& sketch);
        void createEdgeGapSketch(const Ptr<Sketch>& sketch);

    public:
        GapFirstBoxJoint();
        ~GapFirstBoxJoint();

        virtual bool sketch();
        virtual bool extrude();

        virtual void setGapCount(unsigned int gapCount);
        virtual void setToothCount(unsigned int toothCount);
    };
}

inline void BossJoints::GapFirstBoxJoint::setGapCount(unsigned int gapCount)
{
    if (gapCount < 2)
        return;

    m_gapCount = gapCount;
    m_toothCount = m_gapCount - 1;

    updateToothWidth();
    updateGapWidth();
}

inline void BossJoints::GapFirstBoxJoint::setToothCount(unsigned int toothCount)
{
    if (toothCount < 1)
        return;

    m_toothCount = toothCount;
    m_gapCount = m_toothCount + 1;

    updateToothWidth();
    updateGapWidth();
}
