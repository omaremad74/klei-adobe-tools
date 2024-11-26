#pragma once

#include <string>
#include <chrono>
#include <iostream>
#include <fstream>
#include <format>

// Basic logging implementation

class Logger {
	public:
		Logger(const Logger&) = delete;

        static Logger& Get(const std::string& path = "") {
            static Logger logger(path);
            return logger;
        }

        static void Initialize(const std::string& path) {
            Get(path);
        }

        static void Print(const std::string& str) {
            Logger& logger = Get();

            using namespace std::chrono;
        
            const auto duration = steady_clock::now() - logger.m_LogStart;
            const auto elapsed_hours    = duration_cast<hours>  (duration);
            const auto elapsed_minutes  = duration_cast<minutes>(duration) % 60;
            const auto elapsed_seconds  = duration_cast<seconds>(duration) % 60;

            // POSSIBLE OPTIMIZATION (Omar): cache our timer string when we can.
            logger.log_stream.open(logger.m_LogPath, LOGGER_OPEN_MODE);
            logger.log_stream << std::format("[{:02}:{:02}:{:02}]: ", elapsed_hours.count(), elapsed_minutes.count(), elapsed_seconds.count());
            logger.log_stream << str << "\n";
            logger.log_stream.close();
        }

	private:
        Logger(const std::string& path);

        std::string m_LogPath;
        std::chrono::time_point<std::chrono::steady_clock> m_LogStart;
        std::ofstream log_stream;

        static constexpr inline auto LOGGER_OPEN_MODE = std::ios::app | std::ios::out;
};