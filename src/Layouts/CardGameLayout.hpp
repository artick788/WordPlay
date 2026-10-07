#pragma once

#include <VelyraAppFramework/LayoutEngine/Layout.hpp>

namespace WordPlay {

    class CardGameLayout: public App::Layout {
    public:
        explicit CardGameLayout(App::AppData& appData);

        ~CardGameLayout() override = default;

        SP<App::Node> getLayout() override;

    private:
        App::AppData& m_AppData;
    };

}