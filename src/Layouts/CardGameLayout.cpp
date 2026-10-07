#include <Pch.hpp>
#include <Ids.hpp>

#include "CardGameLayout.hpp"
#include "../Widgets/Widgets.hpp"

#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <optional>

namespace WordPlay {

    namespace {

        constexpr float ActionButtonHeight = 45.0f;

        bool centeredButton(const char* label, const float width, const ImVec4* color = nullptr, const bool enabled = true) {
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + std::max(0.0f, (ImGui::GetContentRegionAvail().x - width) * 0.5f));
            ImGui::BeginDisabled(!enabled);
            if (color != nullptr) {
                ImGui::PushStyleColor(ImGuiCol_Button, *color);
            }
            const bool pressed = ImGui::Button(label, ImVec2(width, ActionButtonHeight * 0.8f));
            if (color != nullptr) {
                ImGui::PopStyleColor();
            }
            ImGui::EndDisabled();
            return pressed;
        }

    }

    CardGameLayout::CardGameLayout(App::AppData& appData, Store& store) :
    Layout(WP_LAYOUT_CARD_GAME),
    m_AppData(appData),
    m_Store(store) {

    }

    SP<App::Node> CardGameLayout::getLayout() {
        using namespace Velyra::App;

        return createLayout(
            horizontalSplit(
                verticalSplit(
                    createPanel({
                        .name = "Word Lists",
                        .drawFunction = bindDraw(this, &CardGameLayout::drawWordLists),
                        .sizeRatio = 0.6f
                    }),
                    createPanel({
                        .name = "Game Setup",
                        .drawFunction = bindDraw(this, &CardGameLayout::drawGameSetup),
                        .sizeRatio = 0.4f
                    })
                ),
                createPanel({
                    .name = "Practice",
                    .drawFunction = bindDraw(this, &CardGameLayout::drawPractice),
                    .sizeRatio = 0.72f
                })
            )
        );
    }

    void CardGameLayout::drawWordLists(Core::Window& window, Core::Context&) {
        if (ImGui::Button("Open Folder...")) {
            Widgets::openFolderDialog(window, m_Store);
        }
        ImGui::SameLine();
        if (ImGui::Button("Open Files...")) {
            Widgets::openFilesDialog(window, m_Store);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reload")) {
            m_Store.reloadAll();
        }
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Re-read all files and rescan the folder for new ones");
        }

        if (!m_Store.statusMessage.empty()) {
            ImGui::TextColored(App::Styles::ColorYellow, "%s", m_Store.statusMessage.c_str());
        }
        ImGui::Separator();

        if (m_Store.lists.empty()) {
            ImGui::TextWrapped("Open a folder or files with lines like \"Hola = Hello\".");
            return;
        }

        if (ImGui::SmallButton("Select all")) {
            m_Store.setAllSelected(true);
        }
        ImGui::SameLine();
        if (ImGui::SmallButton("Select none")) {
            m_Store.setAllSelected(false);
        }

        std::optional<Size> toRemove;
        if (ImGui::BeginChild("##ListEntries")) {
            for (Size i = 0; i < m_Store.lists.size(); ++i) {
                WordList& list = m_Store.lists[i];
                ImGui::PushID(static_cast<int>(i));

                const std::string label = list.name + " (" + std::to_string(list.entries.size()) + ")";
                ImGui::Checkbox(label.c_str(), &list.selected);
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s", list.path.string().c_str());
                }

                if (list.skippedLines > 0) {
                    ImGui::SameLine();
                    ImGui::TextColored(App::Styles::ColorYellow, "!");
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%zu line(s) skipped: expected \"spanish = english\"", list.skippedLines);
                    }
                }

                ImGui::SameLine();
                if (ImGui::SmallButton("Remove")) {
                    toRemove = i;
                }
                ImGui::PopID();
            }
        }
        ImGui::EndChild();

        if (toRemove) {
            m_Store.removeList(*toRemove);
        }
    }

    void CardGameLayout::drawGameSetup(Core::Window&, Core::Context&) {
        GameConfig& config = m_Store.config;

        ImGui::SeparatorText("Mode");
        ImGui::RadioButton("Spanish -> English", &config.mode, static_cast<int>(GameMode::SpanishToEnglish));
        ImGui::RadioButton("English -> Spanish", &config.mode, static_cast<int>(GameMode::EnglishToSpanish));
        ImGui::RadioButton("Both directions", &config.mode, static_cast<int>(GameMode::Mixed));

        ImGui::SeparatorText("Options");
        ImGui::Checkbox("Shuffle", &config.shuffle);
        ImGui::Checkbox("Hard words only", &config.hardOnly);
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Only words you missed at least as often as you got right");
        }

        ImGui::Separator();
        const Size cards = m_Store.countCards();
        ImGui::Text("%zu card(s) from %zu selected list(s)", cards, m_Store.selectedCount());

        ImGui::BeginDisabled(cards == 0);
        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
        if (ImGui::Button(m_Store.session.active ? "Restart" : "Start", ImVec2(-FLT_MIN, ActionButtonHeight * 0.8f))) {
            m_Store.startSession();
        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();
    }

    void CardGameLayout::drawPractice(Core::Window&, Core::Context&) {
        handleShortcuts();

        const Session& session = m_Store.session;
        if (!session.active) {
            drawIdle();
        } else if (session.finished()) {
            drawSummary();
        } else {
            drawCard();
        }
    }

    void CardGameLayout::handleShortcuts() {
        const Session& session = m_Store.session;
        if (!session.active || session.finished() || ImGui::GetIO().WantTextInput) {
            return;
        }
        if (!session.revealed) {
            if (ImGui::IsKeyPressed(ImGuiKey_Space, false)) {
                m_Store.reveal();
            }
        } else if (ImGui::IsKeyPressed(ImGuiKey_RightArrow, false)) {
            m_Store.answer(true);
        } else if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow, false)) {
            m_Store.answer(false);
        }
    }

    void CardGameLayout::drawIdle() const {
        const float baseSize = ImGui::GetStyle().FontSizeBase;
        ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y * 0.35f);
        const ImVec4 dim = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
        if (m_Store.lists.empty()) {
            Widgets::CenteredText("Load your word lists to get started", baseSize * 1.8f, &dim);
        } else {
            Widgets::CenteredText("Select lists, pick a mode and press Start", baseSize * 1.8f, &dim);
        }
    }

    void CardGameLayout::drawCard() {
        const Session& session = m_Store.session;
        const Card& card = session.current();
        const ImGuiStyle& style = ImGui::GetStyle();
        const float baseSize = style.FontSizeBase;

        // Progress header
        char overlay[96];
        std::snprintf(overlay, sizeof(overlay), "Correct %zu / %zu   Missed %zu",
                      session.correct, session.initialSize, session.missed);
        const float progress = session.initialSize > 0
            ? static_cast<float>(session.correct) / static_cast<float>(session.initialSize) : 0.0f;
        const float endWidth = ImGui::CalcTextSize("End session").x + style.FramePadding.x * 2.0f;
        ImGui::ProgressBar(progress, ImVec2(ImGui::GetContentRegionAvail().x - endWidth - style.ItemSpacing.x, 0.0f), overlay);
        ImGui::SameLine();
        if (ImGui::Button("End session")) {
            m_Store.endSession();
            return;
        }

        const ImVec4 dim = ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled);
        const float childHeight = std::max(80.0f, ImGui::GetContentRegionAvail().y - ActionButtonHeight - style.ItemSpacing.y);
        if (ImGui::BeginChild("##Card", ImVec2(0.0f, childHeight), ImGuiChildFlags_Borders)) {
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() * 0.15f);
            const char* directionLabel = card.direction == Direction::SpanishToEnglish
                ? "Spanish -> English" : "English -> Spanish";
            Widgets::CenteredText(directionLabel, baseSize * 1.2f, &dim);
            ImGui::Dummy(ImVec2(0.0f, baseSize * 1.5f));

            Widgets::CenteredText(card.prompt, baseSize * 3.0f);
            ImGui::Dummy(ImVec2(0.0f, baseSize * 2.0f));

            if (session.revealed) {
                Widgets::CenteredText(card.answer, baseSize * 3.0f, &App::Styles::ColorGreen);
            } else {
                Widgets::CenteredText("Say the translation, then reveal it", baseSize * 1.2f, &dim);
            }
        }
        ImGui::EndChild();

        const float spacing = style.ItemSpacing.x;
        const float fullWidth = ImGui::GetContentRegionAvail().x;
        if (!session.revealed) {
            if (ImGui::Button("Reveal (Space)", ImVec2(fullWidth, ActionButtonHeight))) {
                m_Store.reveal();
            }
        } else {
            const float halfWidth = (fullWidth - spacing) * 0.5f;
            ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
            if (ImGui::Button("Missed it (Left)", ImVec2(halfWidth, ActionButtonHeight))) {
                m_Store.answer(false);
            }
            ImGui::PopStyleColor();
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorGreen);
            if (ImGui::Button("Got it (Right)", ImVec2(halfWidth, ActionButtonHeight))) {
                m_Store.answer(true);
            }
            ImGui::PopStyleColor();
        }
    }

    void CardGameLayout::drawSummary() {
        const Session& session = m_Store.session;
        const float baseSize = ImGui::GetStyle().FontSizeBase;
        const Size answered = session.correct + session.missed;
        const Size accuracy = answered > 0 ? 100 * session.correct / answered : 0;

        ImGui::Dummy(ImVec2(0.0f, ImGui::GetContentRegionAvail().y * 0.05f));
        Widgets::CenteredText(session.ended ? "Session ended" : "Session complete", baseSize * 2.5f);
        ImGui::Dummy(ImVec2(0.0f, baseSize));

        char line[96];
        std::snprintf(line, sizeof(line), "Correct: %zu    Missed: %zu    Accuracy: %zu%%",
                      session.correct, session.missed, accuracy);
        Widgets::CenteredText(line, baseSize * 1.5f);
        ImGui::Dummy(ImVec2(0.0f, baseSize * 1.5f));

        constexpr float buttonWidth = 280.0f;
        if (centeredButton("Restart", buttonWidth, &App::Styles::ColorGreen)) {
            m_Store.startSession();
            return;
        }
        const std::string missedLabel = "Practice missed words (" + std::to_string(session.missedCards.size()) + ")";
        if (centeredButton(missedLabel.c_str(), buttonWidth, nullptr, !session.missedCards.empty())) {
            m_Store.practiceMissed();
            return;
        }
        if (centeredButton("Back to setup", buttonWidth)) {
            m_Store.closeSession();
            return;
        }

        if (!session.missedCards.empty()) {
            ImGui::SeparatorText("Missed words");
            if (ImGui::BeginChild("##MissedWords")) {
                for (const Card& card : session.missedCards) {
                    ImGui::BulletText("%s = %s",
                        card.direction == Direction::SpanishToEnglish ? card.prompt.c_str() : card.answer.c_str(),
                        card.direction == Direction::SpanishToEnglish ? card.answer.c_str() : card.prompt.c_str());
                }
            }
            ImGui::EndChild();
        }
    }
}
