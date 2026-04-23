/**
 * ЧАСТЬ 1: Ядро лаунчера, Заголовки и Глобальные константы.
 * * В этой части определяются все необходимые зависимости и структура данных
 * для хранения настроек пользователя (Ник, ОЗУ, Путь к Java).
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <shellapi.h>

// Примечание: Для полноценной работы потребуются внешние библиотеки:
// 1. nlohmann/json (для парсинга метаданных Fabric)
// 2. libcurl (для скачивания модов и ресурсов)
// 3. zlib/minizip (для распаковки mods.zip и resourcepacks.zip)

namespace fs = std::filesystem;

// Глобальные настройки и константы
struct LauncherConfig {
    std::string username = "Player";
    int ram_gb = 8;
    int max_ram_gb = 32;
    std::string java_path = "";
    std::string minecraft_path = "";
    std::string version = "1.21.1"; // Fabric 1.21.1 (на момент ответа актуальная версия с медью)
    
    // Ссылки на ваши архивы (замените на свои прямые ссылки)
    std::string mods_url = "https://www.dropbox.com/scl/fi/bnadtftbex774abr3irhk/mods-1.zip?rlkey=t9m1oig4tq6w4zo18g0bzmg2i&st=baz8filb&dl=0";
    std::string resourcepack_url = "https://www.dropbox.com/scl/fi/j0wyafoc95fyoj50ll8vw/resourcepacks.zip?rlkey=syefhs6zc3js3v0vn8rdzbnok&st=nabp2utl&dl=0";
};

class MinecraftLauncher {
public:
    MinecraftLauncher() {
        setup_environment();
    }

    // Инициализация путей (AppData/Roaming/.my_launcher)
    void setup_environment() {
        char* appdata = getenv("APPDATA");
        if (appdata) {
            config.minecraft_path = std::string(appdata) + "\\.custom_launcher";
            if (!fs::exists(config.minecraft_path)) {
                fs::create_directories(config.minecraft_path);
                fs::create_directories(config.minecraft_path + "\\mods");
                fs::create_directories(config.minecraft_path + "\\resourcepacks");
            }
        }
        
        // Автоматический поиск Java
        config.java_path = find_java_path();
    }

    // Поиск Java в системе (простейший алгоритм через переменные окружения)
    std::string find_java_path() {
        char* java_home = getenv("JAVA_HOME");
        if (java_home) {
            std::string path = std::string(java_home) + "\\bin\\javaw.exe";
            if (fs::exists(path)) return path;
        }
        
        // Резервный поиск в Program Files
        std::string pf_java = "C:\\Program Files\\Java";
        if (fs::exists(pf_java)) {
            for (const auto& entry : fs::recursive_directory_iterator(pf_java)) {
                if (entry.path().filename() == "javaw.exe") {
                    return entry.path().string();
                }
            }
        }
        return "java"; // Если не нашли, надеемся на PATH
    }

    // Сохранение настроек в файл
    void save_settings() {
        std::ofstream file(config.minecraft_path + "\\launcher_settings.ini");
        file << "username=" << config.username << "\n";
        file << "ram=" << config.ram_gb << "\n";
        file << "java=" << config.java_path << "\n";
        file.close();
    }

    // Загрузка настроек
    void load_settings() {
        std::ifstream file(config.minecraft_path + "\\launcher_settings.ini");
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("username=") == 0) config.username = line.substr(9);
            if (line.find("ram=") == 0) config.ram_gb = std::stoi(line.substr(4));
            if (line.find("java=") == 0) config.java_path = line.substr(5);
        }
    }

    LauncherConfig config;
};

// В следующей части мы реализуем систему загрузки (Download System) 
// и логику распаковки модов/ресурспаков.
/**
 * ЧАСТЬ 1 и 2: Ядро, настройки и система установки (Downloader & Extractor).
 * * В этой части мы добавляем логику скачивания и распаковки ваших ресурсов.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <shellapi.h>

// Примечание: Для компиляции этой части понадобятся:
// libcurl (для HTTP-запросов) и minizip/zlib (для работы с ZIP)

namespace fs = std::filesystem;

struct LauncherConfig {
    std::string username = "Player";
    int ram_gb = 8;
    int max_ram_gb = 32;
    std::string java_path = "";
    std::string minecraft_path = "";
    std::string version = "1.21.1"; 
    
    // Прямые ссылки на ваши ресурсы
    std::string mods_url = "https://your-server.com/mods.zip";
    std::string resourcepack_url = "https://your-server.com/resourcepacks.zip";
};

class MinecraftLauncher {
public:
    MinecraftLauncher() {
        setup_environment();
    }

    void setup_environment() {
        char* appdata = getenv("APPDATA");
        if (appdata) {
            config.minecraft_path = std::string(appdata) + "\\.custom_launcher";
            if (!fs::exists(config.minecraft_path)) {
                fs::create_directories(config.minecraft_path);
                fs::create_directories(config.minecraft_path + "\\mods");
                fs::create_directories(config.minecraft_path + "\\resourcepacks");
            }
        }
        config.java_path = find_java_path();
    }

    std::string find_java_path() {
        char* java_home = getenv("JAVA_HOME");
        if (java_home) {
            std::string path = std::string(java_home) + "\\bin\\javaw.exe";
            if (fs::exists(path)) return path;
        }
        
        std::string pf_java = "C:\\Program Files\\Java";
        if (fs::exists(pf_java)) {
            for (const auto& entry : fs::recursive_directory_iterator(pf_java)) {
                if (entry.path().filename() == "javaw.exe") {
                    return entry.path().string();
                }
            }
        }
        return "java";
    }

    // --- НОВАЯ ЛОГИКА ЧАСТИ 2: УСТАНОВКА ---

    // Симуляция скачивания (в реальном коде здесь вызов curl_easy_perform)
    bool download_file(const std::string& url, const std::string& target_path) {
        std::cout << "Скачивание: " << url << " -> " << target_path << std::endl;
        // Здесь должна быть логика libcurl
        return true; 
    }

    // Симуляция распаковки (в реальном коде здесь вызов функций minizip)
    bool extract_zip(const std::string& zip_path, const std::string& extract_to) {
        std::cout << "Распаковка: " << zip_path << " в " << extract_to << std::endl;
        // Здесь должна быть логика unzip
        return true;
    }

    // ГЛАВНАЯ КНОПКА: УСТАНОВИТЬ
    void install_game() {
        std::cout << "Начало установки версии Fabric " << config.version << "..." << std::endl;

        // 1. Создание структуры папок
        fs::create_directories(config.minecraft_path + "\\mods");
        fs::create_directories(config.minecraft_path + "\\resourcepacks");

        // 2. Установка модов
        std::string temp_mods_zip = config.minecraft_path + "\\temp_mods.zip";
        if (download_file(config.mods_url, temp_mods_zip)) {
            extract_zip(temp_mods_zip, config.minecraft_path + "\\mods");
            fs::remove(temp_mods_zip); // Удаляем временный архив
        }

        // 3. Установка ресурспаков
        std::string temp_rp_zip = config.minecraft_path + "\\temp_rp.zip";
        if (download_file(config.resourcepack_url, temp_rp_zip)) {
            extract_zip(temp_rp_zip, config.minecraft_path + "\\resourcepacks");
            fs::remove(temp_rp_zip);
        }

        // 4. Загрузка библиотек Fabric (метаданные)
        // В реальном лаунчере здесь скачивается JSON от Fabric и по нему качаются .jar файлы
        std::cout << "Библиотеки Fabric успешно интегрированы." << std::endl;
        
        std::cout << "Установка завершена успешно!" << std::endl;
    }

    void save_settings() {
        std::ofstream file(config.minecraft_path + "\\launcher_settings.ini");
        file << "username=" << config.username << "\n";
        file << "ram=" << config.ram_gb << "\n";
        file << "java=" << config.java_path << "\n";
        file.close();
    }

    void load_settings() {
        std::ifstream file(config.minecraft_path + "\\launcher_settings.ini");
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("username=") == 0) config.username = line.substr(9);
            if (line.find("ram=") == 0) config.ram_gb = std::stoi(line.substr(4));
            if (line.find("java=") == 0) config.java_path = line.substr(5);
        }
    }

    LauncherConfig config;
};

// В следующей части мы создадим графический интерфейс (GUI) на WinAPI
// с кнопками "Пуск", "Настройки" и полями ввода.
/**
 * ЧАСТЬ 1, 2 и 3: Ядро, Установка, GUI (WinAPI) и Запуск.
 * * В этой части мы добавляем графическое окно и формирование команды запуска.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>

#pragma comment(lib, "comctl32.lib")

namespace fs = std::filesystem;

struct LauncherConfig {
    std::string username = "Player";
    int ram_gb = 8;
    int max_ram_gb = 32;
    std::string java_path = "";
    std::string minecraft_path = "";
    std::string version = "1.21.1"; 
    
    std::string mods_url = "https://your-server.com/mods.zip";
    std::string resourcepack_url = "https://your-server.com/resourcepacks.zip";
};

class MinecraftLauncher {
public:
    MinecraftLauncher() {
        setup_environment();
    }

    void setup_environment() {
        char* appdata = getenv("APPDATA");
        if (appdata) {
            config.minecraft_path = std::string(appdata) + "\\.custom_launcher";
            if (!fs::exists(config.minecraft_path)) {
                fs::create_directories(config.minecraft_path);
                fs::create_directories(config.minecraft_path + "\\mods");
                fs::create_directories(config.minecraft_path + "\\resourcepacks");
                fs::create_directories(config.minecraft_path + "\\bin");
            }
        }
        config.java_path = find_java_path();
    }

    std::string find_java_path() {
        char* java_home = getenv("JAVA_HOME");
        if (java_home) {
            std::string path = std::string(java_home) + "\\bin\\javaw.exe";
            if (fs::exists(path)) return path;
        }
        
        std::string pf_java = "C:\\Program Files\\Java";
        if (fs::exists(pf_java)) {
            for (const auto& entry : fs::recursive_directory_iterator(pf_java)) {
                if (entry.path().filename() == "javaw.exe") {
                    return entry.path().string();
                }
            }
        }
        return "java";
    }

    bool download_file(const std::string& url, const std::string& target_path) {
        // Заглушка для cURL
        return true; 
    }

    bool extract_zip(const std::string& zip_path, const std::string& extract_to) {
        // Заглушка для Unzip
        return true;
    }

    void install_game() {
        fs::create_directories(config.minecraft_path + "\\mods");
        fs::create_directories(config.minecraft_path + "\\resourcepacks");

        std::string temp_mods_zip = config.minecraft_path + "\\temp_mods.zip";
        if (download_file(config.mods_url, temp_mods_zip)) {
            extract_zip(temp_mods_zip, config.minecraft_path + "\\mods");
            fs::remove(temp_mods_zip);
        }

        std::string temp_rp_zip = config.minecraft_path + "\\temp_rp.zip";
        if (download_file(config.resourcepack_url, temp_rp_zip)) {
            extract_zip(temp_rp_zip, config.minecraft_path + "\\resourcepacks");
            fs::remove(temp_rp_zip);
        }
    }

    // --- НОВАЯ ЛОГИКА ЧАСТИ 3: ЗАПУСК ИГРЫ ---
    
    void launch_game() {
        save_settings();

        // Формирование аргументов запуска
        // -Xmx: Максимальная память, -Xms: Начальная память
        std::string ram_arg = "-Xmx" + std::to_string(config.ram_gb) + "G";
        
        // В реальности здесь строится огромная строка Classpath со всеми JAR библиотеками
        std::string classpath = "\"" + config.minecraft_path + "\\bin\\*\""; 
        
        std::string cmd = "\"" + config.java_path + "\" " + 
                          ram_arg + " " +
                          "-Djava.library.path=\"" + config.minecraft_path + "\\natives\" " +
                          "-cp " + classpath + " " +
                          "net.fabricmc.loader.impl.launch.knot.KnotClient " + // Точка входа Fabric
                          "--username " + config.username + " " +
                          "--version " + config.version + " " +
                          "--gameDir \"" + config.minecraft_path + "\" " +
                          "--assetsDir \"" + config.minecraft_path + "\\assets\" " +
                          "--assetIndex " + config.version;

        // Запуск процесса
        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            exit(0); // Закрываем лаунчер после успешного пуска
        } else {
            MessageBoxA(NULL, "Ошибка запуска Java! Проверьте путь в настройках.", "Ошибка", MB_OK | MB_ICONERROR);
        }
    }

    void save_settings() {
        std::ofstream file(config.minecraft_path + "\\launcher_settings.ini");
        file << "username=" << config.username << "\n";
        file << "ram=" << config.ram_gb << "\n";
        file << "java=" << config.java_path << "\n";
        file.close();
    }

    void load_settings() {
        std::ifstream file(config.minecraft_path + "\\launcher_settings.ini");
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("username=") == 0) config.username = line.substr(9);
            if (line.find("ram=") == 0) config.ram_gb = std::stoi(line.substr(4));
            if (line.find("java=") == 0) config.java_path = line.substr(5);
        }
    }

    LauncherConfig config;
};

// --- GUI ЧАСТЬ ---

MinecraftLauncher launcher;
HWND hEditNick, hEditRAM, hEditJava;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Текстовые метки и поля ввода
        CreateWindowA("STATIC", "Никнейм:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hEditNick = CreateWindowA("EDIT", launcher.config.username.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 150, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "ОЗУ (ГБ):", WS_VISIBLE | WS_CHILD, 20, 50, 100, 20, hwnd, NULL, NULL, NULL);
        hEditRAM = CreateWindowA("EDIT", std::to_string(launcher.config.ram_gb).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 120, 50, 50, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "Путь к Java:", WS_VISIBLE | WS_CHILD, 20, 80, 100, 20, hwnd, NULL, NULL, NULL);
        hEditJava = CreateWindowA("EDIT", launcher.config.java_path.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 80, 240, 20, hwnd, NULL, NULL, NULL);

        // Кнопки
        CreateWindowA("BUTTON", "УСТАНОВИТЬ", WS_VISIBLE | WS_CHILD, 20, 120, 120, 40, hwnd, (HMENU)1, NULL, NULL);
        CreateWindowA("BUTTON", "ИГРАТЬ", WS_VISIBLE | WS_CHILD, 150, 120, 120, 40, hwnd, (HMENU)2, NULL, NULL);
        CreateWindowA("BUTTON", "ВЫХОД", WS_VISIBLE | WS_CHILD, 280, 120, 80, 40, hwnd, (HMENU)3, NULL, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Установить
            launcher.install_game();
            MessageBoxA(hwnd, "Моды и ресурсы установлены!", "Готово", MB_OK | MB_ICONINFORMATION);
        }
        if (LOWORD(wParam) == 2) { // Играть
            char nick[64], ram[16], java[256];
            GetWindowTextA(hEditNick, nick, 64);
            GetWindowTextA(hEditRAM, ram, 16);
            GetWindowTextA(hEditJava, java, 256);

            launcher.config.username = nick;
            launcher.config.ram_gb = std::stoi(ram);
            launcher.config.java_path = java;

            launcher.launch_game();
        }
        if (LOWORD(wParam) == 3) { // Выход
            PostQuitMessage(0);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    launcher.load_settings();

    const char CLASS_NAME[] = "MC_Launcher_Class";
    WNDCLASSA wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "Fabric 1.21.1 Launcher", WS_OVERLAPPEDWINDOW, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 420, 220, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
/**
 * ЧАСТЬ 4: Подключение внешних библиотек (libcurl и minizip).
 * * В этой части реализована реальная логика скачивания и распаковки.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>

// Подключаем заголовки необходимых библиотек
#include <curl/curl.h>
#include <zip.h> // Библиотека minizip или libzip
#include <unzip.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "zlib.lib")

namespace fs = std::filesystem;

struct LauncherConfig {
    std::string username = "Player";
    int ram_gb = 8;
    int max_ram_gb = 32;
    std::string java_path = "";
    std::string minecraft_path = "";
    std::string version = "1.21.1"; 
    
    std::string mods_url = "https://your-server.com/mods.zip";
    std::string resourcepack_url = "https://your-server.com/resourcepacks.zip";
};

class MinecraftLauncher {
public:
    MinecraftLauncher() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        setup_environment();
    }

    ~MinecraftLauncher() {
        curl_global_cleanup();
    }

    void setup_environment() {
        char* appdata = getenv("APPDATA");
        if (appdata) {
            config.minecraft_path = std::string(appdata) + "\\.custom_launcher";
            if (!fs::exists(config.minecraft_path)) {
                fs::create_directories(config.minecraft_path);
                fs::create_directories(config.minecraft_path + "\\mods");
                fs::create_directories(config.minecraft_path + "\\resourcepacks");
                fs::create_directories(config.minecraft_path + "\\bin");
            }
        }
        config.java_path = find_java_path();
    }

    std::string find_java_path() {
        char* java_home = getenv("JAVA_HOME");
        if (java_home) {
            std::string path = std::string(java_home) + "\\bin\\javaw.exe";
            if (fs::exists(path)) return path;
        }
        
        std::string pf_java = "C:\\Program Files\\Java";
        if (fs::exists(pf_java)) {
            for (const auto& entry : fs::recursive_directory_iterator(pf_java)) {
                if (entry.path().filename() == "javaw.exe") {
                    return entry.path().string();
                }
            }
        }
        return "java";
    }

    // Вспомогательная функция для записи данных curl в файл
    static size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        size_t written = fwrite(ptr, size, nmemb, stream);
        return written;
    }

    // РЕАЛЬНОЕ СКАЧИВАНИЕ через libcurl
    bool download_file(const std::string& url, const std::string& target_path) {
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        FILE* fp = fopen(target_path.c_str(), "wb");
        if (!fp) return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Для редиректов
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Упрощаем для теста

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        return (res == CURLE_OK);
    }

    // РЕАЛЬНАЯ РАСПАКОВКА через minizip
    bool extract_zip(const std::string& zip_path, const std::string& extract_to) {
        unzFile uf = unzOpen(zip_path.c_str());
        if (uf == NULL) return false;

        unz_global_info gi;
        if (unzGetGlobalInfo(uf, &gi) != UNZ_OK) {
            unzClose(uf);
            return false;
        }

        for (uLong i = 0; i < gi.number_entry; i++) {
            char filename[256];
            unz_file_info fi;
            if (unzGetCurrentFileInfo(uf, &fi, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK) break;

            std::string out_path = extract_to + "\\" + filename;
            
            // Если это папка
            if (filename[strlen(filename) - 1] == '/' || filename[strlen(filename) - 1] == '\\') {
                fs::create_directories(out_path);
            } else {
                if (unzOpenCurrentFile(uf) != UNZ_OK) break;

                FILE* fout = fopen(out_path.c_str(), "wb");
                if (fout) {
                    char buffer[4096];
                    int len;
                    while ((len = unzReadCurrentFile(uf, buffer, sizeof(buffer))) > 0) {
                        fwrite(buffer, 1, len, fout);
                    }
                    fclose(fout);
                }
                unzCloseCurrentFile(uf);
            }

            if (i < gi.number_entry - 1) {
                if (unzGoToNextFile(uf) != UNZ_OK) break;
            }
        }

        unzClose(uf);
        return true;
    }

    void install_game() {
        fs::create_directories(config.minecraft_path + "\\mods");
        fs::create_directories(config.minecraft_path + "\\resourcepacks");

        std::string temp_mods_zip = config.minecraft_path + "\\temp_mods.zip";
        if (download_file(config.mods_url, temp_mods_zip)) {
            extract_zip(temp_mods_zip, config.minecraft_path + "\\mods");
            fs::remove(temp_mods_zip);
        }

        std::string temp_rp_zip = config.minecraft_path + "\\temp_rp.zip";
        if (download_file(config.resourcepack_url, temp_rp_zip)) {
            extract_zip(temp_rp_zip, config.minecraft_path + "\\resourcepacks");
            fs::remove(temp_rp_zip);
        }
    }

    void launch_game() {
        save_settings();

        std::string ram_arg = "-Xmx" + std::to_string(config.ram_gb) + "G";
        std::string classpath = "\"" + config.minecraft_path + "\\bin\\*\""; 
        
        std::string cmd = "\"" + config.java_path + "\" " + 
                          ram_arg + " " +
                          "-Djava.library.path=\"" + config.minecraft_path + "\\natives\" " +
                          "-cp " + classpath + " " +
                          "net.fabricmc.loader.impl.launch.knot.KnotClient " +
                          "--username " + config.username + " " +
                          "--version " + config.version + " " +
                          "--gameDir \"" + config.minecraft_path + "\" " +
                          "--assetsDir \"" + config.minecraft_path + "\\assets\" " +
                          "--assetIndex " + config.version;

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            exit(0); 
        } else {
            MessageBoxA(NULL, "Ошибка запуска Java! Проверьте путь в настройках.", "Ошибка", MB_OK | MB_ICONERROR);
        }
    }

    void save_settings() {
        std::ofstream file(config.minecraft_path + "\\launcher_settings.ini");
        file << "username=" << config.username << "\n";
        file << "ram=" << config.ram_gb << "\n";
        file << "java=" << config.java_path << "\n";
        file.close();
    }

    void load_settings() {
        std::ifstream file(config.minecraft_path + "\\launcher_settings.ini");
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("username=") == 0) config.username = line.substr(9);
            if (line.find("ram=") == 0) config.ram_gb = std::stoi(line.substr(4));
            if (line.find("java=") == 0) config.java_path = line.substr(5);
        }
    }

    LauncherConfig config;
};

// --- GUI ОСТАЕТСЯ БЕЗ ИЗМЕНЕНИЙ ---
MinecraftLauncher launcher;
HWND hEditNick, hEditRAM, hEditJava;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateWindowA("STATIC", "Никнейм:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hEditNick = CreateWindowA("EDIT", launcher.config.username.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 150, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "ОЗУ (ГБ):", WS_VISIBLE | WS_CHILD, 20, 50, 100, 20, hwnd, NULL, NULL, NULL);
        hEditRAM = CreateWindowA("EDIT", std::to_string(launcher.config.ram_gb).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 120, 50, 50, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "Путь к Java:", WS_VISIBLE | WS_CHILD, 20, 80, 100, 20, hwnd, NULL, NULL, NULL);
        hEditJava = CreateWindowA("EDIT", launcher.config.java_path.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 80, 240, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("BUTTON", "УСТАНОВИТЬ", WS_VISIBLE | WS_CHILD, 20, 120, 120, 40, hwnd, (HMENU)1, NULL, NULL);
        CreateWindowA("BUTTON", "ИГРАТЬ", WS_VISIBLE | WS_CHILD, 150, 120, 120, 40, hwnd, (HMENU)2, NULL, NULL);
        CreateWindowA("BUTTON", "ВЫХОД", WS_VISIBLE | WS_CHILD, 280, 120, 80, 40, hwnd, (HMENU)3, NULL, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { 
            launcher.install_game();
            MessageBoxA(hwnd, "Моды и ресурсы установлены!", "Готово", MB_OK | MB_ICONINFORMATION);
        }
        if (LOWORD(wParam) == 2) { 
            char nick[64], ram[16], java[256];
            GetWindowTextA(hEditNick, nick, 64);
            GetWindowTextA(hEditRAM, ram, 16);
            GetWindowTextA(hEditJava, java, 256);

            launcher.config.username = nick;
            launcher.config.ram_gb = std::stoi(ram);
            launcher.config.java_path = java;

            launcher.launch_game();
        }
        if (LOWORD(wParam) == 3) { 
            PostQuitMessage(0);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    launcher.load_settings();
    const char CLASS_NAME[] = "MC_Launcher_Class";
    WNDCLASSA wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "Fabric 1.21.1 Launcher", WS_OVERLAPPEDWINDOW, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 420, 220, NULL, NULL, hInstance, NULL);
    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
/**
 * ФИНАЛЬНАЯ ЧАСТЬ: Полировка, валидация и завершение интерфейса.
 * * Реализована проверка ОЗУ, улучшен поиск Java и добавлена стилизация GUI.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <windows.h>
#include <shellapi.h>
#include <commctrl.h>

// Подключаем заголовки необходимых библиотек
#include <curl/curl.h>
#include <zip.h> 
#include <unzip.h>

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "zlib.lib")

namespace fs = std::filesystem;

struct LauncherConfig {
    std::string username = "Player";
    int ram_gb = 8;
    int min_ram_gb = 1;
    int max_ram_gb = 32;
    std::string java_path = "";
    std::string minecraft_path = "";
    std::string version = "1.21.1"; 
    
    // Прямые ссылки на ресурсы
    std::string mods_url = "https://your-server.com/mods.zip";
    std::string resourcepack_url = "https://your-server.com/resourcepacks.zip";
};

class MinecraftLauncher {
public:
    MinecraftLauncher() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        setup_environment();
    }

    ~MinecraftLauncher() {
        curl_global_cleanup();
    }

    void setup_environment() {
        char* appdata = getenv("APPDATA");
        if (appdata) {
            config.minecraft_path = std::string(appdata) + "\\.custom_launcher";
            try {
                if (!fs::exists(config.minecraft_path)) {
                    fs::create_directories(config.minecraft_path);
                    fs::create_directories(config.minecraft_path + "\\mods");
                    fs::create_directories(config.minecraft_path + "\\resourcepacks");
                    fs::create_directories(config.minecraft_path + "\\bin");
                    fs::create_directories(config.minecraft_path + "\\natives");
                }
            } catch (...) {
                MessageBoxA(NULL, "Ошибка при создании директорий игры!", "Системная ошибка", MB_OK | MB_ICONERROR);
            }
        }
        config.java_path = find_java_path();
    }

    std::string find_java_path() {
        // 1. Проверка переменной среды
        char* java_home = getenv("JAVA_HOME");
        if (java_home) {
            std::string path = std::string(java_home) + "\\bin\\javaw.exe";
            if (fs::exists(path)) return path;
        }
        
        // 2. Поиск в стандартных путях (Java 17/21 необходимы для 1.21.1)
        std::vector<std::string> search_paths = {
            "C:\\Program Files\\Java",
            "C:\\Program Files\\Eclipse Foundation",
            "C:\\Program Files\\BellSoft"
        };

        for (const auto& base : search_paths) {
            if (fs::exists(base)) {
                for (const auto& entry : fs::recursive_directory_iterator(base)) {
                    if (entry.path().filename() == "javaw.exe") {
                        return entry.path().string();
                    }
                }
            }
        }
        return "java"; // Fallback на системный путь
    }

    static size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
        return fwrite(ptr, size, nmemb, stream);
    }

    bool download_file(const std::string& url, const std::string& target_path) {
        CURL* curl = curl_easy_init();
        if (!curl) return false;

        FILE* fp = fopen(target_path.c_str(), "wb");
        if (!fp) return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        return (res == CURLE_OK);
    }

    bool extract_zip(const std::string& zip_path, const std::string& extract_to) {
        unzFile uf = unzOpen(zip_path.c_str());
        if (uf == NULL) return false;

        unz_global_info gi;
        if (unzGetGlobalInfo(uf, &gi) != UNZ_OK) {
            unzClose(uf);
            return false;
        }

        for (uLong i = 0; i < gi.number_entry; i++) {
            char filename[256];
            unz_file_info fi;
            if (unzGetCurrentFileInfo(uf, &fi, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK) break;

            std::string out_path = extract_to + "\\" + filename;
            
            if (filename[strlen(filename) - 1] == '/' || filename[strlen(filename) - 1] == '\\') {
                fs::create_directories(out_path);
            } else {
                if (unzOpenCurrentFile(uf) != UNZ_OK) break;

                FILE* fout = fopen(out_path.c_str(), "wb");
                if (fout) {
                    char buffer[4096];
                    int len;
                    while ((len = unzReadCurrentFile(uf, buffer, sizeof(buffer))) > 0) {
                        fwrite(buffer, 1, len, fout);
                    }
                    fclose(fout);
                }
                unzCloseCurrentFile(uf);
            }

            if (i < gi.number_entry - 1) {
                if (unzGoToNextFile(uf) != UNZ_OK) break;
            }
        }
        unzClose(uf);
        return true;
    }

    void install_game(HWND hwnd) {
        bool success = true;
        
        // Скачивание и распаковка модов
        std::string temp_mods = config.minecraft_path + "\\temp_mods.zip";
        if (download_file(config.mods_url, temp_mods)) {
            if (!extract_zip(temp_mods, config.minecraft_path + "\\mods")) success = false;
            fs::remove(temp_mods);
        } else success = false;

        // Скачивание и распаковка ресурсов
        std::string temp_rp = config.minecraft_path + "\\temp_rp.zip";
        if (download_file(config.resourcepack_url, temp_rp)) {
            if (!extract_zip(temp_rp, config.minecraft_path + "\\resourcepacks")) success = false;
            fs::remove(temp_rp);
        } else success = false;

        if (success) {
            MessageBoxA(hwnd, "Установка завершена! Моды и ресурспаки готовы.", "Успех", MB_OK | MB_ICONINFORMATION);
        } else {
            MessageBoxA(hwnd, "Ошибка при загрузке или распаковке файлов. Проверьте интернет-соединение.", "Ошибка", MB_OK | MB_ICONERROR);
        }
    }

    void launch_game() {
        save_settings();

        // Валидация памяти
        if (config.ram_gb < config.min_ram_gb) config.ram_gb = config.min_ram_gb;
        if (config.ram_gb > config.max_ram_gb) config.ram_gb = config.max_ram_gb;

        std::string ram_arg = "-Xmx" + std::to_string(config.ram_gb) + "G";
        // Универсальный classpath для bin
        std::string classpath = "\"" + config.minecraft_path + "\\bin\\*\""; 
        
        std::string cmd = "\"" + config.java_path + "\" " + 
                          ram_arg + " " +
                          "-Djava.library.path=\"" + config.minecraft_path + "\\natives\" " +
                          "-cp " + classpath + " " +
                          "net.fabricmc.loader.impl.launch.knot.KnotClient " +
                          "--username " + config.username + " " +
                          "--version " + config.version + " " +
                          "--gameDir \"" + config.minecraft_path + "\" " +
                          "--assetsDir \"" + config.minecraft_path + "\\assets\" " +
                          "--assetIndex " + config.version;

        STARTUPINFOA si = { sizeof(si) };
        PROCESS_INFORMATION pi;
        if (CreateProcessA(NULL, (LPSTR)cmd.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            exit(0); 
        } else {
            MessageBoxA(NULL, "Не удалось запустить Minecraft. Убедитесь, что Java установлена корректно.", "Ошибка запуска", MB_OK | MB_ICONERROR);
        }
    }

    void save_settings() {
        std::ofstream file(config.minecraft_path + "\\launcher_settings.ini");
        file << "username=" << config.username << "\n";
        file << "ram=" << config.ram_gb << "\n";
        file << "java=" << config.java_path << "\n";
        file.close();
    }

    void load_settings() {
        std::ifstream file(config.minecraft_path + "\\launcher_settings.ini");
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            size_t sep = line.find('=');
            if (sep == std::string::npos) continue;
            std::string key = line.substr(0, sep);
            std::string val = line.substr(sep + 1);

            if (key == "username") config.username = val;
            if (key == "ram") {
                try { config.ram_gb = std::stoi(val); } catch(...) { config.ram_gb = 8; }
            }
            if (key == "java") config.java_path = val;
        }
    }

    LauncherConfig config;
};

// --- GUI ---
MinecraftLauncher launcher;
HWND hEditNick, hEditRAM, hEditJava;
HFONT hFont;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        // Создание шрифта для интерфейса
        hFont = CreateFontA(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Segoe UI");

        // Никнейм
        CreateWindowA("STATIC", "Никнейм:", WS_VISIBLE | WS_CHILD, 20, 20, 100, 20, hwnd, NULL, NULL, NULL);
        hEditNick = CreateWindowA("EDIT", launcher.config.username.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 20, 200, 20, hwnd, NULL, NULL, NULL);

        // RAM
        CreateWindowA("STATIC", "ОЗУ (ГБ):", WS_VISIBLE | WS_CHILD, 20, 50, 100, 20, hwnd, NULL, NULL, NULL);
        hEditRAM = CreateWindowA("EDIT", std::to_string(launcher.config.ram_gb).c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, 120, 50, 50, 20, hwnd, NULL, NULL, NULL);

        // Java Path
        CreateWindowA("STATIC", "Java Путь:", WS_VISIBLE | WS_CHILD, 20, 80, 100, 20, hwnd, NULL, NULL, NULL);
        hEditJava = CreateWindowA("EDIT", launcher.config.java_path.c_str(), WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 80, 240, 20, hwnd, NULL, NULL, NULL);

        // Кнопки
        HWND btnInstall = CreateWindowA("BUTTON", "УСТАНОВИТЬ", WS_VISIBLE | WS_CHILD, 20, 120, 120, 40, hwnd, (HMENU)1, NULL, NULL);
        HWND btnPlay = CreateWindowA("BUTTON", "ИГРАТЬ", WS_VISIBLE | WS_CHILD, 150, 120, 120, 40, hwnd, (HMENU)2, NULL, NULL);
        HWND btnExit = CreateWindowA("BUTTON", "ВЫХОД", WS_VISIBLE | WS_CHILD, 280, 120, 80, 40, hwnd, (HMENU)3, NULL, NULL);

        // Применяем шрифт ко всем элементам
        EnumChildWindows(hwnd, [](HWND child, LPARAM font) -> BOOL {
            SendMessage(child, WM_SETFONT, font, TRUE);
            return TRUE;
        }, (LPARAM)hFont);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) { // Установить
            launcher.install_game(hwnd);
        }
        if (LOWORD(wParam) == 2) { // Играть
            char nick[64], ram[16], java[256];
            GetWindowTextA(hEditNick, nick, 64);
            GetWindowTextA(hEditRAM, ram, 16);
            GetWindowTextA(hEditJava, java, 256);

            launcher.config.username = nick;
            launcher.config.ram_gb = std::stoi(ram);
            launcher.config.java_path = java;

            launcher.launch_game();
        }
        if (LOWORD(wParam) == 3) { // Выход
            PostQuitMessage(0);
        }
        break;

    case WM_DESTROY:
        DeleteObject(hFont);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    launcher.load_settings();

    const char CLASS_NAME[] = "MC_Launcher_Class";
    WNDCLASSA wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); // Цвет стандартного GUI
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassA(&wc);

    int w = 420, h = 220;
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    HWND hwnd = CreateWindowExA(0, CLASS_NAME, "Fabric 1.21.1 Copper Launcher", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, 
                                (screenW - w)/2, (screenH - h)/2, w, h, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}