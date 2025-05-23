//console_ui.cpp
#define NOMINMAX
#include "console_ui.h"
#include <windows.h>
#include <iostream>
#include <limits>
#include <algorithm>

namespace UI {
    // Реализация функции инициализации консоли
    void initConsole() {
        // Устанавливаем кодировку UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        // Настраиваем буферы для поддержки UTF-8
        std::ios_base::sync_with_stdio(false);
        std::locale::global(std::locale(""));
        std::wcout.imbue(std::locale());
        std::wcin.imbue(std::locale());
    }

    void printColored(const std::string& text, int color) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);

        size_t start = 0;
        size_t end = text.find('\n');

        while (end != std::string::npos) {
            std::cout << text.substr(start, end - start) << std::endl;
            start = end + 1;
            end = text.find('\n', start);
        }

        std::cout << text.substr(start);
        SetConsoleTextAttribute(hConsole, 7);
    }

    void printMultiline(const std::string& text, int color, bool isActionDescription) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);

        size_t start = 0;
        size_t end = text.find('\\');

        if (end == std::string::npos) {
            if (isActionDescription && !text.empty()) {
                std::cout << "  " << text;
            }
            else if (!text.empty()) {
                std::cout << text;
            }
        }
        else {
            while (end != std::string::npos && end + 1 < text.length()) {
                std::string part = text.substr(start, end - start);
                if (isActionDescription && !part.empty()) {
                    std::cout << "  " << part;
                }
                else if (!part.empty()) {
                    std::cout << part;
                }

                if (text[end + 1] == 'n') {
                    std::cout << std::endl;
                    start = end + 2;
                }
                else {
                    start = end + 1;
                }

                end = text.find('\\', start);
            }

            std::string lastPart = text.substr(start);
            if (!lastPart.empty()) {
                if (isActionDescription) {
                    std::cout << "  " << lastPart;
                }
                else {
                    std::cout << lastPart;
                }
            }
        }

        SetConsoleTextAttribute(hConsole, 7);
        std::cout << std::endl;
    }

    void printRoomDescription(const Room& room) {
        printColored("\n=== " + room.name + " ===\n", 11); // Голубой цвет для названия
        printMultiline(room.description, 14); // Желтый цвет для описания
        std::cout << std::endl;
    }

    void printAvailableActions(Player& player,
        const std::vector<std::string>& actionIds,
        const std::map<std::string, Action>& actionMap) {
        printColored("\nВыберите действие:\n", 6); // Желтый заголовок

        for (size_t i = 0; i < actionIds.size(); ++i) {
            const std::string& actionId = actionIds[i];
            auto it = actionMap.find(actionId);

            if (it != actionMap.end()) {
                const Action& action = it->second;
                int successPercent = 0;
                std::string statName = action.requiredStat;

                if (statName == "СИЛ") successPercent = player.strength;
                else if (statName == "ЛОВ") successPercent = player.agility;
                else if (statName == "ИНТ") successPercent = player.intellect;
                else if (statName == "ВЫН") successPercent = player.endurance;

                int color = 7; // Белый по умолчанию
                if (statName == "СИЛ") color = 9; // Синий
                else if (statName == "ЛОВ") color = 10; // Зеленый
                else if (statName == "ИНТ") color = 11; // Голубой
                else if (statName == "ВЫН") color = 14; // Желтый

                std::cout << (i + 1) << ". ";
                printColored(action.description + " [", 7);
                printColored(statName + " " + std::to_string(successPercent) + "%", color);
                std::cout << "]" << std::endl;
            }
        }
    }

    void printPlayerStats(const Player& player) {
        printColored("\n=== Ваши характеристики ===\n", 11);
        std::cout << "Здоровье: " << player.health << "\n";
        std::cout << "Детали: " << player.shipParts << "/7\n";
        std::cout << "Флаконы силы: " << player.suddenStrengthPotions << "\n";
        std::cout << "Кристаллы жизни: " << player.lifeCrystals << "\n";

        printColored("\n=== Атрибуты ===\n", 11);
        std::cout << "СИЛ: " << player.strength << "%\n";
        std::cout << "ЛОВ: " << player.agility << "%\n";
        std::cout << "ИНТ: " << player.intellect << "%\n";
        std::cout << "ВЫН: " << player.endurance << "%\n";
    }

    void waitForEnter() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        static bool messageShown = false;
        if (!messageShown) {
            printColored("\nНажмите Enter чтобы продолжить...", 7);
            messageShown = true;
        }

        while (std::cin.get() != '\n') {
            // Ожидание нажатия Enter
        }
    }

    void showGameIntro() {
        printColored("Вы капитан космического корабля.\n", 15);
        printColored("Ваш корабль терпит крушение в системе Нексус на заброшенной космической станции «Эребус».\n\n", 15);
        printColored("Ваша задача — найти в отсеках станции 7 деталей для ремонта корабля и улететь.\n", 14);
        printColored("Опасность подстерегает Вас на каждом шагу! Исследуйте станцию. Находите аптечки,\n", 14);
        printColored("еду и воду для повышения здоровья. Артефакты повысят вашу устойчивость к стихиям,\n", 14);
        printColored("а зелья можно использовать во время боя. Каждая находка — это шанс выжить!\n\n", 14);
        printColored("На станции вас могут атаковать монстры с уникальными стихиями. Чтобы победить их, нужно продумать\n", 12);
        printColored("тактику. Вы можете применить оружие, использовать зелья или участвовать в ближнем бою.\n", 12);
        printColored("Помните: оружие наносит максимальный урон, если его стихия совпадает с уязвимостью монстра.\n", 12);
        printColored("Если вы атакуете монстра оружием его же стихии, урон будет нулевым. Оружие других стихий\n", 12);
        printColored("нанесет 4 HP урона. Продумывайте свои действия, чтобы выжить и спастись!\n\n", 12);
    }

    void showGameEnding(bool success) {
        if (success) {
            printColored("\nПОЗДРАВЛЯЕМ! Вы нашли все детали и починили корабль!\n", 10);
            printColored("Вы успешно улетаете со станции «Эребус»!\n", 10);
        }
        else {
            printColored("\nВы умерли! Игра окончена.\n", 12);
            printColored("Теперь ваши останки покоятся на заброшенной станции «Эребус». †††\n", 12);
        }
    }

    void showRoomActions(const std::vector<std::string>& actions) {
        printColored("\nДоступные действия:\n", 11);
        for (size_t i = 0; i < actions.size(); ++i) {
            std::cout << (i + 1) << ". ";
            printColored(actions[i], 15);
            std::cout << std::endl;
        }
    }

}