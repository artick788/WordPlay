#include "src/WordPlayLayer.hpp"
#include <iostream>
#include <VelyraAppFramework/Application.hpp>

int main(const int argc, const char* argv[]) {
    using namespace WordPlay;


    const App::ProgramArgs args(argv, argv + argc);
    try {
        App::ApplicationDesc desc;
        desc.applicationName = "WordPlay";
        desc.settingsEnableSave = true;
        desc.useImPlot = true;
        desc.saveImGuiWindowData = false; // We don't want imgui.ini everywhere
        App::Application app(desc, args);
        app.createAppLayer<WordPlayLayer>();
        app.run();

    } catch (const std::exception& e) {
        std::cerr << "Application failed to start: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

