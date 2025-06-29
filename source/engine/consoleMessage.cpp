#include "ginkgopch.h"
#include "consolePanel.h"

std::vector<std::string> ConsolePanel::consoleMessages;

void AddConsoleMessage(const std::string& message) {
    if (ConsolePanel::consoleMessages.size() > 100) {
        ConsolePanel::consoleMessages.erase(ConsolePanel::consoleMessages.begin());
    }
    ConsolePanel::consoleMessages.push_back(message);
}