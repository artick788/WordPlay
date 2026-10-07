#include <Pch.hpp>

#include "Store.hpp"

#include <algorithm>
#include <random>
#include <unordered_set>

namespace WordPlay {

    namespace {

        template<typename Fn>
        void forEachCard(const Store& store, Fn&& fn) {
            const auto mode = static_cast<GameMode>(store.config.mode);
            std::unordered_set<std::string> seen; // The same word may appear in several lists

            const auto visit = [&](const Direction direction) {
                const char* suffix = direction == Direction::SpanishToEnglish ? "|0" : "|1";
                for (const WordList& list : store.lists) {
                    if (!list.selected) {
                        continue;
                    }
                    for (const WordEntry& entry : list.entries) {
                        std::string key = entry.key();
                        if (store.config.hardOnly && !store.isHard(key)) {
                            continue;
                        }
                        if (!seen.insert(key + suffix).second) {
                            continue;
                        }
                        fn(entry, std::move(key), direction);
                    }
                }
            };

            // Mixed shows every word in both directions, all of one direction first.
            if (mode == GameMode::SpanishToEnglish || mode == GameMode::Mixed) {
                visit(Direction::SpanishToEnglish);
            }
            if (mode == GameMode::EnglishToSpanish || mode == GameMode::Mixed) {
                visit(Direction::EnglishToSpanish);
            }
        }

        void shuffleCards(std::vector<Card>& cards) {
            // Utils::Random is seeded with a constant, which would repeat the same order every run.
            static std::mt19937 generator{std::random_device{}()};
            std::ranges::shuffle(cards, generator);
        }

    }

    void Store::addFolder(const fs::path& folder) {
        statusMessage.clear();
        const std::vector<fs::path> files = findWordListFiles(folder);
        if (files.empty()) {
            statusMessage = "No .txt files found in " + folder.string();
            return;
        }
        folderPath = folder.string();
        for (const fs::path& file : files) {
            if (!addList(file, true)) {
                statusMessage = "Could not read " + file.filename().string();
            }
        }
        sortLists();
    }

    void Store::addFiles(const std::vector<fs::path>& files) {
        statusMessage.clear();
        for (const fs::path& file : files) {
            if (!addList(file, true)) {
                statusMessage = "Could not read " + file.filename().string();
            }
        }
        sortLists();
    }

    void Store::removeList(const Size index) {
        if (index < lists.size()) {
            lists.erase(lists.begin() + static_cast<std::ptrdiff_t>(index));
        }
    }

    void Store::reloadAll() {
        statusMessage.clear();
        const std::vector<WordList> previous = std::move(lists);
        lists.clear();
        for (const WordList& old : previous) {
            addList(old.path, old.selected); // Files that no longer exist are dropped
        }
        if (!folderPath.empty()) {
            for (const fs::path& file : findWordListFiles(folderPath)) {
                addList(file, false);
            }
        }
        sortLists();
    }

    void Store::setAllSelected(const bool selected) {
        for (WordList& list : lists) {
            list.selected = selected;
        }
    }

    Size Store::selectedCount() const {
        return static_cast<Size>(std::ranges::count_if(lists, [](const WordList& l) { return l.selected; }));
    }

    Size Store::countCards() const {
        Size count = 0;
        forEachCard(*this, [&](const WordEntry&, std::string&&, Direction) { ++count; });
        return count;
    }

    bool Store::isHard(const std::string& key) const {
        const auto it = stats.find(key);
        return it != stats.end() && it->second.missed > 0 && it->second.missed >= it->second.correct;
    }

    bool Store::startSession() {
        std::vector<Card> deck = buildDeck();
        if (deck.empty()) {
            return false;
        }
        if (config.shuffle) {
            shuffleCards(deck);
        }
        beginSession(std::move(deck));
        return true;
    }

    bool Store::practiceMissed() {
        std::vector<Card> deck = session.missedCards;
        if (deck.empty()) {
            return false;
        }
        if (config.shuffle) {
            shuffleCards(deck);
        }
        beginSession(std::move(deck));
        return true;
    }

    void Store::reveal() {
        if (session.active && !session.finished()) {
            session.revealed = true;
        }
    }

    void Store::answer(const bool correct) {
        if (!session.active || session.finished() || !session.revealed) {
            return;
        }
        const Card card = session.deck[session.position];
        WordStats& wordStats = stats[card.key];
        if (correct) {
            ++wordStats.correct;
            ++session.correct;
        } else {
            ++wordStats.missed;
            ++session.missed;
            session.deck.push_back(card); // Shown again at the end of the session
            const bool known = std::ranges::any_of(session.missedCards, [&](const Card& c) {
                return c.key == card.key && c.direction == card.direction;
            });
            if (!known) {
                session.missedCards.push_back(card);
            }
        }
        ++session.position;
        session.revealed = false;
    }

    void Store::endSession() {
        if (session.active) {
            session.ended = true;
        }
    }

    void Store::closeSession() {
        session = Session{};
    }

    StoreState Store::snapshot() const {
        StoreState state;
        state.folderPath = folderPath;
        state.stats = stats;
        state.config = config;
        for (const WordList& list : lists) {
            state.listPaths.push_back(list.path.string());
            if (list.selected) {
                state.selectedPaths.push_back(list.path.string());
            }
        }
        return state;
    }

    void Store::restore(const StoreState& state) {
        folderPath = state.folderPath;
        stats = state.stats;
        config = state.config;
        config.mode = std::clamp(config.mode, 0, static_cast<int>(GameMode::Mixed));
        lists.clear();
        for (const std::string& path : state.listPaths) {
            const bool selected = std::ranges::find(state.selectedPaths, path) != state.selectedPaths.end();
            addList(path, selected); // Files that no longer exist are dropped
        }
        sortLists();
    }

    bool Store::addList(const fs::path& path, const bool selected) {
        std::error_code ec;
        fs::path normalized = fs::weakly_canonical(path, ec);
        if (ec) {
            normalized = path;
        }
        if (const auto existing = std::ranges::find_if(lists, [&](const WordList& l) { return l.path == normalized; });
            existing != lists.end()) {
            existing->selected = existing->selected || selected;
            return true;
        }
        std::optional<WordList> list = loadWordList(normalized);
        if (!list) {
            return false;
        }
        list->selected = selected;
        lists.push_back(std::move(*list));
        return true;
    }

    void Store::sortLists() {
        std::ranges::stable_sort(lists, [](const WordList& a, const WordList& b) {
            return naturalLess(a.path.filename().string(), b.path.filename().string());
        });
    }

    std::vector<Card> Store::buildDeck() const {
        std::vector<Card> deck;
        forEachCard(*this, [&](const WordEntry& entry, std::string&& key, const Direction direction) {
            Card card;
            card.key = std::move(key);
            card.direction = direction;
            if (direction == Direction::SpanishToEnglish) {
                card.prompt = entry.spanish;
                card.answer = entry.english;
            } else {
                card.prompt = entry.english;
                card.answer = entry.spanish;
            }
            deck.push_back(std::move(card));
        });
        return deck;
    }

    void Store::beginSession(std::vector<Card> deck) {
        session = Session{};
        session.initialSize = deck.size();
        session.deck = std::move(deck);
        session.active = true;
    }

}
