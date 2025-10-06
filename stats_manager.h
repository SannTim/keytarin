// stats_manager.h
#pragma once

#include <string>
#include <map>
#include <vector>
#include <deque>

struct CharStats {
    int totalAttempts = 0;
    int correctAttempts = 0;
    std::deque<bool> recentAttempts; // Последние 100 попыток
    double getAccuracy() const;
};

class StatsManager {
public:
    StatsManager(const std::string& filename = "stats.json");
    void loadStats();
    void saveStats();
    void updateStats(wchar_t ch, bool correct);
    wchar_t getWorstChar() const;
    std::map<wchar_t, CharStats> getAllStats() const;
    
private:
    std::string filename_;
    std::map<wchar_t, CharStats> stats_;
};
