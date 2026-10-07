#pragma once

#include "WordPlayInclude.hpp"
#include <VelyraAppFramework/Layer.hpp>

#include "Layouts/CardGameLayout.hpp"
#include "Model/Store.hpp"
#include "Popups/StatsPopup.hpp"

namespace WordPlay {

    class WordPlayLayer : public App::Layer {
    public:
        explicit WordPlayLayer(App::AppData& app_data);

        ~WordPlayLayer() override;

        void mainMenuBar(Core::Window &window, Core::Context &context) override;

    private:

        void loadSettings();

        void loadProgramArgs();

        void saveSettings();

        void attachPopups();

    private:
        Store m_Store;
        SP<StatsPopup> m_StatsPopup;
        CardGameLayout m_CardGameLayout;
    };

}