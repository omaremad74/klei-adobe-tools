#include "logger.hpp"

Logger::Logger(const std::string& path) : m_LogPath(path), m_LogStart(std::chrono::steady_clock::now()) {
    log_stream.open(m_LogPath, std::ios::out);
    log_stream << "[00:00:00]: Logger started.\n";
    log_stream.close();
}