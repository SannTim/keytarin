// text_generator.h
#pragma once
#include <cstddef>
#include <vector>
#include <string>
#include <map>

class TextGenerator {
public:
    TextGenerator(const std::string& dictionaryPath = "dictionary.txt");
    std::vector<std::wstring> generateWords(int count, wchar_t requiredChar = 0);
    std::wstring getPracticeText(wchar_t requiredChar = 0);
    void loadDictionary(const std::string& path);
    
private:
    std::vector<std::wstring> dictionary_;
    std::map<wchar_t, std::vector<std::wstring>> wordsByChar_;
    
    void buildCharIndex();
    bool containsChar(const std::wstring& word, wchar_t ch);
};
