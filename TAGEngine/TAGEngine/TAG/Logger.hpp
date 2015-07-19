#pragma once

// https://github.com/fallahn/sfml-tmxloader/blob/master/include/tmx/Log.h

#include <SFML/System/NonCopyable.hpp>

#include <string>
#include <iostream>
#include <fstream>

// To enable logging define one of these in your preprocessor directives
#ifdef LOG_OUTPUT_ALL
#define LOG(m, t) Logger::Log(m, t, Logger::All)

#elif defined LOG_OUTPUT_CONSOLE
#define LOG(m, t) Logger::Log(m, t, Logger::Console)

#elif defined LOG_OUTPUT_FILE
#define LOG(m, t) Logger::Log(m, t, Logger::File)

#else
#define LOG(m, t)
#endif

#define LOG_INF(m) LOG(m, LOGGER::Type::Info)
#define LOG_WRN(m) LOG(m, LOGGER::Type::Warning)
#define LOG_ERR(m) LOG(m, LOGGER::Type::Error)

#ifdef _MSC_VER
#include <Windows.h>
#endif

class Logger final : private sf::NonCopyable
{
public:
    enum class Output
    {
        Console,
        File,
        All
    };
    
    enum Type
    {
        Info = (1 << 0),
        Warning = (1 << 1),
        Error = (1 << 2)
    };
    
    // logs a message to a given destination.
    static void Log(const std::string& message, Type type = Type::Info, Output output = Output::Console)
    {
        if(!mLogFilter & type) return;
        
        std::string outstring;
        
        switch(type)
        {
            case Type::Info:
            default:
                outstring = "INFO: " + message;
                break;
            case Type::Error:
                outstring = "ERROR: " + message;
                break;
            case Type::Warning:
                outstring = "WARNING: " + message;
                break;
        }
        
        if(output == Output::Console || output == Output::All)
        {
            (type == Type::Error) ?
                std::cerr << outstring << std::endl
            :
                std::cout << outstring << std::endl;
#ifdef _MSC_VER
            outstring += "\n";
            OutputDebugString(outstring.c_str());
#endif
        }
        if(output == Output::File || output == Output::All)
        {
            // output to a log file
            std::ofstream file("output.log", std::ios::app);
            if(file.good())
            {
                file << outstring << std::endl;
                file.close();
            }
            else
            {
                Log(message, type, Output::Console);
                Log("Above message was intended for log file. Opening file probably failed.", Type::Warning, Output::Console);
            }
        }
    }
    
    // Sets the level of logging via a bit mask
    static void SetLogLevel(int level)
    {
        mLogFilter = level;
    }
private:
    static int mLogFilter;
};