#include <Pch.hpp>

#include "Widgets.hpp"
#include "../Model/Store.hpp"

#include <algorithm>

namespace WordPlay::Widgets {

    void CenteredText(const std::string& text, const float fontSize, const ImVec4* color) {
        const float available = ImGui::GetContentRegionAvail().x;

        ImGui::PushFont(nullptr, fontSize);
        const ImVec2 size = ImGui::CalcTextSize(text.c_str(), nullptr, false, available * 0.9f);
        const float startX = ImGui::GetCursorPosX() + std::max(0.0f, (available - size.x) * 0.5f);
        ImGui::SetCursorPosX(startX);
        ImGui::PushTextWrapPos(startX + size.x + 1.0f);
        if (color != nullptr) {
            ImGui::PushStyleColor(ImGuiCol_Text, *color);
        }
        ImGui::TextUnformatted(text.c_str());
        if (color != nullptr) {
            ImGui::PopStyleColor();
        }
        ImGui::PopTextWrapPos();
        ImGui::PopFont();
    }

    void openFolderDialog(Core::Window& window, Store& store) {
        Core::OpenFolderDesc desc;
        desc.title = "Select a folder with word lists";
        desc.defaultPath = store.folderPath;
        if (const auto folder = window.openFolderDialog(desc)) {
            store.addFolder(*folder);
        }
    }

    void openFilesDialog(Core::Window& window, Store& store) {
        Core::OpenFileDesc desc;
        desc.title = "Select word list files";
        desc.defaultPath = store.folderPath;
        desc.filterPatterns = {"*.txt"};
        desc.filterDescription = "Word lists (*.txt)";
        desc.allowMultipleSelects = true;
        const std::vector<fs::path> files = window.openFileDialog(desc);
        if (!files.empty()) {
            store.addFiles(files);
        }
    }

}
