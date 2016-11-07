#pragma once

#include <string>

namespace BossJoints
{
    struct COMMAND {
        static const std::string ID;
        static const std::string NAME;
        static const std::string DESCRIPTION;
        static const std::string RESOURCE_PATH;
    };

    struct PLANE_SELECT {
        static const std::string ID;
        static const std::string NAME;
        static const std::string PROMPT;
    };

    struct EDGE_SELECT {
        static const std::string ID;
        static const std::string NAME;
        static const std::string PROMPT;
    };

    struct POSITION_GROUP {
        static const std::string ID;
        static const std::string NAME;
    };

    struct EDGE_USE_SELECT {
        static const std::string ID;
        static const std::string NAME;
    };

    struct CENTER_OFFSET_SELECT {
        static const std::string ID;
        static const std::string NAME;
    };

    struct CENTER_OFFSET_VALUE {
        static const std::string ID;
        static const std::string NAME;
    };

    struct WIDTH_VALUE {
        static const std::string ID;
        static const std::string NAME;
    };

    struct STYLE_GROUP {
        static const std::string ID;
        static const std::string NAME;
    };

    struct STYLE_SELECT {
        static const std::string ID;
        static const std::string NAME;
        static const std::string OPTION_TOOTH;
        static const std::string OPTION_GAP;
    };

    struct TOOTH_COUNT {
        static const std::string ID;
        static const std::string NAME;
    };

    struct GAP_COUNT {
        static const std::string ID;
        static const std::string NAME;
    };

    struct DIMENSION_GROUP {
        static const std::string ID;
        static const std::string NAME;
    };

    struct TOOL_DIAMETER {
        static const std::string ID;
        static const std::string NAME;
    };

    struct WIGGLE_ROOM {
        static const std::string ID;
        static const std::string NAME;
    };

    struct MATERIAL_THICKNESS {
        static const std::string ID;
        static const std::string NAME;
    };
}
