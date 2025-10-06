#include "ui_manager.h"
#include <locale.h>
#include <wchar.h>
#include "stats_manager.h"

UIManager::UIManager() {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    initColors();
    
    main_win = newwin(LINES, COLS, 0, 0);
}

UIManager::~UIManager() {
    delwin(main_win);
    endwin();
}

void UIManager::initColors() {
    if(has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_YELLOW, COLOR_BLACK);
        init_pair(4, COLOR_CYAN, COLOR_BLACK);
        init_pair(5, COLOR_WHITE, COLOR_BLUE);
        init_pair(6, COLOR_MAGENTA, COLOR_BLACK); // Для целевой буквы
    }
}

void UIManager::drawBorder() {
    wattron(main_win, COLOR_PAIR(5));
    box(main_win, 0, 0);
    mvwprintw(main_win, 0, 2, " Typing Trainer ");
    wattroff(main_win, COLOR_PAIR(5));
}

void UIManager::printStats(int wpm, float accuracy, int timeElapsed, wchar_t targetChar) {
    wattron(main_win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(main_win, 1, 2, "WPM: %d | Accuracy: %.1f%% | Time: %ds | Target: %c | Press 's' for stats, ESC to exit", 
             wpm, accuracy, timeElapsed, targetChar);
    wattroff(main_win, COLOR_PAIR(3) | A_BOLD);
}

void UIManager::printText(const std::wstring& text, const std::wstring& userInput, size_t currentPos) {
    // Очищаем только область текста
    for (int i = 3; i < LINES - 1; i++) {
        wmove(main_win, i, 1);
        wclrtoeol(main_win);
    }
    
    int row = 3;
    int col = 2;
    int max_width = COLS - 4;
    int current_col = col;
    int current_row = row;
    
    for(size_t i = 0; i < text.length(); ++i) {
        if(current_col >= max_width) {
            current_row++;
            current_col = 2;
        }
        
        if(current_row >= LINES - 2) {
            break;
        }
        
        wmove(main_win, current_row, current_col);
        
        // Определяем цвет символа
        int color_pair = 0;
        int attributes = A_NORMAL;
        
        if(i == currentPos) {
            color_pair = 4;
            attributes = A_BOLD | A_REVERSE;
        } else if(i < userInput.length()) {
            if(text[i] == userInput[i]) {
                color_pair = 1;
                attributes = A_BOLD;
            } else {
                color_pair = 2;
                attributes = A_BOLD;
            }
        }
        
        if (color_pair > 0) {
            wattron(main_win, COLOR_PAIR(color_pair) | attributes);
        }
        
        waddch(main_win, text[i]);
        
        if (color_pair > 0) {
            wattroff(main_win, COLOR_PAIR(color_pair) | attributes);
        }
        
        current_col++;
    }
}

void UIManager::printResult(int wpm, float accuracy) {
    WINDOW* result_win = newwin(8, 40, LINES/2-4, COLS/2-20);
    wattron(result_win, COLOR_PAIR(5));
    box(result_win, 0, 0);
    wattroff(result_win, COLOR_PAIR(5));
    
    wattron(result_win, A_BOLD);
    mvwprintw(result_win, 1, 12, "RESULTS");
    wattroff(result_win, A_BOLD);
    
    mvwprintw(result_win, 3, 10, "WPM: %d", wpm);
    mvwprintw(result_win, 4, 10, "Accuracy: %.1f%%", accuracy);
    mvwprintw(result_win, 6, 8, "Press any key to continue");
    
    wrefresh(result_win);
    getch();
    delwin(result_win);
}

void UIManager::showStatistics(const std::map<wchar_t, CharStats>& stats) {
    WINDOW* stats_win = newwin(LINES-4, COLS-4, 2, 2);
    wattron(stats_win, COLOR_PAIR(5));
    box(stats_win, 0, 0);
    mvwprintw(stats_win, 0, 2, " Statistics ");
    wattroff(stats_win, COLOR_PAIR(5));
    
    int row = 1;
    mvwprintw(stats_win, row++, 2, "Char | Accuracy | Total | Recent (last 100)");
    mvwprintw(stats_win, row++, 2, "-----+----------+-------+------------------");
    
    for (const auto& [ch, charStats] : stats) {
        if (row >= LINES-6) break;
        
        int correctRecent = 0;
        for (bool correct : charStats.recentAttempts) {
            if (correct) correctRecent++;
        }
        
        mvwprintw(stats_win, row, 2, "  %c  |", ch);
        mvwprintw(stats_win, row, 9, " %6.1f%% |", charStats.getAccuracy());
        mvwprintw(stats_win, row, 18, " %4d |", charStats.totalAttempts);
        mvwprintw(stats_win, row, 26, " %3d/%3d", correctRecent, 
                 static_cast<int>(charStats.recentAttempts.size()));
        row++;
    }
    
    mvwprintw(stats_win, LINES-7, 2, "Press any key to continue...");
    
    wrefresh(stats_win);
    getch();
    delwin(stats_win);
}

void UIManager::refreshScreen() {
    wrefresh(main_win);
}

void UIManager::clearScreen() {
    wclear(main_win);
    drawBorder();
}
