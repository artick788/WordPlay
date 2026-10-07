#pragma once

#include "../WordPlayInclude.hpp"

#include <string>

struct ImVec4;

namespace WordPlay {
    struct Store;
}

namespace WordPlay::Widgets {

    // Draws wrapped text horizontally centered in the current content region.
    void CenteredText(const std::string& text, float fontSize, const ImVec4* color = nullptr);

    void openFolderDialog(Core::Window& window, Store& store);

    void openFilesDialog(Core::Window& window, Store& store);

}
