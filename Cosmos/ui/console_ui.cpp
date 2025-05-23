//console_ui.cpp
#define NOMINMAX
#include "console_ui.h"
#include <windows.h>
#include <iostream>
#include <limits>
#include <algorithm>

namespace UI {
    // ���������� ������� ������������� �������
    void initConsole() {
        // ������������� ��������� UTF-8
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);

        // ����������� ������ ��� ��������� UTF-8
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
        printColored("\n=== " + room.name + " ===\n", 11); // ������� ���� ��� ��������
        printMultiline(room.description, 14); // ������ ���� ��� ��������
        std::cout << std::endl;
    }

    void printAvailableActions(Player& player,
        const std::vector<std::string>& actionIds,
        const std::map<std::string, Action>& actionMap) {
        printColored("\n�������� ��������:\n", 6); // ������ ���������

        for (size_t i = 0; i < actionIds.size(); ++i) {
            const std::string& actionId = actionIds[i];
            auto it = actionMap.find(actionId);

            if (it != actionMap.end()) {
                const Action& action = it->second;
                int successPercent = 0;
                std::string statName = action.requiredStat;

                if (statName == "���") successPercent = player.strength;
                else if (statName == "���") successPercent = player.agility;
                else if (statName == "���") successPercent = player.intellect;
                else if (statName == "���") successPercent = player.endurance;

                int color = 7; // ����� �� ���������
                if (statName == "���") color = 9; // �����
                else if (statName == "���") color = 10; // �������
                else if (statName == "���") color = 11; // �������
                else if (statName == "���") color = 14; // ������

                std::cout << (i + 1) << ". ";
                printColored(action.description + " [", 7);
                printColored(statName + " " + std::to_string(successPercent) + "%", color);
                std::cout << "]" << std::endl;
            }
        }
    }

    void printPlayerStats(const Player& player) {
        printColored("\n=== ���� �������������� ===\n", 11);
        std::cout << "��������: " << player.health << "\n";
        std::cout << "������: " << player.shipParts << "/7\n";
        std::cout << "������� ����: " << player.suddenStrengthPotions << "\n";
        std::cout << "��������� �����: " << player.lifeCrystals << "\n";

        printColored("\n=== �������� ===\n", 11);
        std::cout << "���: " << player.strength << "%\n";
        std::cout << "���: " << player.agility << "%\n";
        std::cout << "���: " << player.intellect << "%\n";
        std::cout << "���: " << player.endurance << "%\n";
    }

    void waitForEnter() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        static bool messageShown = false;
        if (!messageShown) {
            printColored("\n������� Enter ����� ����������...", 7);
            messageShown = true;
        }

        while (std::cin.get() != '\n') {
            // �������� ������� Enter
        }
    }

    void showGameIntro() {
        printColored("�� ������� ������������ �������.\n", 15);
        printColored("��� ������� ������ �������� � ������� ������ �� ����������� ����������� ������� �������.\n\n", 15);
        printColored("���� ������ � ����� � ������� ������� 7 ������� ��� ������� ������� � �������.\n", 14);
        printColored("��������� ������������ ��� �� ������ ����! ���������� �������. �������� �������,\n", 14);
        printColored("��� � ���� ��� ��������� ��������. ��������� ������� ���� ������������ � �������,\n", 14);
        printColored("� ����� ����� ������������ �� ����� ���. ������ ������� � ��� ���� ������!\n\n", 14);
        printColored("�� ������� ��� ����� ��������� ������� � ����������� ��������. ����� �������� ��, ����� ���������\n", 12);
        printColored("�������. �� ������ ��������� ������, ������������ ����� ��� ����������� � ������� ���.\n", 12);
        printColored("�������: ������ ������� ������������ ����, ���� ��� ������ ��������� � ����������� �������.\n", 12);
        printColored("���� �� �������� ������� ������� ��� �� ������, ���� ����� �������. ������ ������ ������\n", 12);
        printColored("������� 4 HP �����. ������������ ���� ��������, ����� ������ � ��������!\n\n", 12);
    }

    void showGameEnding(bool success) {
        if (success) {
            printColored("\n�����������! �� ����� ��� ������ � �������� �������!\n", 10);
            printColored("�� ������� �������� �� ������� �������!\n", 10);
        }
        else {
            printColored("\n�� ������! ���� ��������.\n", 12);
            printColored("������ ���� ������� �������� �� ����������� ������� �������. ���\n", 12);
        }
    }

    void showRoomActions(const std::vector<std::string>& actions) {
        printColored("\n��������� ��������:\n", 11);
        for (size_t i = 0; i < actions.size(); ++i) {
            std::cout << (i + 1) << ". ";
            printColored(actions[i], 15);
            std::cout << std::endl;
        }
    }

}