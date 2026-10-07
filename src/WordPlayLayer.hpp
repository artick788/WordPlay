#pragma once

#include "WordPlayInclude.hpp"
#include <VelyraAppFramework/Layer.hpp>

#include "Layouts/CardGameLayout.hpp"

namespace WordPlay {

    class WordPlayLayer : public App::Layer {
    public:
        explicit WordPlayLayer(App::AppData& app_data);

        ~WordPlayLayer() override;

        void mainMenuBar(Core::Window &window, Core::Context &context) override;

    private:

        void loadSettings();

        void saveSettings();

        void attachPopups();

    private:
        CardGameLayout m_CardGameLayout;
    };

}