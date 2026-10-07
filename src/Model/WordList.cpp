#include <Pch.hpp>

#include "WordList.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace WordPlay {

    namespace {

        std::string trim(const std::string_view text) {
            constexpr const char* whitespace = " \t\r\n";
            const Size begin = text.find_first_not_of(whitespace);
            if (begin == std::string_view::npos) {
                return {};
            }
            const Size end = text.find_last_not_of(whitespace);
            return std::string(text.substr(begin, end - begin + 1));
        }

        bool isDigit(const char c) {
            return std::isdigit(static_cast<unsigned char>(c)) != 0;
        }

    }

    bool naturalLess(const std::string& a, const std::string& b) {
        Size i = 0;
        Size j = 0;
        while (i < a.size() && j < b.size()) {
            if (isDigit(a[i]) && isDigit(b[j])) {
                Size iEnd = i;
                Size jEnd = j;
                while (iEnd < a.size() && isDigit(a[iEnd])) ++iEnd;
                while (jEnd < b.size() && isDigit(b[jEnd])) ++jEnd;
                // Skip leading zeros but keep at least one digit.
                while (i + 1 < iEnd && a[i] == '0') ++i;
                while (j + 1 < jEnd && b[j] == '0') ++j;
                const Size lenA = iEnd - i;
                const Size lenB = jEnd - j;
                if (lenA != lenB) {
                    return lenA < lenB;
                }
                const int cmp = a.compare(i, lenA, b, j, lenB);
                if (cmp != 0) {
                    return cmp < 0;
                }
                i = iEnd;
                j = jEnd;
            } else {
                const int ca = std::tolower(static_cast<unsigned char>(a[i]));
                const int cb = std::tolower(static_cast<unsigned char>(b[j]));
                if (ca != cb) {
                    return ca < cb;
                }
                ++i;
                ++j;
            }
        }
        return (a.size() - i) < (b.size() - j);
    }

    std::optional<WordList> loadWordList(const fs::path& path) {
        std::ifstream file(path);
        if (!file) {
            return std::nullopt;
        }

        WordList list;
        list.name = path.stem().string();
        list.path = path;

        std::string line;
        bool firstLine = true;
        while (std::getline(file, line)) {
            if (firstLine) {
                firstLine = false;
                if (line.starts_with("\xEF\xBB\xBF")) {
                    line.erase(0, 3);
                }
            }

            const std::string trimmed = trim(line);
            if (trimmed.empty() || trimmed.front() == '#') {
                continue;
            }

            // Only the first '=' splits, so translations may contain '='.
            const Size separator = trimmed.find('=');
            if (separator == std::string::npos) {
                ++list.skippedLines;
                continue;
            }

            std::string spanish = trim(std::string_view(trimmed).substr(0, separator));
            std::string english = trim(std::string_view(trimmed).substr(separator + 1));
            if (spanish.empty() || english.empty()) {
                ++list.skippedLines;
                continue;
            }
            list.entries.push_back({std::move(spanish), std::move(english)});
        }
        return list;
    }

    std::vector<fs::path> findWordListFiles(const fs::path& folder) {
        std::vector<fs::path> files;
        std::error_code ec;
        for (fs::directory_iterator it(folder, ec), end; !ec && it != end; it.increment(ec)) {
            if (it->is_regular_file(ec) && it->path().extension() == ".txt") {
                files.push_back(it->path());
            }
        }
        std::ranges::sort(files, [](const fs::path& a, const fs::path& b) {
            return naturalLess(a.filename().string(), b.filename().string());
        });
        return files;
    }

}
