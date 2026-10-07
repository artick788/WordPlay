#pragma once

#include <VelyraUtils/VelyraEnum.hpp>

#include <VelyraAppFramework/LayoutEngine/Layout.hpp>
#include <VelyraAppFramework/Widgets/Popup.hpp>

VL_ENUM(WP_LAYOUT, Velyra::App::LayoutID,
    WP_LAYOUT_CARD_GAME = 1
);

VL_ENUM(WP_POPUP, Velyra::App::Widgets::PopupID,
    WP_POPUP_STATS = 1
);
