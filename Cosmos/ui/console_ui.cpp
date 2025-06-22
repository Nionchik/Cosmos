#define NOMINMAX
#include <algorithm>
#include <iostream>
#include <limits>
#include <windows.h>

#include "console_ui.h"

namespace ui {

  void ClearScreen() {
#if defined(_WIN32)
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, 0 };
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;

    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    if (!FillConsoleOutputCharacter(hStdOut, ' ', cellCount, coord, &count))
      return;
    if (!FillConsoleOutputAttribute(hStdOut, csbi.wAttributes, cellCount,
      coord, &count))
      return;
    SetConsoleCursorPosition(hStdOut, coord);
#else
    std::cout << "\033[2J\033[1;1H";
#endif
  }

  void InitConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::ios_base::sync_with_stdio(false);
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    std::wcin.imbue(std::locale());
  }

  void PrintColored(const std::string& text, int color) {
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

  void PrintMultiline(const std::string& text, int color,
    bool is_action_description) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);

    size_t start = 0;
    size_t end = text.find('\\');

    if (end == std::string::npos) {
      if (is_action_description && !text.empty()) {
        std::cout << "  " << text;
      }
      else if (!text.empty()) {
        std::cout << text;
      }
    }
    else {
      while (end != std::string::npos && end + 1 < text.length()) {
        std::string part = text.substr(start, end - start);
        if (is_action_description && !part.empty()) {
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

      std::string last_part = text.substr(start);
      if (!last_part.empty()) {
        if (is_action_description) {
          std::cout << "  " << last_part;
        }
        else {
          std::cout << last_part;
        }
      }
    }

    SetConsoleTextAttribute(hConsole, 7);
    std::cout << std::endl;
  }

  void PrintRoomDescription(const Room& room) {
    PrintColored("\n=== " + room.name + " ===\n", 11);
    PrintMultiline(room.description, 14);
    std::cout << std::endl;
  }

  void PrintAvailableActions(Player& player,
    const std::vector<std::string>& action_ids,
    const std::map<std::string, Action>& action_map) {
    PrintColored("\nВыберите действие:\n", 6);

    for (size_t i = 0; i < action_ids.size(); ++i) {
      const std::string& action_id = action_ids[i];
      auto it = action_map.find(action_id);

      if (it != action_map.end()) {
        const Action& action = it->second;
        int success_percent = 0;
        std::string stat_name = action.required_stat;

        if (stat_name == "СИЛ") {
          success_percent = player.strength;
        }
        else if (stat_name == "ЛОВ") {
          success_percent = player.agility;
        }
        else if (stat_name == "ИНТ") {
          success_percent = player.intellect;
        }
        else if (stat_name == "ВЫН") {
          success_percent = player.endurance;
        }

        int color = 7;
        if (stat_name == "СИЛ") color = 9;
        else if (stat_name == "ЛОВ") color = 10;
        else if (stat_name == "ИНТ") color = 11;
        else if (stat_name == "ВЫН") color = 14;

        std::cout << (i + 1) << ". ";
        PrintColored(action.description + " [", 7);
        PrintColored(stat_name + " " + std::to_string(success_percent) + "%",
          color);
        std::cout << "]" << std::endl;
      }
    }
  }

  void PrintPlayerStats(const Player& player) {
    PrintColored("\n=== Ваши характеристики ===\n", 11);
    std::cout << "Здоровье: " << player.health << "\n";
    std::cout << "Детали: " << player.ship_parts << "/7\n";
    std::cout << "Флаконы силы: " << player.sudden_strength_potions << "\n";
    std::cout << "Кристаллы жизни: " << player.life_crystals << "\n";

    PrintColored("\n=== Атрибуты и сопротивления ===\n", 11);
    std::cout << "СИЛ: " << player.strength << "%";
    std::cout << "        Земля: " << player.earth_resistance << "%\n";

    std::cout << "ЛОВ: " << player.agility << "%";
    std::cout << "        Огонь: " << player.fire_resistance << "%\n";

    std::cout << "ИНТ: " << player.intellect << "%";
    std::cout << "        Яд: " << player.poison_resistance << "%\n";

    std::cout << "ВЫН: " << player.endurance << "%";
    std::cout << "        Вода: " << player.water_resistance << "%\n";
  }

  void WaitForEnter(const std::string& message) {
    if (!message.empty()) {
      std::cout << message;
    }
    std::string dummy;
    std::getline(std::cin, dummy);
  }

  void ShowGameIntro() {
    PrintColored("Вы капитан космического корабля.\n", 15);
    PrintColored("Ваш корабль терпит крушение в системе Нексус "
      "на заброшенной космической станции «Эребус».\n\n", 15);
    PrintColored("Ваша задача — найти в отсеках станции 7 деталей "
      "для ремонта корабля и улететь.\n", 14);
    PrintColored("Опасность подстерегает Вас на каждом шагу! "
      "Исследуйте станцию. Находите аптечки,\n", 14);
    PrintColored("еду и воду для повышения здоровья. Артефакты повысят "
      "вашу устойчивость к стихиям,\n", 14);
    PrintColored("а зелья можно использовать во время боя. "
      "Каждая находка — это шанс выжить!\n\n", 14);
    PrintColored("На станции вас могут атаковать монстры с уникальными "
      "стихиями. Чтобы победить их, нужно продумать\n", 12);
    PrintColored("тактику. Вы можете применить оружие, использовать зелья "
      "или участвовать в ближнем бою.\n", 12);
    PrintColored("Помните: оружие наносит максимальный урон, если его стихия "
      "совпадает с уязвимостью монстра.\n", 12);
    PrintColored("Если вы атакуете монстра оружием его же стихии, "
      "урон будет нулевым. Оружие других стихий\n", 12);
    PrintColored("нанесет 4 HP урона. Продумывайте свои действия, "
      "чтобы выжить и спастись!\n\n", 12);
  }

  void ShowGameEnding(bool success) {
    if (success) {
      PrintColored("\nПОЗДРАВЛЯЕМ! Вы нашли все детали и починили "
        "корабль!\n", 10);
      PrintColored("Вы успешно улетаете со станции «Эребус»!\n", 10);
      ui::WaitForEnter("Нажмите Enter ...");
    }
    else {
      PrintColored("\nВы умерли! Игра окончена.\n", 12);
      PrintColored("Теперь ваши останки покоятся "
        "на заброшенной станции «Эребус». †††\n", 12);
      ui::WaitForEnter("Нажмите Enter ...");
    }
  }

  void ShowRoomActions(const std::vector<std::string>& actions) {
    PrintColored("\nДоступные действия:\n", 11);
    for (size_t i = 0; i < actions.size(); ++i) {
      std::cout << (i + 1) << ". ";
      PrintColored(actions[i], 15);
      std::cout << std::endl;
    }
  }
}