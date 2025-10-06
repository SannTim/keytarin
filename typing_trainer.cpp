// typing_trainer.cpp
#include "typing_trainer.h"
#include <thread>

TypingTrainer::TypingTrainer() 
    : textGen("dictionary.txt"), statsManager("stats.json") {
}

void TypingTrainer::run() {
    startNewSession();
    
    int ch;
    while((ch = getch()) != 27) { // ESC для выхода
        if (ch == '`') {
            showStatsScreen();
            continue;
        }
        
        handleInput(ch);
        
        int wpm;
        float accuracy;
        int elapsed;
        calculateStats(wpm, accuracy, elapsed);
        
        ui.clearScreen();
        ui.printStats(wpm, accuracy, elapsed, currentRequiredChar);
        ui.printText(practiceText, userInput, currentPosition);
        ui.refreshScreen();
        
        if(isTrainingComplete()) {
            updateStatistics();
            ui.printResult(wpm, accuracy);
            startNewSession();
        }
    }
    
    statsManager.saveStats();
}

void TypingTrainer::startNewSession() {
    // Получаем букву с наихудшей статистикой
    currentRequiredChar = statsManager.getWorstChar();
    practiceText = textGen.getPracticeText(currentRequiredChar);
    userInput.clear();
    currentPosition = 0;
    startTime = std::chrono::steady_clock::now();
}

void TypingTrainer::updateStatistics() {
    for (size_t i = 0; i < userInput.length() && i < practiceText.length(); i++) {
        bool correct = (userInput[i] == practiceText[i]);
        statsManager.updateStats(practiceText[i], correct);
    }
}

void TypingTrainer::showStatsScreen() {
    auto allStats = statsManager.getAllStats();
    ui.showStatistics(allStats);
}

void TypingTrainer::calculateStats(int& wpm, float& accuracy, int& elapsedTime) {
    auto now = std::chrono::steady_clock::now();
    elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
    
    size_t correctChars = 0;
    for(size_t i = 0; i < userInput.length() && i < practiceText.length(); ++i) {
        if(userInput[i] == practiceText[i]) correctChars++;
    }
    accuracy = (userInput.empty() ? 0 : (correctChars * 100.0 / userInput.length()));
    
    size_t words = userInput.length() / 5;
    wpm = (elapsedTime > 0) ? static_cast<int>((words * 60) / elapsedTime) : 0;
}

void TypingTrainer::handleInput(int ch) {
    if(ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
        if(!userInput.empty() && currentPosition > 0) {
            userInput.pop_back();
            currentPosition--;
        }
    } else if(ch >= 32 && ch <= 126) {
        if (currentPosition < practiceText.length()) {
            if (currentPosition == userInput.length()) {
                userInput += static_cast<wchar_t>(ch);
            } else {
                userInput[currentPosition] = static_cast<wchar_t>(ch);
            }
            currentPosition++;
        }
    } else if(ch == KEY_LEFT) {
        if(currentPosition > 0) currentPosition--;
    } else if(ch == KEY_RIGHT) {
        if(currentPosition < userInput.length()) currentPosition++;
    }
}

bool TypingTrainer::isTrainingComplete() {
    return userInput.length() >= practiceText.length();
}
