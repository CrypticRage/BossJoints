#include "Constants.h"

using namespace BossJoints;

const std::string COMMAND::ID =                 "BossJoints";
const std::string COMMAND::NAME =               "Boss Joints";
const std::string COMMAND::DESCRIPTION =        "Create a joint...like a boss.";
const std::string COMMAND::RESOURCE_PATH =      "./resources";

const std::string PLANE_SELECT::ID =            COMMAND::ID + "_plane_select";
const std::string PLANE_SELECT::NAME =          "Select Plane";
const std::string PLANE_SELECT::PROMPT =        "Select the primary plane...";

const std::string EDGE_SELECT::ID =             COMMAND::ID + "_edge_select";
const std::string EDGE_SELECT::NAME =           "Select Edge";
const std::string EDGE_SELECT::PROMPT =         "Select the primary edge...";

const std::string POSITION_GROUP::ID =          COMMAND::ID + "_position_group";
const std::string POSITION_GROUP::NAME =        "Joint Position";

const std::string EDGE_USE_SELECT::ID =         COMMAND::ID + "_edge_use_select";
const std::string EDGE_USE_SELECT::NAME =      "Use Entire Edge Length";

const std::string CENTER_OFFSET_SELECT::ID =    COMMAND::ID + "_center_offset_select";
const std::string CENTER_OFFSET_SELECT::NAME =  "Set Center Offset At Edge Center";

const std::string CENTER_OFFSET_VALUE::ID =     COMMAND::ID + "_center_offset_value";
const std::string CENTER_OFFSET_VALUE::NAME =   "Center Offset";

const std::string WIDTH_VALUE::ID =             COMMAND::ID + "_width_value";
const std::string WIDTH_VALUE::NAME =           "Joint Width";

const std::string STYLE_GROUP::ID =             COMMAND::ID + "_style_group";
const std::string STYLE_GROUP::NAME =           "Joint Style";

const std::string STYLE_SELECT::ID =            COMMAND::ID + "_style_select";
const std::string STYLE_SELECT::NAME =          "Style";
const std::string STYLE_SELECT::OPTION_TOOTH =  "Start With Tooth";
const std::string STYLE_SELECT::OPTION_GAP =    "Start With Gap";

const std::string TOOTH_COUNT::ID =             COMMAND::ID + "_tooth_count";
const std::string TOOTH_COUNT::NAME =           "Tooth Count";

const std::string GAP_COUNT::ID =               COMMAND::ID + "_gap_count";
const std::string GAP_COUNT::NAME =             "Gap Count";

const std::string DIMENSION_GROUP::ID =         COMMAND::ID + "_dimension_group";
const std::string DIMENSION_GROUP::NAME =       "Joint Dimensions";

const std::string TOOL_DIAMETER::ID =           COMMAND::ID + "_tool_diameter";
const std::string TOOL_DIAMETER::NAME =         "Tool Diameter";

const std::string WIGGLE_ROOM::ID =             COMMAND::ID + "_wiggle_room";
const std::string WIGGLE_ROOM::NAME =           "Wiggle Room";

const std::string MATERIAL_THICKNESS::ID =      COMMAND::ID + "_material_thickness";
const std::string MATERIAL_THICKNESS::NAME =    "Material Thickness";
