#pragma once

#include <VelyraAppFramework/Widgets/Popup.hpp>

#include "../Model/Store.hpp"

namespace WordPlay {

    class StatsPopup : public App::Widgets::Popup {
    public:
        StatsPopup(App::AppData& appData, Store& store);

        ~StatsPopup() override = default;

    protected:
        void drawContent(Core::Window& window, Core::Context& context) override;

        void onOpen() override;

        void reset() override;

    private:
        void rebuildRows();

    private:
        struct Row {
            std::string key;
            int correct = 0;
            int missed = 0;
        };

        Store& m_Store;
        std::vector<Row> m_Rows;
        bool m_ConfirmReset = false;
    };

}
