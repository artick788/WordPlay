#pragma once

#include "../WordPlayInclude.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace WordPlay {

    namespace fs = std::filesystem;

    struct WordEntry {
        std::string spanish;
        std::string english;

        std::string key() const { return spanish + " = " + english; }
    };

    struct WordList {
        std::string name;
        fs::path path;
        std::vector<WordEntry> entries;
        Size skippedLines = 0;
        bool selected = false;
    };

    // Lines look like "Hola = Hello"; blank lines and lines starting with '#' are ignored.
    std::optional<WordList> loadWordList(const fs::path& path);

    std::vector<fs::path> findWordListFiles(const fs::path& folder);

    // Compares digit runs numerically so that unidad_2 sorts before unidad_10.
    bool naturalLess(const std::string& a, const std::string& b);

}
