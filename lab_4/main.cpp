#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <utility>
#include <limits>
#include <algorithm>

class SportsManager
{
public:
    /**
     * @brief Добавляет нового спортсмена и связывает его с видом спорта.
     */
    void addAthlete()
    {
        std::string currentAthleteName, currentSportName;
        std::cout << "Введите ФИО спортсмена: ";
        std::getline(std::cin, currentAthleteName);
        if (currentAthleteName.empty())
            throw std::invalid_argument("Ошибка: ФИО не может быть пустым.");
        if (findAthlete(currentAthleteName) != -1)
            throw std::runtime_error("Ошибка: Спортсмен уже существует.");

        std::cout << "Введите вид спорта: ";
        std::getline(std::cin, currentSportName);
        if (currentSportName.empty())
            throw std::invalid_argument("Ошибка: Название вида спорта не может быть пустым.");

        int sportIndex = getOrCreateSport(currentSportName);

        athleteNames.push_back(currentAthleteName);
        int athleteIndex = athleteNames.size() - 1;

        links.push_back({athleteIndex, sportIndex});
        std::cout << "=> Спортсмен '" << currentAthleteName << "' успешно добавлен." << std::endl;
    }

    /**
     * @brief Изменяет вид спорта для существующего спортсмена.
     */
    void changeAthleteSport()
    {
        std::string currentAthleteName, newSportName;
        std::cout << "Введите ФИО спортсмена: ";
        std::getline(std::cin, currentAthleteName);

        int athleteIndex = findAthlete(currentAthleteName);
        if (athleteIndex == -1)
            throw std::runtime_error("Ошибка: Спортсмен не найден.");

        std::cout << "Введите новое название вида спорта: ";
        std::getline(std::cin, newSportName);
        if (newSportName.empty())
            throw std::invalid_argument("Ошибка: Название не может быть пустым.");

        int newSportIndex = getOrCreateSport(newSportName);

        for (auto &link : links)
        {
            if (link.first == athleteIndex)
            {
                link.second = newSportIndex;
                break;
            }
        }
        std::cout << "=> Вид спорта для '" << currentAthleteName << "' изменен на '" << newSportName << "'." << std::endl;
    }

    /**
     * @brief Выводит на экран (без сортировки) список спортсменов для указанного вида спорта.
     */
    void displayAthletesBySport()
    {
        std::string currentSportName;
        std::cout << "Введите название вида спорта: ";
        std::getline(std::cin, currentSportName);

        int sportIndex = findSport(currentSportName);
        if (sportIndex == -1)
            throw std::runtime_error("Ошибка: Вид спорта не найден.");

        std::cout << "\n--- Спортсмены в виде спорта '" << currentSportName << "' ---" << std::endl;
        bool found = false;
        for (auto &link : links)
        {
            if (link.second == sportIndex)
            {
                std::cout << "- " << athleteNames[link.first] << std::endl;
                found = true;
            }
        }
        if (!found)
            std::cout << "В этом виде спорта нет спортсменов." << std::endl;
    }

    /**
     * @brief Выводит отсортированный по ФИО список всех спортсменов с их видами спорта.
     */
    void displayAllAthletesSorted()
    {
        if (links.empty())
        {
            std::cout << "Список спортсменов пуст." << std::endl;
            return;
        }

        auto sortedLinks = links;
        customQuickSortLinks(sortedLinks, 0, sortedLinks.size() - 1);

        std::cout << "\n--- Все спортсмены (отсортировано по ФИО) ---" << std::endl;
        for (auto &link : sortedLinks)
        {
            int athleteIdx = link.first;
            int sportIdx = link.second;
            std::cout << "- " << athleteNames[athleteIdx]
                      << " (" << sportNames[sportIdx] << ")" << std::endl;
        }
    }

    /**
     * @brief Выводит отсортированный по ФИО список спортсменов в указанном виде спорта.
     */
    void displayAthletesInSportSorted()
    {
        std::string currentSportName;
        std::cout << "Введите название вида спорта: ";
        std::getline(std::cin, currentSportName);

        int sportIndex = findSport(currentSportName);
        if (sportIndex == -1)
            throw std::runtime_error("Ошибка: Вид спорта не найден.");

        std::vector<std::pair<int, int>> filteredLinks;
        for (auto &link : links)
        {
            if (link.second == sportIndex)
            {
                filteredLinks.push_back(link);
            }
        }

        if (filteredLinks.empty())
        {
            std::cout << "В этом виде спорта нет спортсменов." << std::endl;
            return;
        }

        customQuickSortLinks(filteredLinks, 0, filteredLinks.size() - 1);

        std::cout << "\n--- Спортсмены в виде спорта '" << currentSportName << "' (отсортировано по ФИО) ---" << std::endl;
        for (auto &link : filteredLinks)
        {
            std::cout << "- " << athleteNames[link.first] << std::endl;
        }
    }

private:
    std::vector<std::string> athleteNames;
    std::vector<std::string> sportNames;

    // Хранит пары индексов, связывая `athleteNames` и `sportNames`.
    // - pair.first:  индекс спортсмена в векторе `athleteNames`.
    // - pair.second: индекс вида спорта в векторе `sportNames`.
    std::vector<std::pair<int, int>> links;

    /**
     * @brief Ищет индекс спортсмена по его ФИО.
     */
    int findAthlete(std::string &name)
    {
        for (int i = 0; i < athleteNames.size(); ++i)
        {
            if (athleteNames[i] == name)
                return i;
        }
        return -1;
    }

    /**
     * @brief Ищет индекс вида спорта по его названию.
     */
    int findSport(std::string &name)
    {
        for (int i = 0; i < sportNames.size(); ++i)
        {
            if (sportNames[i] == name)
                return i;
        }
        return -1;
    }

    /**
     * @brief Находит или создает новый вид спорта.
     */
    int getOrCreateSport(std::string &name)
    {
        int sportIndex = findSport(name);
        if (sportIndex == -1)
        {
            sportNames.push_back(name);
            sportIndex = sportNames.size() - 1;
            std::cout << "-> Добавлен новый вид спорта: " << name << std::endl;
        }
        return sportIndex;
    }

    /**
     * @brief Часть алгоритма Quick Sort.
     */
    int partitionLinks(std::vector<std::pair<int, int>> &linksToSort, int low, int high)
    {
        std::string pivot_name = athleteNames[linksToSort[high].first];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++)
        {
            if (athleteNames[linksToSort[j].first] <= pivot_name)
            {
                i++;
                std::swap(linksToSort[i], linksToSort[j]);
            }
        }
        std::swap(linksToSort[i + 1], linksToSort[high]);
        return (i + 1);
    }

    /**
     * @brief Собственная реализация Quick Sort.
     */
    void customQuickSortLinks(std::vector<std::pair<int, int>> &linksToSort, int low, int high)
    {
        if (low < high)
        {
            int pi = partitionLinks(linksToSort, low, high);
            customQuickSortLinks(linksToSort, low, pi - 1);
            customQuickSortLinks(linksToSort, pi + 1, high);
        }
    }
};

/**
 * @brief Отображает в консоли текстовое меню для пользователя.
 */
void printMenu()
{
    std::cout << "\n===== Меню =====\n"
              << "1. Добавить нового спортсмена\n"
              << "2. Изменить вид спорта для спортсмена\n"
              << "3. Показать спортсменов по виду спорта\n"
              << "4. Показать всех спортсменов (сортировка по ФИО)\n"
              << "5. Показать спортсменов в виде спорта (сортировка по ФИО)\n"
              << "0. Выход\n"
              << "Выберите опцию: ";
}

/**
 * @brief Главная функция программы. Точка входа.
 */
int main()
{
    setlocale(LC_ALL, "Russian");
    SportsManager manager;
    int choice;
    while (true)
    {
        printMenu();
        std::cin >> choice;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Ошибка: некорректный ввод.\n";
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (choice == 0)
            break;
        try
        {
            switch (choice)
            {
            case 1:
                manager.addAthlete();
                break;
            case 2:
                manager.changeAthleteSport();
                break;
            case 3:
                manager.displayAthletesBySport();
                break;
            case 4:
                manager.displayAllAthletesSorted();
                break;
            case 5:
                manager.displayAthletesInSportSorted();
                break;
            default:
                std::cout << "Неверный выбор.\n";
            }
        }
        catch (std::exception &e)
        {
            std::cerr << "Произошла ошибка: " << e.what() << std::endl;
        }
    }
    return 0;
}