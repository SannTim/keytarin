#include "stats_manager.h"
#include <fstream>
#include <algorithm>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

double CharStats::getAccuracy() const {
    if (totalAttempts == 0) return 100.0;
    return (correctAttempts * 100.0) / totalAttempts;
}

StatsManager::StatsManager(const std::string& filename) 
    : filename_(filename) {
    loadStats();
}

void StatsManager::loadStats() {
    std::ifstream file(filename_);
    if (!file.is_open()) {
        return; // Файл не существует, начнем с пустой статистики
    }
    
    try {
        json j;
        file >> j;
        
        for (auto& [key, value] : j.items()) {
            if (key.length() == 1) {
                wchar_t ch = key[0];
                CharStats charStats;
                charStats.totalAttempts = value["total"];
                charStats.correctAttempts = value["correct"];
                
                if (value.contains("recent")) {
                    for (bool attempt : value["recent"]) {
                        charStats.recentAttempts.push_back(attempt);
                    }
                }
                
                stats_[ch] = charStats;
            }
        }
    } catch (const std::exception& e) {
        // В случае ошибки парсинга, начнем с пустой статистики
        stats_.clear();
    }
}

void StatsManager::saveStats() {
    json j;
    
    for (const auto& [ch, charStats] : stats_) {
        std::string key(1, static_cast<char>(ch)); // Convert wchar_t to char for JSON key
        j[key]["total"] = charStats.totalAttempts;
        j[key]["correct"] = charStats.correctAttempts;
        
        json recentArray = json::array();
        for (bool attempt : charStats.recentAttempts) {
            recentArray.push_back(attempt);
        }
        j[key]["recent"] = recentArray;
    }
    
    std::ofstream file(filename_);
    file << j.dump(4);
}

void StatsManager::updateStats(wchar_t ch, bool correct) {
    CharStats& charStats = stats_[ch];
    charStats.totalAttempts++;
    if (correct) {
        charStats.correctAttempts++;
    }
    
    charStats.recentAttempts.push_back(correct);
    if (charStats.recentAttempts.size() > 100) {
        charStats.recentAttempts.pop_front();
    }
}

wchar_t StatsManager::getWorstChar() const {
    if (stats_.empty()) {
        return L'e'; // По умолчанию, если статистики нет
    }
    
    wchar_t worstChar = stats_.begin()->first;
    double worstAccuracy = 100.0;
    
    for (const auto& [ch, charStats] : stats_) {
        double accuracy = charStats.getAccuracy();
        if (accuracy < worstAccuracy) {
            worstAccuracy = accuracy;
            worstChar = ch;
        }
    }
    
    return worstChar;
}

std::map<wchar_t, CharStats> StatsManager::getAllStats() const {
    return stats_;
}
