#pragma once

#include "../WordPlayInclude.hpp"

#include <string>
#include <vector>

namespace WordPlay {

    enum class Direction {
        SpanishToEnglish,
        EnglishToSpanish
    };

    // Cards hold copies of the strings so a session survives lists being removed or reloaded.
    struct Card {
        std::string prompt;
        std::string answer;
        std::string key; // Statistics key shared by both directions of a word
        Direction direction = Direction::SpanishToEnglish;
    };

    struct Session {
        std::vector<Card> deck;
        std::vector<Card> missedCards;
        Size position = 0;
        Size initialSize = 0;
        Size correct = 0;
        Size missed = 0;
        bool active = false;
        bool revealed = false;
        bool ended = false;

        bool finished() const { return active && (ended || position >= deck.size()); }

        const Card& current() const { return deck[position]; }

        Size remaining() const { return deck.size() - position; }
    };

}
