#include "frequency.h"

#include <algorithm>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "word_extracter.h"

namespace {

void toLower(std::string& str) {
    std::transform(
        str.begin(), str.end(), str.begin(),
        [] (unsigned char c) {
            return std::tolower(c);
        }
    );
}

std::string loadText(const std::filesystem::path& filename) {
    if (!exists(filename)) {
        return {};
    }

    std::ifstream stream(filename);
    if (!stream.is_open()) {
        return {};
    }

    std::string result;
    result.reserve(file_size(filename));

    getline(stream, result, {});
    toLower(result);

    return result;
}

Words::Raw countWords(const std::string& text) {
    Words::Raw result;

    WordExtracter wordExtracter(text);

    for (auto word = wordExtracter.nextWord();
            static_cast<bool>(word);
            word = wordExtracter.nextWord()) {
        ++result[*word];
    }

    return result;
}

std::ostream& operator<<(std::ostream& stream, const Words::Raw& words) {
    for (const auto& [count, word] : words) {
        stream << count << ' ' << word << '\n';
    }
    return stream;
}

struct Frequency {
    Frequency(int count, std::string_view word)
        : count(count)
        , word(word)
    {}

    int count;
    std::string_view word;
};

bool operator<(const Frequency& lhs, const Frequency& rhs) {
    if (lhs.count != rhs.count) {
        // This sign is inverted because
        // words with more count should appear higher
        return lhs.count > rhs.count;
    }
    // This sign is not inverted because
    // words with same count should appear in alphabetical order
    return lhs.word < rhs.word;
}

using FrequencyMap = std::vector<Frequency>;

auto convertToFrequencyMap(const Words::Raw& words) {
    FrequencyMap result;
    result.reserve(words.size());
    for ( const auto& [word, count] : words ) {
        result.emplace_back(count, word);
    }
    std::sort(result.begin(), result.end());
    return result;
}

void writeFrequencyMapToFile(const std::filesystem::path& filename,
                             const FrequencyMap& frequencyMap) {
    std::ofstream stream(filename);
    if (!stream.is_open()) {
        std::cout << "Error: Can not open file " <<
            filename << " to write frequency map" << '\n';
        return;
    }

    for (const auto& [count, word] : frequencyMap) {
        stream << count << ' ' << word << '\n';
    }
}

} // namespace

Words::Words(std::string inText)
    : text(std::move(inText))
    , raw(countWords(text))
{
}

Words countWordsInText(std::string text) {
    return {std::move(text)};
}

Words countWordsInFile(const std::filesystem::path& filename) {
    return countWordsInText(loadText(filename));
}

void writeWordsToFile(const std::filesystem::path& filename, const Words& words) {
    const FrequencyMap frequencyMap = convertToFrequencyMap(words.getRaw());
    writeFrequencyMapToFile(filename, frequencyMap);
}

void countWordsAndWrite(const std::filesystem::path& inputFilename,
                        const std::filesystem::path& outputFilename) {
    auto words = countWordsInFile(inputFilename);
    writeWordsToFile(outputFilename, words);
}
