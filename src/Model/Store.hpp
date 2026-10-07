#pragma once

#include "../WordPlayInclude.hpp"
#include "Session.hpp"
#include "WordList.hpp"

#include <VelyraUtils/DevUtils/JsonSerializer.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace WordPlay {

    enum class GameMode : int {
        SpanishToEnglish = 0,
        EnglishToSpanish = 1,
        Mixed = 2
    };

    struct WordStats {
        int correct = 0;
        int missed = 0;

        VL_GENERATE_JSON_SERIALIZER_RELAXED(WordStats, correct, missed)
    };

    struct GameConfig {
        int mode = static_cast<int>(GameMode::Mixed); // GameMode, kept as int for ImGui::RadioButton
        bool shuffle = true;
        bool hardOnly = false;

        VL_GENERATE_JSON_SERIALIZER_RELAXED(GameConfig, mode, shuffle, hardOnly)
    };

    // The part of the Store that is persisted in the application settings.
    struct StoreState {
        std::string folderPath;
        std::vector<std::string> listPaths;
        std::vector<std::string> selectedPaths;
        std::unordered_map<std::string, WordStats> stats;
        GameConfig config;

        VL_GENERATE_JSON_SERIALIZER_RELAXED(StoreState, folderPath, listPaths, selectedPaths, stats, config)
    };

    struct Store {
        std::string folderPath;
        std::vector<WordList> lists;
        std::unordered_map<std::string, WordStats> stats; // Keyed by WordEntry::key()
        GameConfig config;
        Session session;
        std::string statusMessage;

        // Word lists
        void addFolder(const fs::path& folder);
        void addFiles(const std::vector<fs::path>& files);
        void removeList(Size index);
        void reloadAll();
        void setAllSelected(bool selected);
        Size selectedCount() const;

        // Cards available with the current selection and config
        Size countCards() const;
        bool isHard(const std::string& key) const;

        // Session
        bool startSession();
        bool practiceMissed();
        void reveal();
        void answer(bool correct);
        void endSession();
        void closeSession();

        // Persistence
        StoreState snapshot() const;
        void restore(const StoreState& state);

    private:
        bool addList(const fs::path& path, bool selected);
        void sortLists();
        std::vector<Card> buildDeck() const;
        void beginSession(std::vector<Card> deck);
    };

}
