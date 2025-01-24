#include <iostream>
#include "include/libdd/libdd.h"
#include "include/libdd/packets.h"
#include "libremidi/libremidi.hpp"
#include "config.h"
#include "include/libdd/encodings.h"
#include <vector>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <libremidi/libremidi.hpp>
#include <thread>
#include <cmath>
#include <algorithm>
#include <tuple>



void DisplayKeys(CDrunkDeerInterface &keyboard)
{
    std::vector<short> key_heights;
    for (char x : keyboard.SendCommandSync(KB_COMMAND(ECommandType::RequestKeys)).result)
    {
        if (x == -52)
        {
            break;
        }
        key_heights.push_back(x);
    }
    for (int i = 0; i < 127; i++)
    {
        std::cout << key_heights[i] << " ";
        if ((i + 1) % 16 == 0)
        {
            std::cout << std::endl;
        }
    }
    key_heights.clear();
}


struct KeyState {
    std::unordered_map<int, std::chrono::steady_clock::time_point> timestamps; 
    bool is_pressed = false; 
};

int main(int, char **)
{
    auto keyboard = CDrunkDeerInterface();
    auto config = CKeyboardMidiConfig();
    // Create a MIDI output instance
    libremidi::midi_out midiOut;

    libremidi::observer obs;
    for (const libremidi::output_port &port : obs.get_output_ports())
    {
        std::cout << port.port_name << "\n";
        if (port.port_name == "DDMidiPort")
        {
            midiOut.open_port(port);
        }
    }
 
    bool pedal_toggle = false;
    int pedal_on = 0;
    int oct_offset = 0;
    int key_offset = 0;

    int key_pedal = config.GetKeyBindByName(keyboard,"Pedal");
    int key_octup = config.GetKeyBindByName(keyboard,"OctUp");
    int key_octdown = config.GetKeyBindByName(keyboard,"OctDown");

    int key_up = config.GetKeyBindByName(keyboard,"KeyUp");
    int key_down = config.GetKeyBindByName(keyboard,"KeyDown");

    auto thresholds = config.GetThresholds();
    const int threshold1 = std::get<0>(thresholds);
    const int threshold2 = std::get<1>(thresholds);
    const int threshold3 = std::get<2>(thresholds);

    const int min_velocity = config.GetVelocityClampValues().first;
    const int max_velocity = config.GetVelocityClampValues().second;

    auto key_map = config.BuildMidiKeyMap(keyboard);

    std::unordered_map<int, KeyState> key_states;
    std::vector<short> old_height(127, 0);

    while (true)
    {
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::vector<short> key_heights;
        for (char x : keyboard.SendCommandSync(KB_COMMAND(ECommandType::RequestKeys)).result)
        {
            key_heights.push_back(x);
        }

        for (int i = 0; i < 127; i++)
        {
            auto &state = key_states[i]; // Get the state for the current key

            if (key_heights[i] >= threshold1 && old_height[i] < threshold1)
            {
                state.timestamps[threshold1] = std::chrono::steady_clock::now();
            }
            if (key_heights[i] >= threshold2 && old_height[i] < threshold2)
            {
                state.timestamps[threshold2] = std::chrono::steady_clock::now();
            }
            if (key_heights[i] >= threshold3 && old_height[i] < threshold3)
            {
                state.timestamps[threshold3] = std::chrono::steady_clock::now();
            }


            if (key_heights[i] >= threshold3 && !state.is_pressed)
            {
                if (key_pedal == i)
                {
                    if (pedal_toggle)
                    {
                        midiOut.send_message(libremidi::channel_events::control_change(1, 64, 127*pedal_on));
                        pedal_on = !pedal_on;
                    }
                    else
                    {
                        midiOut.send_message(libremidi::channel_events::control_change(1, 64, 127));
                    }
                        
                    state.is_pressed = true;
                    
                    continue;
                }

                if (key_up == i)
                {
                    key_offset++;
                    for (int i=0;i<127;i++)
                    {
                        midiOut.send_message(libremidi::channel_events::note_off(1, i, 0));
                    }
                    spdlog::info("Key Up: {}", key_offset);
                    state.is_pressed = true;
                    continue;
                }

                if (key_down == i)
                {
                    key_offset--;
                    for (int i=0;i<127;i++)
                    {
                        midiOut.send_message(libremidi::channel_events::note_off(1, i, 0));
                    }
                    spdlog::info("Key Down: {}", key_offset);
                    state.is_pressed = true;
                    continue;
                }

                if (key_octup == i)
                {
                    if(oct_offset > 2)
                        continue;

                    oct_offset++;
                    for (int i=0;i<127;i++)
                    {
                        midiOut.send_message(libremidi::channel_events::note_off(1, i, 0));
                    }
                    spdlog::info("Octave Up: {}", oct_offset);
                    state.is_pressed = true;
                    continue;
                }

                if (key_octdown == i)
                {
                    if(oct_offset < -2)
                        continue;
                    oct_offset--;
                    for (int i=0;i<127;i++)
                    {
                        midiOut.send_message(libremidi::channel_events::note_off(1, i, 0));
                    }
                    spdlog::info("Octave Down: {}", oct_offset);
                    state.is_pressed = true;
                    continue;
                }

                if( key_map[i] == 0)
                    continue;

                // Calculate velocity based on time between thresholds
                if (state.timestamps.count(threshold1) && state.timestamps.count(threshold3))
                {
                    auto time1 = state.timestamps[threshold1];
                    auto time3 = state.timestamps[threshold3];
                    auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(time3 - time1).count();

                    // Map time_diff inversely to velocity
                    int velocity = std::clamp(max_velocity - static_cast<int>(time_diff), min_velocity, max_velocity);

                    midiOut.send_message(libremidi::channel_events::note_on(1, key_map[i] + (oct_offset * 12) + key_offset, velocity));
                    std::cout << "Sent Note On: " << key_map[i] << " with Velocity: " << velocity << std::endl;
                }

                state.is_pressed = true;
            }


            if (key_heights[i] < threshold1 && state.is_pressed)
            {   
                if (key_pedal == i && pedal_toggle == 0)
                {
                    midiOut.send_message(libremidi::channel_events::control_change(1, 64, 0));
                    state.is_pressed = false;
                    pedal_on = !pedal_on;
                    continue;
                }
                // Send MIDI Note Off
                midiOut.send_message(libremidi::channel_events::note_off(1, key_map[i] + (oct_offset * 12) + key_offset, 0));
                std::cout << "Sent Note Off: " << key_map[i] << std::endl;

                state.timestamps.clear();
                state.is_pressed = false;
            }


            old_height[i] = key_heights[i];
        }
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        std::chrono::milliseconds elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        SetConsoleTitle(ConvertUtf8ToWide(fmt::format("Drunkdeer Midi Adatper ( EPR: {} hz | Oct: {} | Key: {} )", 1000 / elapsed.count(),std::to_string(oct_offset),std::to_string(key_offset))).c_str());
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    midiOut.close_port();
    std::cout << "Virtual MIDI port closed." << std::endl;
}