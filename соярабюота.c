#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ADMIN_LOGIN     "admin"
#define ADMIN_PASSWORD  "admin123"

const char* FILE_USERS = "users.txt";

struct user
{
    char login[50];
    char password[50];
};

struct user* USERS = NULL;
int user_capacity = 0;
int user_count = 0;

void create_account();
void user_vertification();
void load_users_from_file();
void save_users_to_file();
void admin_menu();
int delete_user_by_login(const char* login);
int find_user_by_login(const char* login);

int find_user_by_login(const char* login)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(USERS[i].login, login) == 0)
            return i;
    }
    return -1;
}

void load_users_from_file()
{
    FILE* f = fopen(FILE_USERS, "r");
    if (f == NULL)
        return;

    char line[200];
    while (fgets(line, sizeof line, f) != NULL)
    {
        char login[50], password[50];
        if (sscanf(line, "%s %s", login, password) != 2)
            continue;
        if (strcmp(login, ADMIN_LOGIN) == 0)
            continue;
        if (find_user_by_login(login) != -1)
            continue;

        if (user_count == user_capacity)
        {
            int new_user_capacity = (user_capacity == 0) ? 1 : user_capacity * 2;
            struct user* temp = realloc(USERS, new_user_capacity * sizeof(struct user));
            if (temp == NULL)
            {
                fclose(f);
                return;
            }
            USERS = temp;
            user_capacity = new_user_capacity;
        }
        strcpy(USERS[user_count].login, login);
        strcpy(USERS[user_count].password, password);
        user_count++;
    }
    fclose(f);
}

void save_users_to_file(void)
{
    FILE* f = fopen(FILE_USERS, "w");
    if (f == NULL)
    {
        printf("Не удалось сохранить файл пользователей.\n");
        return;
    }
    for (int i = 0; i < user_count; i++)
        fprintf(f, "%s %s\n", USERS[i].login, USERS[i].password);
    fclose(f);
}

int main(void)
{
    load_users_from_file();

    int choice;
    do
    {
        printf("====меню====\n");
        printf("1. создать аккаунт\n");
        printf("2. войти в аккаунт\n");
        printf("3. Выход\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1: create_account(); break;
        case 2: user_vertification(); break;
        case 3: break;
        default:
            printf("Неверный выбор\n");
        }
    }
    while (choice != 3);

    save_users_to_file();
    free(USERS);
    return 0;
}

void create_account()
{
    char login[50];
    char password[50];

    if (user_count == user_capacity)
    {
        int new_user_capacity = (user_capacity == 0) ? 1 : user_capacity * 2;
        struct user* temp = realloc(USERS, new_user_capacity * sizeof(struct user));
        if (temp == NULL)
        {
            printf("error capacity\n");
            return;
        }
        USERS = temp;
        user_capacity = new_user_capacity;
    }
    printf("введите логин: ");
    scanf("%s", login);
    if (strcmp(login, ADMIN_LOGIN) == 0)
    {
        printf("Этот логин зарезервирован для главного администратора.\n");
        return;
    }
    if (find_user_by_login(login) != -1)
    {
        printf("пользователь с таким логином существует.\n");
        return;
    }
    printf("введите пароль: ");
    scanf("%s", password);
    strcpy(USERS[user_count].login, login);
    strcpy(USERS[user_count].password, password);
    user_count++;
    save_users_to_file();
    printf("Аккаунт создан.\n");
}

void user_vertification()
{
    char login[50];
    char password[50];

    printf("введите логин: ");
    scanf("%s", login);
    printf("введите пароль: ");
    scanf("%s", password);

    if (strcmp(login, ADMIN_LOGIN) == 0 && strcmp(password, ADMIN_PASSWORD) == 0)
    {
        printf("Вход выполнен успешно. Режим главного администратора.\n");
        admin_menu();
        return;
    }

    int idx = find_user_by_login(login);
    if (idx == -1)
    {
        printf("Пользователь не найден.\n");
        return;
    }
    if (strcmp(USERS[idx].password, password) == 0)
    {
        printf("Вход выполнен успешно.\n");
    }
    else
    {
        printf("Неверный пароль.\n");
    }
}

int delete_user_by_login(const char* login)
{
    int idx = find_user_by_login(login);
    if (idx < 0)
        return 0;
    for (int i = idx; i < user_count - 1; i++)
        USERS[i] = USERS[i + 1];
    user_count--;
    save_users_to_file();
    return 1;
}

void admin_menu()
{
    int choice;
    do
    {
        printf("----панель администратора----\n");
        printf("1. удалить пользователя\n");
        printf("2. Выйти в главное меню\n");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
        {
            char login[50];
            printf("логин пользователя для удаления: ");
            scanf("%49s", login);
            if (strcmp(login, ADMIN_LOGIN) == 0)
            {
                printf("нельзя удалить учётную запись главного администратора.\n");
                break;
            }
            if (delete_user_by_login(login))
                printf("Пользователь удалён.\n");
            else
                printf("Пользователь с таким логином не найден.\n");
            break;
        }
        case 2:
            break;
        default:
            printf("Неверный выбор\n");
        }
    }
    while (choice != 2);
}
