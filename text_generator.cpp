// text_generator.cpp
#include "text_generator.h"
#include <cstddef>
#include <fstream>
#include <random>
#include <algorithm>
#include <locale>
#include <codecvt>

TextGenerator::TextGenerator(const std::string& dictionaryPath) {
    loadDictionary(dictionaryPath);
}

void TextGenerator::loadDictionary(const std::string& path) {
    std::wifstream file(path);
    if (!file.is_open()) {
        // Если файла нет, используем встроенный словарь
        dictionary_ = {
            L"the", L"be", L"to", L"of", L"and", L"a", L"in", L"that", L"have", L"I",
            L"it", L"for", L"not", L"on", L"with", L"he", L"as", L"you", L"do", L"at"
        };
        buildCharIndex();
        return;
    }
    
    // Устанавливаем локаль для правильного чтения UTF-8
    std::locale utf8_locale("en_US.UTF-8");
    file.imbue(utf8_locale);
    
    std::wstring word;
    while (std::getline(file, word)) {
        if (!word.empty()) {
            dictionary_.push_back(word);
        }
    }
    
    buildCharIndex();
}

void TextGenerator::buildCharIndex() {
    wordsByChar_.clear();
    for (const auto& word : dictionary_) {
        for (wchar_t ch : word) {
            wordsByChar_[ch].push_back(word);
        }
    }
}

bool TextGenerator::containsChar(const std::wstring& word, wchar_t ch) {
    return word.find(ch) != std::wstring::npos;
}

std::vector<std::wstring> TextGenerator::generateWords(int count, wchar_t requiredChar) {
    std::vector<std::wstring> result;
    
    if (requiredChar != 0 && wordsByChar_.count(requiredChar) > 0) {
        // Используем слова, содержащие нужную букву
        const auto& wordsWithChar = wordsByChar_[requiredChar];
        std::sample(
            wordsWithChar.begin(),
            wordsWithChar.end(),
            std::back_inserter(result),
            std::min(count, static_cast<int>(wordsWithChar.size())),
            std::mt19937{std::random_device{}()}
        );
    }
    
    // Если не набрали достаточно слов, добавляем случайные из общего словаря
    if (result.size() < static_cast<size_t>(count)) {
        int remaining = count - result.size();
        std::sample(
            dictionary_.begin(),
            dictionary_.end(),
            std::back_inserter(result),
            remaining,
            std::mt19937{std::random_device{}()}
        );
    }
    
    return result;
}

std::wstring TextGenerator::getPracticeText(wchar_t requiredChar, int count) {
    auto words = generateWords(count, requiredChar);
    std::wstring result;
    for (const auto& word : words) {
        result += word + L" ";
    }
    return result;
}
