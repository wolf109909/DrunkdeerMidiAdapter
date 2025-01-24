#pragma once
#include "hidapi.h"
#include "spdlog/spdlog.h"
#include "layouts.h"
#include "packets.h"
#include "encodings.h"
#include <atomic>
#include <unordered_set>
#include <queue>
typedef struct _CallResult
{
    bool success;
    std::string error;
    unsigned char result[256];
    
} CallResult;

enum ECommandType
{
    RequestID = 0x01,
    RequestKeys = 0x02,
};

const char* GetPacketDataByType(ECommandType type)
{
    switch (type)
    {
    case RequestID:
        return CMD_REQUEST_ID;
    case RequestKeys:
        return CMD_REQUEST_KEYS;
    default:
        return nullptr;
    }
}

int GetResponsePacketCount(ECommandType type)
{
    switch (type)
    {
    case RequestID:
        return 1;
    case RequestKeys:
        return 3;
    default:
        return 0;
    }
}

class KB_COMMAND
{
public:

    KB_COMMAND(ECommandType type)
    {
        auto cmd = GetPacketDataByType(type);
        this->type = type;
        memcpy(this->cmd, cmd, sizeof(cmd));
        this->size = sizeof(cmd);
    }
    ECommandType type;
    unsigned char cmd[64];
    size_t size;
};

class CHIDInterface
{
public:
    CHIDInterface(){};
    bool Open()
    {
        if (hid_init() == 0)
        {
            spdlog::info("HIDAPI initialized");
        }
        else
        {
            spdlog::error("Failed to initialize HIDAPI: {}", ConvertWideToUtf8(hid_error(nullptr)));
            return false;
        }

        auto devs = hid_enumerate(0x0, 0x0);
        for (; devs; devs = devs->next)
        {
            
            if (devs->vendor_id == 0x352D &&
                (devs->product_id == 0x2383 || devs->product_id == 0x2382 ||
                 devs->product_id == 0x2384 || devs->product_id == 0x2386) &&
                devs->usage == 0x0)
            {
                spdlog::info("Device found: {}", devs->path);
                current_keyboard = hid_open_path(devs->path);
                hid_free_enumeration(devs);
                hid_set_nonblocking(current_keyboard, 0);
                return true;
            }
        }
        spdlog::error("Failed to enumerate or target devices not found");
        hid_free_enumeration(devs);
        hid_exit();
        return false;
    }
    void Close()
    {
        if (current_keyboard)
        {
            hid_close(current_keyboard);
        }
        hid_exit();
    }

    void NotifySend()
    {
        send_block.store(true);
    }

    int SendPacketSync(const unsigned char *buf, size_t len)
    {
        return hid_write(current_keyboard, buf, len);
    }

    int ReadPacketSync(unsigned char *buf, size_t len)
    {
        return hid_read(current_keyboard, buf, len);
    }
    hid_device *GetHIDHandle()
    {
        return current_keyboard;
    }

private:
    struct hid_device_info *devs;
    hid_device *current_keyboard;
    std::atomic_bool send_block;
    long long send_interval_ms = 5;
};

class CDrunkDeerInterface
{
public:
    CDrunkDeerInterface() : keyboard()
    {
        if (!Initialize())
        {
            throw std::runtime_error("Failed to initialize CDrunkDeerInterface");
        }
    };
    ~CDrunkDeerInterface()
    {
        keyboard.Close();
    }

    bool Initialize()
    {
        if (!keyboard.Open())
        {
            spdlog::error("Failed to open HID interface");
            return false;
        }

        auto result = SendCommandSync(KB_COMMAND(ECommandType::RequestID));
        if (!result.success)
        {
            spdlog::error("Failed to get keyboard identifier: {}", result.error);
            return false;
        }
        keyboard_identifier = ParseIdentifierResult(result.result);
        spdlog::info("Keyboard identifier: {}", GetKeyboardNameFromId(keyboard_identifier));

        if(keyboard_identifier == 75 || keyboard_identifier == 750)
        {
            layout = keyboard_layout_a75;
        }

        return true;
    }
    CallResult SendCommandSync(KB_COMMAND cmd)
    {
        int res = keyboard.SendPacketSync(cmd.cmd, cmd.size);
        if (res < 0)
        {
            spdlog::error("Unable to write(): {}", ConvertWideToUtf8(hid_error(keyboard.GetHIDHandle())));
            return {false, ConvertWideToUtf8(hid_error(keyboard.GetHIDHandle()))};
        }

        unsigned char result[256];

        for (int i = 0; i < GetResponsePacketCount(cmd.type); i++)
        {
            unsigned char buf[64];
            int res = keyboard.ReadPacketSync(buf, sizeof(buf));
            if (res < 0)
            {
                spdlog::error("Unable to read(): {}", ConvertWideToUtf8(hid_error(keyboard.GetHIDHandle())));
                return {false, ConvertWideToUtf8(hid_error(keyboard.GetHIDHandle()))};
            }
            else if (res > 0)
            {
                unsigned char command = buf[1];
                unsigned char arg1 = buf[2];
                unsigned char arg2 = buf[3];
                unsigned char arg3 = buf[4];
                if (cmd.type == ECommandType::RequestID)
                {
                    if (command == 0xa0 && arg1 == 0x02 && arg2 == 0x00)
                    {
                        //spdlog::info("Received response with type: {}", command);
                        memcpy(result, buf + 5, 59);
                    }else
                    {
                        i = 0;
                        continue;
                    }
                }
                else if (cmd.type == ECommandType::RequestKeys) 
                {
                    if (command == 0xb7)
                        memcpy(result + i * 59, buf + 5, 59);
                }
                

                //spdlog::info("Received response with type: {}", command);
            }
        }

        auto callresult = CallResult();
        callresult.success = true;
        callresult.error = "";
        memcpy(callresult.result, result, sizeof(result));
        return callresult;

    }
    int keyboard_identifier;
    int send_interval_ms = 5;
    std::vector<std::string> layout;
    std::mutex send_queue_mutex;
    CHIDInterface keyboard;
};