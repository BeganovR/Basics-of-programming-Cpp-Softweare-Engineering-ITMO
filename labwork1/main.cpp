#include <iostream>
#include <fstream>

/*Ввод в файл строки*/
void OutFunc(const char *argv, int stri)
{
    std::ofstream text;
    text.open(argv, std::ios::app);
    text << stri << '\n';
}
/*Ввод в файл строки*/
void OutFunc_st(const char *argv, char *stri)
{
    std::ofstream text;
    text.open(argv, std::ios::app);
    text << stri << '\n';
}

/*Перевод из строки в int*/
int StrToInt(const char *str)
{
    int result = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        if ((str[i] >= '0') && (str[i] <= '9'))
        {
            result = result * 10 + (str[i] - '0');
        }
        else
        {
            break;
        }
    }
    return result;
}

/*Перевод из массива чаров в лонг*/
bool Stroll(const char *str, long long &result)
{ // str to long int
    result = 0;
    bool negative = false;

    /*Пропуск пробелов и обработка знака*/
    while (*str == ' ')
        ++str;
    if (*str == '-')
    {
        negative = true;
        ++str;
    }
    else if (*str == '+')
    {
        ++str;
    }

    if (!std::isdigit(*str))
        return false;
    while (std::isdigit(*str))
    {
        int digit = *str - '0';
        if (result > (INT_MAX - digit) / 10)
            return false; /*Проверка на переполнение*/
        result = result * 10 + digit;
        ++str;
    }

    if (*str != '\0')
        return false;

    if (negative)
        result = -result;

    return true;
}

/*Парсинг аргументов*/
void ParsArgs(int argc, char **argv, long long &from, long long &to, long long &window, long long &stats, const char *&output_file, bool &print, const char *&filename)
{
    bool fileSpecified = false; // Флаг для проверки указания файла

    for (int i = 1; i < argc; ++i)
    {
        if (std::strncmp(argv[i], "--output=", 9) == 0)
        {
            output_file = argv[i] + 9;
        }
        else if (std::strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                output_file = argv[++i];
            }
        }
        else if (std::strcmp(argv[i], "--print") == 0 || std::strcmp(argv[i], "-p") == 0)
        {
            print = true;
        }
        else if (std::strncmp(argv[i], "--stats=", 8) == 0)
        {
            const char *stats_str = argv[i] + 8;
            if (!Stroll(stats_str, stats))
            {
                stats = 10;
            }
        }
        else if (std::strcmp(argv[i], "-s") == 0)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                const char *stats_str = argv[++i];
                if (!Stroll(stats_str, stats))
                {
                    std::cout << "Неправильное значение s: " << stats_str << std::endl;
                    stats = 10;
                }
            }
            else
            {
                stats = 10;
            }
        }
        else if (std::strncmp(argv[i], "--window=", 9) == 0)
        {
            const char *window_str = argv[i] + 9;
            if (!Stroll(window_str, window))
            {
                window = 0;
            }
        }
        else if (std::strcmp(argv[i], "-w") == 0)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                const char *window_str = argv[++i];
                if (!Stroll(window_str, window))
                {
                    std::cout << "Неправильное значение w: " << window_str << std::endl;
                    window = 0;
                }
            }
            else
            {
                window = 0;
            }
        }
        else if (std::strncmp(argv[i], "--from=", 7) == 0)
        {
            const char *from_str = argv[i] + 7;
            if (!Stroll(from_str, from))
            {
                from = 0; // Значение по умолчанию
            }
        }
        else if (std::strcmp(argv[i], "-f") == 0)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                const char *from_str = argv[++i];
                if (!Stroll(from_str, from))
                {
                    from = 0;
                }
            }
            else
            {
                from = 0;
            }
        }
        else if (std::strncmp(argv[i], "--to=", 5) == 0)
        {
            const char *to_str = argv[i] + 5;
            if (!Stroll(to_str, to))
            {
                to = LLONG_MAX; // Значение по умолчанию
            }
        }
        else if (std::strcmp(argv[i], "-e") == 0)
        {
            if (i + 1 < argc && argv[i + 1][0] != '-')
            {
                const char *to_str = argv[++i];
                if (!Stroll(to_str, to))
                {
                    to = LLONG_MAX;
                }
            }
            else
            {
                to = LLONG_MAX;
            }
        }
        else if (argv[i][0] != '-')
        { // Файл не должен начинаться с '-'
            filename = argv[i];
            fileSpecified = true;
        }
        else
        {
            std::cout << "Незнакомый аргумент: " << argv[i] << "\n";
        }
    }

    // Если файл не был указан, выводим ошибку
    if (!fileSpecified)
    {
        std::cout << "Файл не был указан" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

///////////////////////////////////TIME////////////////////////////////////////////////////////////////
/*Парс чисел*/
int ParsNumb(const char *date_time, int start, int len)
{
    int result = 0;
    for (int i = start; i < (start + len); ++i)
    {
        result = result * 10 + (date_time[i] - '0');
    }
    return result;
}
/*Парс месяцов*/
int ParsMonth(const char *str)
{
    const char *month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < 12; ++i)
    {
        if ((str[0] == month[i][0]) && (str[1] == month[i][1]) && (str[2] == month[i][2]))
        {
            return i;
        }
    }
    return -1;
}

/*Перевод даты в секунды*/
long long DateToSec(const char *date_time)
{
    std::tm tm = {};
    tm.tm_mday = ParsNumb(date_time, 0, 2);
    const char *mont[3];
    mont[0] = &date_time[3]; // Копируем 3-й символ
    mont[1] = &date_time[4]; // Копируем 4-й символ
    mont[2] = &date_time[5]; // Копируем 5-й символ
    tm.tm_mon = ParsMonth(*mont);
    tm.tm_year = ParsNumb(date_time, 7, 4) - 1900;
    tm.tm_hour = ParsNumb(date_time, 12, 2);
    tm.tm_min = ParsNumb(date_time, 15, 2);
    tm.tm_sec = ParsNumb(date_time, 18, 2);
    char utc_offset_sign = date_time[21];
    std::time_t time_stamp = std::mktime(&tm);
    if (time_stamp == -1)
    {
        std::cout << ("Ошибка преобразования времени.");
    }
    int utc_offset_hours = ParsNumb(date_time, 22, 2);
    int utc_offset_minut = ParsNumb(date_time, 24, 2);
    int utc_offset_second = (utc_offset_hours * 3600) + (utc_offset_minut * 60);
    if (utc_offset_sign == '-')
    {
        time_stamp = time_stamp + utc_offset_second;
    }
    else
    {
        time_stamp = time_stamp - utc_offset_second;
    }
    return static_cast<long long>(time_stamp);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

struct struct_log
{
    char date_time[32];
    char url[2048];
    int status;
};

struct struct_for_stats
{
    char url[2000];
    int count;
};

/*Парсинг логов*/
void ParsingOfLogs(const char *log, struct_log &slog)
{
    char status_for_struct[4];
    int big_i = 0, small_i = 0;
    // парсим дату
    while (log[big_i] != '[' && log[big_i] != '\0')
    {
        big_i++;
    }
    big_i++;
    if (log[big_i] == '[')
    {
        big_i++;
    }
    while (log[big_i] != ']' && log[big_i] != '\0')
    {
        slog.date_time[small_i++] = log[big_i++];
    }
    slog.date_time[small_i] = '\0';
    ///////////////////////////
    int end_log = std::strlen(log) - 1;
    // пропуск пробелов
    while (log[end_log] == ' ' && end_log >= 0)
    {
        end_log--;
    }
    // пропуск байтов
    while (log[end_log] != ' ' && end_log >= 0)
    {
        end_log--;
    }
    // пропуск пробелов
    while (log[end_log] == ' ' && end_log >= 0)
    {
        end_log--;
    }
    // берем статус
    small_i = 0;
    while (log[end_log] != ' ')
    {
        status_for_struct[small_i++] = log[end_log--];
    }
    // переворот статуса
    char tmp = status_for_struct[2];
    status_for_struct[2] = status_for_struct[0];
    status_for_struct[0] = tmp;
    slog.status = StrToInt(status_for_struct);
    //////////////////////////////
    const char *start_forging = strchr(log, '"');
    int index_start_forging = start_forging - log;
    const char *end_forging = strrchr(log, '"');
    int index_end_forging = end_forging - log;
    small_i = 0;
    for (int i = index_start_forging + 1; i < index_end_forging; i++)
    {
        slog.url[small_i++] = log[i];
    }
}

/*Window*/
void WindowM(const char *filename, long long window_duration, long long from, long long to)
{
    if (window_duration == 0)
    {
        std::cout << "Расчет окна не производится, тк показатель window = 0" << std::endl;
        return;
    }
    int size_of_buffer = 10000;
    int timestamp = 0;
    char buffer[size_of_buffer];
    struct_log slog;
    std::ifstream file1(filename);
    const int buffer_size = 10000;
    int time_buffer[buffer_size];
    int head = 0, tail = 0;
    long long max_requests = 0;
    long long max_window_start = 0;
    int current_count = 0;

    if (!file1.is_open())
    {
        std::cout << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }
    while (file1.getline(buffer, size_of_buffer))
    {
        if (file1.fail())
        {
            std::cout << "Ошибка чтения строки: длина строки превышает допустимый размер буфера" << std::endl;
            break;
        }
        ParsingOfLogs(buffer, slog);
        timestamp = DateToSec(slog.date_time);
        if (timestamp == -1)
        {
            std::cout << "Ошибка преобразования даты: " << slog.date_time << std::endl;
            continue;
        }
        if (timestamp < from && timestamp > to)
        {
            continue;
        }
        while (current_count > 0 && (timestamp - time_buffer[head] >= window_duration))
        {
            head = (head + 1) % buffer_size;
            current_count--;
        }
        // заполнение массива
        time_buffer[tail] = timestamp;
        tail = (tail + 1) % buffer_size;
        current_count++;
        // обновление максимального количества запросов и начала окна
        if (current_count > max_requests)
        {
            max_requests = current_count;
            max_window_start = time_buffer[head];
        }
    }
    file1.close();
    // вывод
    if (max_requests > 0)
    {
        char message[100];
        std::sprintf(message, "Max requests: %lld at window starting: %lld\n", max_requests, max_window_start);
        std::cout << message; // Вывод на консоль
    }
    else
    {
        std::cout << "No requests have found." << std::endl;
    }
}

/*Stats*/
void Stats(const char *filename, long long stats, long long from, long long to)
{
    struct_log slog;
    std::ifstream file1(filename);
    struct_for_stats url[2000];
    int size_of_buffer = 10000;
    char buffer[size_of_buffer];
    int timestamp;
    int url_counts_size = 0;
    if (!file1.is_open())
    {
        std::cout << "Ошибка открытия файла: " << filename << std::endl;
        return;
    }
    while (file1.getline(buffer, size_of_buffer))
    {
        if (file1.fail())
        {
            std::cout << "Ошибка: строка слишком длинная для буфера." << std::endl;
            file1.clear(); // Сбрасываем ошибку, чтобы продолжить чтение
            continue;
        }
        ParsingOfLogs(buffer, slog);
        timestamp = DateToSec(slog.date_time);
        if (timestamp >= from && timestamp <= to)
        {
            if (slog.status >= 500 && slog.status <= 599)
            {
                int idx = -1;
                for (int i = 0; i < url_counts_size; ++i)
                {
                    if (std::strncmp(url[i].url, slog.url, 255) == 0)
                    {
                        idx = i;
                        break;
                    }
                }
                if (idx != -1)
                {
                    // Найден - увеличиваем счетчик
                    url[idx].count++;
                }
                else
                {
                    // Не найден, добавляем новый урл
                    if (url_counts_size >= 2000)
                    {
                        std::cout << "Ошибка: достигнуто максимальное количество URL-адресов." << std::endl;
                        break;
                    }
                    else
                    {
                        std::strncpy(url[url_counts_size].url, slog.url, 254);
                        url[url_counts_size].url[254] = '\0';
                        url[url_counts_size].count = 1;
                        url_counts_size++;
                    }
                }
            }
        }
    }
    file1.close();

    if (url_counts_size > 0 && stats > 0)
    {
        for (int i = 0; i < url_counts_size - 1; ++i)
        {
            int max_idx = i;
            for (int j = i + 1; j < url_counts_size; ++j)
            {
                if (url[j].count > url[max_idx].count)
                {
                    max_idx = j;
                }
            }
            if (max_idx != i)
            {
                struct_for_stats tmp = url[i];
                url[i] = url[max_idx];
                url[max_idx] = tmp;
            }
        }
        int output_count;
        if (stats < url_counts_size)
        {
            output_count = stats;
        }
        else
        {
            output_count = url_counts_size;
        }
        char line[512];
        std::snprintf(line, sizeof(line), "Top %d most frequent 5XX requests:\n", output_count);
        std::cout << line << std::endl;
        for (int i = 0; i < output_count; ++i)
        {
            std::snprintf(line, sizeof(line), "%d: %s (%d times)\n", i + 1, url[i].url, url[i].count);
            std::cout << line << std::endl;
        }
    }
}

int main(int argc, char **argv)
{
    long long stats = 10;
    long long window = 0; // Значение по умолчанию
    long long from_timestamp = 0;
    long long to_timestamp = LLONG_MAX;
    const char *output_file = nullptr; // Поток вывода
    bool print_console = false;     // Флаг для печати в консоль
    const char *filename = nullptr; // Имя файла
    // Вызов функции парсинга аргументов (например, ParsArgs)
    ParsArgs(argc, argv, from_timestamp, to_timestamp, window, stats, output_file, print_console, filename);
    if (window < 0)
    {
        std::cout << "Ошибка: Окно должно быть больше 0." << std::endl;
        return 1;
    }
    std::ifstream file1(filename);
    if (!file1.is_open())
    {
        std::cout << "Ошибка открытия файла: " << filename << std::endl;
        return 1;
    }
    int size_of_buffer = 10000;
    std::cout << print_console;
    int timestamp = 0, status = 0;
    char buffer[size_of_buffer];
    struct_log slog; // Временная структура для хранения одного лога
    struct_for_stats url[2000];
    if (filename == nullptr)
    {
        std::cout << "Ошибка: filename не инициализировано!" << std::endl;
        return 1;
    }
    WindowM(filename, window, from_timestamp, to_timestamp);
    Stats(filename, stats, from_timestamp, to_timestamp);
    while (file1.getline(buffer, size_of_buffer))
    {
        if (file1.fail())
        {
            std::cout << "Ошибка чтения строки: длина строки превышает допустимый размер буфера" << std::endl;
            break;
        }
        ParsingOfLogs(buffer, slog);
        timestamp = DateToSec(slog.date_time);

        if (timestamp == -1)
        {
            std::cout << "Ошибка преобразования даты: " << slog.date_time << std::endl;
            continue;
        }
        // нахождение ошбики
        if (timestamp >= from_timestamp && timestamp <= to_timestamp)
        {
            if (slog.status >= 500 && slog.status < 600)
            {
                OutFunc(output_file, slog.status);
                OutFunc_st(output_file, slog.url);
            }
        }
    }
    if (print_console == true)
    {
        std::ifstream file_to_read(output_file);
        if (!file_to_read.is_open())
        {
            std::cout << "Ошибка: Не удалось открыть файл!" << std::endl;
            return 1;
        }
        int line_size = 10000;
        char line[line_size];
        std::cout << "Начало чтения файла: " << std::endl;
        while (file_to_read.getline(line, line_size))
        {
            std::cout << line << std::endl;
        }
        std::cout << "Конец чтения файла." << std::endl;
        file_to_read.close();
    }
    file1.close();
    return 0;
}
