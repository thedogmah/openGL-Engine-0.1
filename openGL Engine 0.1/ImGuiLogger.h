#pragma once
#include <vector>
#include <string>
#include <imgui.h>
#include "globals.h"
#include <iomanip>
#include <chrono>


class ImGuiLogger {
public:
    enum class LogType {
        Info,
        Warning,
        Error,
        OpenGL,
        Assimp,
        Shader
    };
    bool logWindowOpen = true;
    bool showInfo = true;    // Filter for Info logs
    bool showWarning = true; // Filter for Warning logs
    bool showError = true;   // Filter for Error logs
    bool showOpenGL = false;  // Filter for OpenGL logs
    bool showAssimp = true;    // Filter for Assimp logs
    bool showShader = true;
    bool pauseLogging = false;

    float fontScale = 1.25; //change size of font within program

    struct Log {
        std::string message;
        LogType type;
        std::chrono::system_clock::time_point timestamp;


        Log(const std::string& msg, LogType logType)
            : message(msg), type(logType) {
        this->timestamp = std::chrono::system_clock::now();
        }
    };

    // Constructor
    ImGuiLogger() : autoScroll(true) {
        this->AddLog("Application started", ImGuiLogger::LogType::Info);
        this->AddLog("This is a warning", ImGuiLogger::LogType::Warning);
        
    }

    // Method to add a log
    void AddLog(const std::string& message, LogType type = LogType::Info) {
       
      
        if(!pauseLogging) logBuffer.emplace_back(message, type);
    }

    // Method to clear all logs
    void Clear() {
        logBuffer.clear();
    }


    std::string getCurrentTimeString() {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::tm local_time = *std::localtime(&time_t_now);

        std::stringstream timeStream;
        timeStream << std::put_time(&local_time, "%H:%M:%S");
        return timeStream.str();
    }

    std::string formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
        // Convert time_point to time_t
        std::time_t time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);

        // Convert time_t to local time (e.g., HH:MM:SS)
        std::tm local_time = *std::localtime(&time_t_timestamp);

        // Use a stringstream to format the time into a string
        std::stringstream timeStream;
        timeStream << std::put_time(&local_time, "%H:%M:%S"); // Format as HH:MM:SS

        return timeStream.str(); // Return the formatted time string
    }
    // Method to draw the log window in ImGui
    void Draw(const char* title, bool* p_open = nullptr) {
        ImGui::Begin(title, p_open);
        // Options to filter log types
        if(ImGui::Button("Clear Log"))
        {
            logBuffer.clear();
        }
        ImGui::Checkbox("Show Info", &showInfo); ImGui::SameLine();
        ImGui::Checkbox("Show Warning", &showWarning); ImGui::SameLine();
        ImGui::Checkbox("Show Error", &showError); ImGui::SameLine();
        ImGui::Checkbox("Show OpenGL", &showOpenGL); ImGui::SameLine();
        ImGui::Checkbox("Show Assimp", &showAssimp); ImGui::SameLine();
        ImGui::Checkbox("Show Shader", &showShader); ImGui::SameLine();
        // Options
        ImGui::Checkbox("Auto-scroll", &autoScroll); ImGui::SameLine();
        ImGui::Checkbox("Pause Logging", &pauseLogging); 
        ImGui::PushItemWidth(100.0f);
        ImGui::SliderFloat("Text Size", &fontScale, 0.9, 2.0);
        ImGui::PopItemWidth();
        ImGui::BeginChild("LogOutput", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), false, ImGuiWindowFlags_HorizontalScrollbar);
       
        int index = 1;
        for (const auto& log : logBuffer) {
            index++;

            // Skip this log if it doesn't match the filter
            if ((log.type == LogType::Info && !showInfo) ||
                (log.type == LogType::Warning && !showWarning) ||
                (log.type == LogType::Error && !showError) ||
                (log.type == LogType::OpenGL && !showOpenGL) ||
                (log.type == LogType::Assimp && !showAssimp) ||
                (log.type == LogType::Shader && !showShader)
                
                ) {
                continue;
            }

            // Create the log entry prefix with index and timestamp
            std::string entry = std::to_string(index);
            std::string timeStr = formatTimestamp(log.timestamp); // Use the stored timestamp
            std::string prefix = entry + ": [" + timeStr + "] ";
            ImGui::SetWindowFontScale(fontScale);
            // Display the log message with appropriate colors based on the log type
            if (log.type == LogType::Error) {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s[Error] %s", prefix.c_str(), log.message.c_str());
            }
            else if (log.type == LogType::Warning) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s[Warning] %s", prefix.c_str(), log.message.c_str());
            }
            else if (log.type == LogType::OpenGL) {
                ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.2f, 1.0f), "%s[OpenGL] %s", prefix.c_str(), log.message.c_str());
            }
            else if (log.type == LogType::Assimp) {
                ImGui::TextColored(ImVec4(0.5f, 0.4f, 0.5f, 1.0f), "%s[Assimp] %s", prefix.c_str(), log.message.c_str());
            }
            else if (log.type == LogType::Shader) {
                ImGui::TextColored(ImVec4(0.7f, 0.8f, 0.8f, 1.0f), "%s[Shader] %s", prefix.c_str(), log.message.c_str());
            }
            else {
                ImGui::Text("%s[Info] %s", prefix.c_str(), log.message.c_str());
            }
            ImGui::SetWindowFontScale(1.0);
        }

        // Auto-scroll
        if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::End();




        std::string logFilePath = "log.txt"; // Path to your log file
        std::ofstream logFile(logFilePath, std::ios::trunc); // Open log file in append mode

        // Check if the file is open
        if (!logFile.is_open()) {
            std::cerr << "Failed to open log file: " << logFilePath << std::endl;
            return; // Exit if we can't open the log file
        }

        // Write all log messages to the file
        for (const auto& log : logBuffer) {
            std::string logEntry;

            // You may want to format the log entry here
            logEntry += std::to_string(index) + ": "; // Adding index
            logEntry += log.message + "\n"; // Add log message with newline

            logFile << logEntry; // Write to the file
        }

        logFile.close(); // Close the log file
    }

private:
    std::vector<Log> logBuffer; // Stores all the logs
    bool autoScroll;            // Automatically scroll to the bottom
};



