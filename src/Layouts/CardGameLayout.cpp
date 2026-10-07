#include <Pch.hpp>
#include <Ids.hpp>

#include "CardGameLayout.hpp"

namespace WordPlay {

    CardGameLayout::CardGameLayout(App::AppData& appData) :
    Layout(WP_LAYOUT_CARD_GAME),
    m_AppData(appData) {

    }

    SP<App::Node> CardGameLayout::getLayout() {
        // TODO: Implement the layout for the card game
    }
}
