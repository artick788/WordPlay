#include <Pch.hpp>
#include <Ids.hpp>

#include "StatsPopup.hpp"

#include <algorithm>

namespace WordPlay {

    StatsPopup::StatsPopup(App::AppData& appData, Store& store) :
    Popup(appData, "Statistics", WP_POPUP_STATS),
    m_Store(store) {

    }

    void StatsPopup::onOpen() {
        rebuildRows();
        m_ConfirmReset = false;
    }

    void StatsPopup::reset() {
        m_ConfirmReset = false;
    }

    void StatsPopup::rebuildRows() {
        m_Rows.clear();
        for (const auto& [key, stats] : m_Store.stats) {
            m_Rows.push_back({key, stats.correct, stats.missed});
        }
        // Hardest words first
        std::ranges::sort(m_Rows, [](const Row& a, const Row& b) {
            const int scoreA = a.missed - a.correct;
            const int scoreB = b.missed - b.correct;
            if (scoreA != scoreB) return scoreA > scoreB;
            if (a.missed != b.missed) return a.missed > b.missed;
            return a.key < b.key;
        });
    }

    void StatsPopup::drawContent(Core::Window&, Core::Context&) {
        ImGui::Text("%zu practiced word(s), hardest first", m_Rows.size());

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
                                          ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable;
        if (ImGui::BeginTable("##Stats", 4, flags, ImVec2(640.0f, 360.0f))) {
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableSetupColumn("Word", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Correct", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Missed", ImGuiTableColumnFlags_WidthFixed, 70.0f);
            ImGui::TableSetupColumn("Accuracy", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(m_Rows.size()));
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    const Row& row = m_Rows[static_cast<Size>(i)];
                    const int total = row.correct + row.missed;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(row.key.c_str());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::Text("%d", row.correct);
                    ImGui::TableSetColumnIndex(2);
                    ImGui::Text("%d", row.missed);
                    ImGui::TableSetColumnIndex(3);
                    ImGui::Text("%d%%", total > 0 ? 100 * row.correct / total : 0);
                }
            }
            ImGui::EndTable();
        }

        ImGui::Checkbox("Confirm reset", &m_ConfirmReset);
        ImGui::SameLine();
        ImGui::BeginDisabled(!m_ConfirmReset);
        ImGui::PushStyleColor(ImGuiCol_Button, App::Styles::ColorRed);
        if (ImGui::Button("Reset statistics")) {
            m_Store.stats.clear();
            m_Rows.clear();
            m_ConfirmReset = false;
        }
        ImGui::PopStyleColor();
        ImGui::EndDisabled();

        ImGui::SameLine();
        if (ImGui::Button("Close", ImVec2(100.0f, App::Styles::ButtonHeight))) {
            reset();
            setOpen(false);
        }
    }

}
