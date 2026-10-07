#include <Pch.hpp>

#include "WordPlayLayer.hpp"
#include "Widgets/Widgets.hpp"

namespace WordPlay {

    WordPlayLayer::WordPlayLayer(App::AppData& appData) :
    Layer(appData),
    m_CardGameLayout(appData, m_Store) {
        loadSettings();
        loadProgramArgs();
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
                if (ImGui::MenuItem("Open Folder...")) {
                    Widgets::openFolderDialog(window, m_Store);
                }
                if (ImGui::MenuItem("Open Files...")) {
                    Widgets::openFilesDialog(window, m_Store);
                }
                if (ImGui::MenuItem("Reload")) {
                    m_Store.reloadAll();
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) {
                    window.close();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("View")) {
                if (ImGui::MenuItem("Statistics...")) {
                    m_StatsPopup->setOpen(true);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }

    void WordPlayLayer::loadSettings() {
        m_Store.restore(m_AppData.settings.getSetting<StoreState>("Store"));
    }

    void WordPlayLayer::loadProgramArgs() {
        const App::ProgramArgs& args = m_AppData.programArgs;
        if (args.size() < 2) {
            return; // args[0] is the executable
        }

        // Launching with paths practices only those, so deselect what the saved settings restored.
        m_Store.setAllSelected(false);
        std::vector<fs::path> files;
        for (Size i = 1; i < args.size(); ++i) {
            const fs::path path{std::string(args[i])};
            std::error_code ec;
            if (fs::is_directory(path, ec)) {
                m_Store.addFolder(path);
            } else {
                files.push_back(path);
            }
        }
        if (!files.empty()) {
            m_Store.addFiles(files);
        }
    }

    void WordPlayLayer::saveSettings() {
        m_AppData.settings.setSetting("Store", m_Store.snapshot());
    }

    void WordPlayLayer::attachPopups() {
        m_StatsPopup = createSP<StatsPopup>(m_AppData, m_Store);
        m_AppData.addPopup(m_StatsPopup);
    }

}
