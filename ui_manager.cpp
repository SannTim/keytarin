// ui_manager.cpp
#include "ui_manager.h"
#include <locale.h>
#include <ncursesw/ncurses.h>
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
    wattron(main_win, COLOR_BLACK);
    box(main_win, 0, 0);
    mvwprintw(main_win, 0, 2, " Typing Trainer ");
    wattroff(main_win, COLOR_BLACK);
}

void UIManager::printStats(int wpm, float accuracy, int timeElapsed, wchar_t targetChar, bool strictMode) {
    wattron(main_win, COLOR_PAIR(3) | A_BOLD);
    mvwprintw(main_win, 1, 2, "WPM: %d | Accuracy: %.1f%% | Time: %ds | Target: %c |Mode: %s , ESC to exit", 
             wpm, accuracy, timeElapsed, targetChar, strictMode ? "STRICT" : "NORMAL");
    wattroff(main_win, COLOR_PAIR(3) | A_BOLD);
}

void UIManager::printText(const std::wstring& text, const std::wstring& userInput, size_t currentPos) {
    // Очищаем область текста (одна строка)
	const int textRow = LINES / 2;
    wmove(main_win, textRow, 1);
    wclrtoeol(main_win);

    const int visibleWidth = COLS - 4; // Учитываем рамку: 2 слева, 2 справа
    if (visibleWidth <= 0) return;

    // Определяем, с какого индекса начинать отображение
    int scrollOffset = 0;

    if (static_cast<int>(text.length()) <= visibleWidth) {
        // Весь текст помещается — показываем с начала
        scrollOffset = 0;
    } else {
        // Центрируем currentPos
        int desiredCenter = visibleWidth / 2;
        scrollOffset = static_cast<int>(currentPos) - desiredCenter;

        // Ограничиваем смещение, чтобы не выйти за границы текста
        scrollOffset = std::max(0, scrollOffset);
        scrollOffset = std::min(scrollOffset, static_cast<int>(text.length()) - visibleWidth);
    }

    // Отображаем только видимую часть
    int col = 2; // начальная колонка после рамки
    for (int i = 0; i < visibleWidth && (scrollOffset + i) < static_cast<int>(text.length()); ++i) {
        size_t idx = scrollOffset + i;
        wchar_t ch = text[idx];

        int color_pair = 0;
        int attributes = A_NORMAL;

        if (idx == currentPos) {
            color_pair = 4;
            attributes = A_BOLD | A_REVERSE;
        } else if (idx < userInput.length()) {
            if (text[idx] == userInput[idx]) {
                color_pair = 1;
                attributes = A_BOLD;
            } else {
                color_pair = 2;
                attributes = A_BOLD;
            }
        }

        wmove(main_win, textRow, col);
        if (color_pair > 0) {
            wattron(main_win, COLOR_PAIR(color_pair) | attributes);
        }
        waddch(main_win, ch);
        if (color_pair > 0) {
            wattroff(main_win, COLOR_PAIR(color_pair) | attributes);
        }
        col++;
    }

    // Очищаем остаток строки справа (на случай, если текст стал короче)
    while (col < COLS - 1) {
        mvwaddch(main_win, textRow, col++, ' ');
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
