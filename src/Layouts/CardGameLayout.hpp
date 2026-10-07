#pragma once

#include <VelyraAppFramework/LayoutEngine/Layout.hpp>

#include "../Model/Store.hpp"

namespace WordPlay {

    class CardGameLayout: public App::Layout {
    public:
        CardGameLayout(App::AppData& appData, Store& store);

        ~CardGameLayout() override = default;

        SP<App::Node> getLayout() override;

    private:
        void drawWordLists(Core::Window& window, Core::Context& context);

        void drawGameSetup(Core::Window& window, Core::Context& context);

        void drawPractice(Core::Window& window, Core::Context& context);

        void handleShortcuts();

        void drawIdle() const;

        void drawCard();

        void drawSummary();

    private:
        App::AppData& m_AppData;
        Store& m_Store;
    };

}