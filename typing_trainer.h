#pragma once
#include "ui_manager.h"
#include "text_generator.h"
#include "stats_manager.h"
#include <chrono>

class TypingTrainer {
public:
    TypingTrainer();
    void run();
    
private:
    UIManager ui;
    TextGenerator textGen;
    StatsManager statsManager;
    std::wstring practiceText;
    std::wstring userInput;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    size_t currentPosition;
    wchar_t currentRequiredChar;
    
    void calculateStats(int& wpm, float& accuracy, int& elapsedTime);
    void handleInput(int ch);
    bool isTrainingComplete();
    void updateStatistics();
    void startNewSession();
    void showStatsScreen();
};
