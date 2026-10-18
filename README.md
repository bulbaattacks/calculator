# calculator

Консольный калькулятор для целых чисел типа `int`. Принимает два числа и операцию через ключи командной строки и печатает результат. Переполнение `int` и недопустимые аргументы обнаруживаются и выводятся как ошибки.

## Операции

| Операция | Ключ `-o` | Что считает | Ограничения |
|---|---|---|---|
| Сложение | `+` | `a + b` | |
| Вычитание | `-` | `a - b` | |
| Умножение | `*` | `a * b` | |
| Деление | `/` | `a / b` | `b != 0`, дробная часть отбрасывается |
| Степень | `^` | `a` в степени `b` | `b >= 0` |
| Факториал | `!` | `a!` | `a >= 0`, ключ `-b` не используется |

## Использование

```
calculator-app -a NUM -o OP [-b NUM]
calculator-app -h
```

| Ключ | Значение |
|---|---|
| `-a NUM` | первое число (`int`) |
| `-b NUM` | второе число (`int`), для факториала не нужно |
| `-o OP` | операция: `+ - * / ^ !` |
| `-h` | показать справку и выйти |


### Примеры

```console
$ ./build/calculator-app -a 5 -b 10 -o +
Result: 15
$ ./build/calculator-app -a 7 -b 2 -o /
Result: 3
$ ./build/calculator-app -a 2 -b 10 -o '^'
Result: 1024
$ ./build/calculator-app -a 5 -o '!'
Result: 120
$ ./build/calculator-app -a 2147483647 -b 1 -o +
Error: result is overflow
$ ./build/calculator-app -a 1 -b 0 -o /
Error: some argument is invalid. Use -h for help.
```

## Ошибки

| Сообщение | Причина |
|---|---|
| `Error: result is overflow` | результат не помещается в `int` |
| `Error: some argument is invalid. Use -h for help.` | неизвестная операция, не указан `-o`, деление на 0, отрицательная степень или отрицательное число для факториала |

## Сборка

Требования:
- Linux или macOS (используется POSIX-функция `getopt`);
- CMake 3.16 или новее;
- компилятор с поддержкой C++17 (GCC или Clang);
- Git и доступ в интернет: при настройке CMake скачивает библиотеку `ar_math`;
- необязательно: Ninja, `clang-format` и `clang-tidy`.

```sh
cmake -B build -G Ninja
cmake --build build
```

Без Ninja уберите `-G Ninja`, тогда сборка пойдёт через `make`.

Опции настройки:

| Опция | По умолчанию | Что делает |
|---|---|---|
| `USE_CLANG_FORMAT` | `OFF` | форматирует исходники через clang-format перед каждой сборкой |
| `USE_CLANG_TIDY` | `OFF` | проверяет исходники через clang-tidy во время компиляции |

Пример: `cmake -B build -G Ninja -DUSE_CLANG_FORMAT=ON -DUSE_CLANG_TIDY=ON`

Установка в `/usr/local/bin`:

```sh
sudo cmake --install build
```

## Зависимости

Математические функции вынесены в отдельную header-only библиотеку [ar_math](https://github.com/bulbaattacks/ar_math). Она подключается автоматически через CMake FetchContent, устанавливать её вручную не нужно.

## Структура

```
CMakeLists.txt    сборка и подключение ar_math
src/main.cpp      точка входа
src/app.h         объявление app::applicationRun
src/app.cpp       разбор аргументов, вызов операций, вывод
```