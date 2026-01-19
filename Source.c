#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

struct Date {
    int day;
    int month;
    int year;
};

typedef struct {
    char name[30];
    char position[50];
    struct Date hire_date;
} Employee;

int save_to_file(char* filename, Employee employees[], int n);
int load_from_file(char* filename, Employee employees[], int* n);
struct Date get_current_date();
double calculate_experience(struct Date hire_date);
double calculate_average_experience(Employee employees[], int n, double staj[]);
void print_employee(Employee emp, double staj);
Employee* search_by_field(Employee employees[], int n, double staj[], int choice, char* search_value);
void sort_by_field(Employee employees[], int n, double staj[]);
void generate_random_employees(Employee employees[], int n);
int compare_by_name_asc(const void* a, const void* b);
int compare_by_name_desc(const void* a, const void* b);

const char* NAMES[] = { "Aleksandr", "Aleksey", "Andrey", "Artem", "Boris", "Ekaterina", "Valentina", "Valerii", "Viktor", "Vladimir" };
const char* POSITIONS[] = { "Menedjer", "Proggramist", "Buhgalter", "Disigner", "Analitic", "Testirovhik" };

int main() {
    srand(time(NULL));

    int i, n = 0;
    Employee employees[10];
    double staj[10];
    double avg_years = 0;
    int menu;
    char filename[100];

    printf("---РАБОТА С СОТРУДНИКАМИ---\n");

    printf("1. Загрузить данные из файла\n");
    printf("2. Создать новые данные\n");
    printf("Выберите действие: ");
    int init_choice;
    scanf("%d", &init_choice);

    if (init_choice == 1) {
        printf("Введите имя файла для загрузки: ");
        scanf("%s", filename);

        if (load_from_file(filename, employees, &n) == 0) {
            printf("Данные успешно загружены из файла '%s'! Загружено %d сотрудников.\n", filename, n);
        }
        else {
            printf("Не удалось загрузить данные. Будет выполнена генерация новых данных.\n");
            goto generate_new;
        }
    }
    else {
    generate_new:
        printf("Сколько сотрудников добавить (от 5 до 10): ");
        scanf("%d", &n);

        if (n < 5 || n > 10) {
            printf("Ошибка - введите от 5 до 10 сотрудников\n");
            return 1;
        }

        generate_random_employees(employees, n);
        printf("\nДанные %d сотрудников успешно добавлены!\n", n);
    }

    for (i = 0; i < n; i++) {
        staj[i] = calculate_experience(employees[i].hire_date);
    }
    avg_years = calculate_average_experience(employees, n, staj);

    do {
        printf("\n---МЕНЮ---\n");
        printf("1. Показать всех сотрудников\n");
        printf("2. Поиск сотрудника\n");
        printf("3. Сортировка сотрудников\n");
        printf("4. Расчет среднего стажа\n");
        printf("5. Показать сотрудников со стажем выше среднего\n");
        printf("6. Сохранить данные в файл\n");
        printf("7. Выход\n");
        printf("Выберите действие: ");
        scanf("%d", &menu);

        switch (menu) {
        case 1:
            printf("\n---Все сотрудники---\n");
            for (i = 0; i < n; i++) {
                print_employee(employees[i], staj[i]);
            }
            break;

        case 2:
        {
            int search_choice;
            char search_value[50];
            Employee* found_employee;
            int found = 0;

            printf("\n---ПОИСК СОТРУДНИКА---\n");
            printf("По какому полю искать:\n");
            printf("1. Имя\n");
            printf("2. Должность\n");
            printf("Выберите поле для поиска: ");
            scanf("%d", &search_choice);

            printf("Введите значение для поиска: ");
            scanf("%s", search_value);

            found_employee = search_by_field(employees, n, staj, search_choice, search_value);

            if (found_employee != NULL) {
                int index = found_employee - employees;
                printf("\nРезультаты поиска:\n");
                print_employee(*found_employee, staj[index]);
                found = 1;
            }

            if (!found) {
                printf("\nСотрудники не найдены.\n");
            }
        }
        break;

        case 3:
            sort_by_field(employees, n, staj);
            break;

        case 4:
            printf("\n---Расчет стажа---\n");
            for (i = 0; i < n; i++) {
                printf("%s (%s): %.1f лет\n", employees[i].name, employees[i].position, staj[i]);
            }
            printf("\nСредний стаж: %.1f лет\n", avg_years);
            break;

        case 5:
            printf("\n---Сотрудники со стажем выше среднего (%.1f лет)---\n", avg_years);
            int count = 0;
            for (i = 0; i < n; i++) {
                if (staj[i] > avg_years) {
                    print_employee(employees[i], staj[i]);
                    count++;
                }
            }
            if (count == 0) {
                printf("Таких сотрудников нет.\n");
            }
            else {
                printf("\nНайдено: %d сотрудников\n", count);
            }
            break;

        case 6:
            printf("Введите имя файла для сохранения: ");
            scanf("%s", filename);

            if (save_to_file(filename, employees, n) == 0) {
                printf("Данные успешно сохранены в файл '%s'!\n", filename);
            }
            else {
                printf("Ошибка при сохранении данных!\n");
            }
            break;

        case 7:
            printf("Выход из программы.\n");
            break;

        default:
            printf("Неверный выбор. Попробуйте снова.\n");
        }
    } while (menu != 7);

    return 0;
}

int save_to_file(char* filename, Employee employees[], int n) {
    FILE* file = fopen(filename, "wb"); 

    if (file == NULL) {
        printf("Ошибка открытия файла для записи!\n");
        return -1;
    }

    if (fwrite(&n, sizeof(int), 1, file) != 1) {
        printf("Ошибка записи количества элементов!\n");
        fclose(file);
        return -1;
    }

    int written = fwrite(employees, sizeof(Employee), n, file);
    if (written != n) {
        printf("Ошибка записи данных! Записано %d из %d элементов\n", written, n);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}

int load_from_file(char* filename, Employee employees[], int* n) {
    FILE* file = fopen(filename, "rb"); 

    if (file == NULL) {
        printf("Файл '%s' не найден или не может быть открыт!\n", filename);
        return -1;
    }

    if (fread(n, sizeof(int), 1, file) != 1) {
        printf("Ошибка чтения количества элементов!\n");
        fclose(file);
        return -1;
    }

    if (*n < 0 || *n > 10) {
        printf("Некорректное количество элементов в файле: %d\n", *n);
        fclose(file);
        return -1;
    }

    int read = fread(employees, sizeof(Employee), *n, file);
    if (read != *n) {
        printf("Ошибка чтения данных! Прочитано %d из %d элементов\n", read, *n);
        fclose(file);
        return -1;
    }

    fclose(file);
    return 0;
}


struct Date get_current_date() {
    struct tm* mytime;
    time_t t;
    t = time(NULL);
    mytime = localtime(&t);

    struct Date current;
    current.day = mytime->tm_mday;
    current.month = mytime->tm_mon + 1;
    current.year = mytime->tm_year + 1900;

    return current;
}

double calculate_experience(struct Date hire_date) {
    struct Date current = get_current_date();
    int days = (current.year - hire_date.year) * 365 + (current.month - hire_date.month) * 30 + (current.day - hire_date.day);

    if (days < 0) {
        return 0;
    }

    return days / 365.0;
}

double calculate_average_experience(Employee employees[], int n, double staj[]) {
    
    double total_years = 0;

    for (int i = 0; i < n; i++) {
        staj[i] = calculate_experience(employees[i].hire_date);
        total_years += staj[i];
    }

    return total_years / n;
}

void print_employee(Employee emp, double staj) {
    printf("\n***Сотрудник***\n");
    printf("Имя: %s\n", emp.name);
    printf("Должность: %s\n", emp.position);
    printf("Дата приема: %02d.%02d.%d\n", emp.hire_date.day, emp.hire_date.month, emp.hire_date.year);
    printf("Стаж: %.1f лет\n", staj);
}

Employee* search_by_field(Employee employees[], int n, double staj[], int choice, char* search_value) {
    switch (choice) {
    case 1:
        for (int i = 0; i < n; i++) {
            if (strcmp(employees[i].name, search_value) == 0) {
                return &employees[i];
            }
        }
        break;

    case 2:
        for (int i = 0; i < n; i++) {
            if (strcmp(employees[i].position, search_value) == 0) {
                return &employees[i];
            }
        }
        break;
    }

    return NULL;
}

int compare_by_name_asc(const void* a, const void* b) {
    const Employee* employeA = (Employee*)a;
    const Employee* employeB = (Employee*)b;
    return strcmp(employeA->name, employeB->name);
}

int compare_by_name_desc(const void* a, const void* b) {
    const Employee* employeA = (Employee*)a;
    const Employee* employeB = (Employee*)b;
    return strcmp(employeB->name, employeA->name);
}

void sort_by_field(Employee employees[], int n, double staj[]) {
    int menu;
    printf("\n---СОРТИРОВКА СОТРУДНИКОВ---\n");
    printf("По какому полю сортировать:\n");
    printf("1. Имя (А-Я)\n");
    printf("2. Имя (Я-А)\n");
    printf("Выберите поле для сортировки: ");
    scanf("%d", &menu);

    if (menu == 1) {
        qsort(employees, n, sizeof(Employee), compare_by_name_asc);
        printf("\nСортировка по имени (А-Я) выполнена.\n");
    }
    else if (menu == 2) {
        qsort(employees, n, sizeof(Employee), compare_by_name_desc);
        printf("\nСортировка по имени (Я-А) выполнена.\n");
    }
    else {
        printf("Неверный выбор.\n");
        return;
    }

    printf("\n---Отсортированный список сотрудников---\n");
    for (int i = 0; i < n; i++) {
        print_employee(employees[i], staj[i]);
    }
}

void generate_random_employees(Employee employees[], int n) {
    struct Date today = get_current_date();

    for (int i = 0; i < n; i++) {
        strcpy(employees[i].name, NAMES[rand() % 10]);
        strcpy(employees[i].position, POSITIONS[rand() % 6]);

        int days_ago = rand() % 7300;

        int today_in_days = today.year * 365 + today.month * 30 + today.day;
        int hire_in_days = today_in_days - days_ago;

        employees[i].hire_date.year = hire_in_days / 365;
        hire_in_days %= 365;
        employees[i].hire_date.month = hire_in_days / 30;
        employees[i].hire_date.day = hire_in_days % 30;

        if (employees[i].hire_date.month <= 0) {
            employees[i].hire_date.month = 1;
        }
        else if (employees[i].hire_date.month > 12) {
            employees[i].hire_date.month = 12;
        }

        if (employees[i].hire_date.day <= 0) {
            employees[i].hire_date.day = 1;
        }
        else if (employees[i].hire_date.day > 28) {
            employees[i].hire_date.day = 28;
        }
    }
}