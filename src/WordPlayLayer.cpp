#include <Pch.hpp>

#include "WordPlayLayer.hpp"

namespace WordPlay {

    WordPlayLayer::WordPlayLayer(App::AppData& appData) :
    Layer(appData),
    m_CardGameLayout(appData) {
        loadSettings();
        attachPopups();

        m_AppData.registerLayout(m_CardGameLayout);
        m_AppData.setActiveLayout(m_CardGameLayout.getID());
    }

    WordPlayLayer::~WordPlayLayer() {
        saveSettings();
    }

    void WordPlayLayer::mainMenuBar(Core::Window &window, Core::Context &context) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    window.close();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void WordPlayLayer::loadSettings() {
        // Load settings from a file or other source
    }

    void WordPlayLayer::saveSettings() {
        // Save settings to a file or other destination
    }

    void WordPlayLayer::attachPopups() {
        // Attach any popups needed for the layer
    }

}
