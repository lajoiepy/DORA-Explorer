#include "radiation_loop_functions.h"
#include <argos3/core/simulator/simulator.h>
#include <argos3/core/utility/configuration/argos_configuration.h>
#include <argos3/plugins/robots/foot-bot/simulator/footbot_entity.h>
#include <json/json.h>

const std::string RADIATION_SOURCES_FILE = "data/radiation_sources";

/****************************************/
/****************************************/

CRadiationLoopFunctions::CRadiationLoopFunctions() : CLoopFunctions() {
    /* Find experiment number and file */
    int experiment_number = -1;
    std::string file_name;
    do
    {
        experiment_number++;
    } while (std::ifstream(radiation_file_name_).good());
    radiation_file_name_ = RADIATION_SOURCES_FILE + std::to_string(experiment_number) + ".json";

    sources = this->ReadRadiationSources();
}

CRadiationLoopFunctions::~CRadiationLoopFunctions() {
}

void CRadiationLoopFunctions::Init(TConfigurationNode &t_node) {
    try {
        /* Get the radius of radiation items from XML */
        TConfigurationNode &tRadiation = GetNode(t_node, "radiation");
        GetNodeAttribute(tRadiation, "radius", m_fRadiationRadius);

        /* Create radiation entities */
        for (auto source : this->sources) {
            m_cVisibleRadiation.push_back(
                CCylinderEntity("c" + source.ToString(),
                                source.GetCoordinates(),
                                CQuaternion(),
                                false,
                                m_fRadiationRadius,
                                1.0));
        }
    } catch (CARGoSException &ex) {
        THROW_ARGOSEXCEPTION_NESTED("Error parsing loop functions!", ex);
    }
}

std::vector<RadiationSource> CRadiationLoopFunctions::ReadRadiationSources() {
    Json::Value radiationValues;
    Json::Reader reader;
    std::ifstream radiationFile(radiation_file_name_);
    std::vector<RadiationSource> sources;

    reader.parse(radiationFile, radiationValues);
    if (radiationValues["sources"].size() <= 0) {
        throw JSON_USE_EXCEPTION;
    }

    for (auto source : radiationValues["sources"]) {
        sources.push_back(RadiationSource(source["x"].asFloat(), source["y"].asFloat(), source["intensity"].asFloat()));
    }

    return sources;
}

/****************************************/
/****************************************/

REGISTER_LOOP_FUNCTIONS(CRadiationLoopFunctions, "radiation_loop_functions")