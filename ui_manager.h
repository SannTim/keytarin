// ui_manager.h
#pragma once
#include <ncursesw/ncurses.h>
#include <string>
#include <vector>
#include <map>

struct CharStats;

class UIManager {
public:
    UIManager();
    ~UIManager();
    
    void initColors();
    void printStats(int wpm, float accuracy, int timeElapsed, wchar_t targetChar);
    void printText(const std::wstring& text, const std::wstring& userInput, size_t currentPos);
    void printResult(int wpm, float accuracy);
    void refreshScreen();
    void clearScreen();
    void drawBorder();
    void showStatistics(const std::map<wchar_t, CharStats>& stats);
    
private:
    WINDOW* main_win;
};
