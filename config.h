#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <tuple>
#include "include/libdd/libdd.h"

class CKeyboardMidiConfig
{
public:
    CKeyboardMidiConfig()
    {
        try {
            std::ifstream f("config.json");
            m_configjson = nlohmann::json::parse(f);

        } catch (const std::exception &e) {
            spdlog::error("Error while reading config file: \n{}", e.what());
        }
    }

    std::pair<int,int> GetVelocityClampValues()
    {
        return std::make_pair(m_configjson["Settings"]["MinVelocity"], m_configjson["Settings"]["MaxVelocity"]);
    }

    std::tuple <int,int,int> GetThresholds()
    {
        return std::make_tuple(m_configjson["Settings"]["Threshold1"], m_configjson["Settings"]["Threshold2"], m_configjson["Settings"]["Threshold3"]);
    }

    int GetKeyBindByName(CDrunkDeerInterface &keyboard,std::string key)
    {
        std::string target_key = m_configjson["Settings"][key];
        for (int i = 0 ; i < keyboard.layout.size(); i++)
        {
            if (keyboard.layout[i] == target_key)
            {
                return i;
            }
        }
    }

    std::unordered_map<int, int> BuildMidiKeyMap(CDrunkDeerInterface &keyboard)
    {
        std::unordered_map<int, int> key_map;
        for (auto &key : m_configjson["Keymap"].items())
        {
            std::string key_name = key.key();
            int key_id = key.value();
            for (int i = 0; i < keyboard.layout.size(); i++)
            {
                if (keyboard.layout[i] == key_name)
                {
                    key_map[i] = key_id;
                    break;
                }
            }
           // spdlog::error("Key {} not found in keymap", key_name);
        }
            
        return key_map;
    }
    nlohmann::json m_configjson;

};