# 🔍 PatternSeeker

Легковесная header-only библиотека на C++20 для эффективного парсинга строк с предопределёнными паттернами. Идеально подходит для парсинга JSON, XML и пользовательских текстовых форматов без накладных расходов на тяжёлые регулярные выражения или сложные парсеры.

## ✨ Особенности

- **Абстракция с нулевой стоимостью** — использует `std::string_view` для операций без копирования
- **Header-only** — просто подключите и используйте, компиляция не требуется
- **Легковесная** — нет внешних зависимостей, минимальные накладные расходы
- **Гибкое управление указателем** — вы решаете, когда перемещать позицию парсинга
- **Поддержка JSON и XML** — удобные помощники для распространённых форматов
- **Типобезопасный парсинг** — встроенный парсинг целых чисел с обработкой ошибок
- **Готова к C++20** — современный C++ с чистым API

## 🚀 Быстрый старт

```cpp
#include "PatternSeeker.hpp"
#include <iostream>

using namespace PatterSeekerNS;

int main() {
    // Парсинг простого паттерна
    PatternSeeker ps("Имя: Иван, Возраст: 30");

    ps.to("Имя: ", move_after);
    auto name = ps.extract(", ", move_after);
    std::cout << "Имя: " << name << std::endl;  // "Иван"

    ps.to("Возраст: ", move_after);
    auto age = ps.takeUInt64();
    std::cout << "Возраст: " << *age << std::endl;   // 30

    return 0;
}
```

## 📦 Установка

### Использование CMake

```bash
git clone https://github.com/yourusername/PatternSeeker.git
cd PatternSeeker
mkdir build && cd build
cmake .. -DPATTERN_SEEKER_BUILD_TESTS=ON
cmake --build .
```

### Как подмодуль

```bash
git submodule add https://github.com/yourusername/PatternSeeker.git
# Подключите PatternSeeker.hpp в вашем проекте
```

### Прямое включение

Просто скопируйте `PatternSeeker.hpp` в ваш проект и подключите его.

## 📖 Примеры использования

### Парсинг JSON

```cpp
PatternSeeker json(R"({"name": "Алиса", "score": 42, "active": true})");

auto name = json.getJsonProp("name");
std::cout << name << std::endl;  // Алиса

auto score = json.getJsonProp("score").takeUInt64();
std::cout << *score << std::endl;  // 42

auto active = json.getJsonProp("active");
std::cout << active << std::endl;  // true
```

### Парсинг XML

```cpp
PatternSeeker xml("<user id=\"123\"><name>Боб</name></user>");

auto id = xml.getXmlAttr("id");
std::cout << id << std::endl;  // 123

auto name = xml.getXmlTagBody("name");
std::cout << name << std::endl;  // Боб
```

### Извлечение паттернов

```cpp
PatternSeeker ps("Начало [вложенные [данные]] Конец");

// Извлечение между скобками
auto content = ps.extract('[', ']');
std::cout << content << std::endl;  // [вложенные [данные]]

// Извлечение между строками
PatternSeeker ps2("Привет Мир!");
auto word = ps2.extract("Привет ", "!");
std::cout << word << std::endl;  // Мир
```

### Работа с числами

```cpp
PatternSeeker ps("Температура: -15 градусов");

ps.to("Температура: ", move_after);
auto temp = ps.takeInt64();
if (temp) {
    std::cout << "Температура: " << *temp << "°C" << std::endl;  // -15°C
}

// С значением по умолчанию
auto temp2 = ps.takeInt64(0);  // Вернёт 0 при ошибке
```

## 🎯 Ключевые концепции

### Режимы перемещения (Move Modes)

Управляйте моментом перемещения позиции парсинга:

- `none` — не перемещать указатель (по умолчанию)
- `move_before` — переместить к началу совпадения
- `move_after` — переместить за совпадение

```cpp
PatternSeeker ps("Найди это слово");

// Не перемещает указатель
ps.to("это");  // mode = none
std::cout << ps << std::endl;  // "Найди это слово"

// Перемещает за "это "
ps.to("это ", move_after);
std::cout << ps << std::endl;  // "слово"
```

### Дизайн без копирования (Zero-Copy Design)

PatternSeeker внутри использует `std::string_view`, что означает, что данные никогда не копируются. Это делает библиотеку чрезвычайно быстрой, но **вы должны гарантировать, что исходная строка существует дольше всех объектов PatternSeeker**.

```cpp
std::string data = "Важные данные";
PatternSeeker ps(data);

// ❌ ОШИБКА: data будет уничтожена, но ps всё ещё на неё ссылается
auto getDangling = [&]() {
    std::string temp = "Временные данные";
    return PatternSeeker(temp);  // Опасность!
};

// ✅ ПРАВИЛЬНО: data живёт достаточно долго
void processData(const std::string& data) {
    PatternSeeker ps(data);
    // Используем ps здесь
}
```

### Извлечение с вложенностью

Метод `extract(char start, char end)` учитывает вложенность:

```cpp
PatternSeeker ps("{ \"obj\": { \"nested\": 123 } }");

auto obj = ps.extract('{', '}');
std::cout << obj << std::endl;  // { "obj": { "nested": 123 } }
```

## 📚 API справка

### Основные методы

| Метод | Описание |
|-------|----------|
| `size()` | Возвращает размер видимой строки |
| `isEmpty()` | Проверяет, пуста ли строка |
| `to_string()` | Преобразует в `std::string` |
| `to_string_view()` | Возвращает `std::string_view` |

### Навигация

| Метод | Описание |
|-------|----------|
| `expect(str)` | Проверяет и перемещается за `str` |
| `startsWith(str)` | Проверяет без перемещения |
| `to(str, mode)` | Находит `str` и перемещается |
| `skip(n)` | Пропускает `n` символов |
| `skipWhiteSpaces()` | Пропускает пробелы |

### Извлечение

| Метод | Описание |
|-------|----------|
| `extract(from, to, mode)` | Извлекает между строками |
| `extract(to, mode)` | Извлекает до строки |
| `extract(start, end, mode)` | Извлекает с учётом вложенности |
| `extract(size, mode)` | Извлекает N символов |
| `extractUntilOneOf(chars, mode)` | Извлекает до любого из символов |

### Парсинг чисел

| Метод | Описание |
|-------|----------|
| `takeUInt64()` | Парсит беззнаковое число |
| `takeUInt64(def)` | С значением по умолчанию |
| `takeInt64()` | Парсит знаковое число |
| `takeInt64(def)` | С значением по умолчанию |

### JSON и XML

| Метод | Описание |
|-------|----------|
| `getJsonProp(name)` | Извлекает JSON свойство |
| `getXmlTag(name, mode)` | Извлекает весь XML тег |
| `getXmlTagBody(name, mode)` | Извлекает содержимое тега |
| `getXmlAttr(name)` | Извлекает XML атрибут |

## 🧪 Тестирование

```bash
cmake -DPATTERN_SEEKER_BUILD_TESTS=ON ..
cmake --build .
ctest
```

## 💡 Советы по использованию

1. **Для JSON и XML используйте специализированные парсеры** в production-коде для сложных структур
2. **Всегда проверяйте результат** методов `take*()` и `extract()` на пустоту
3. **Используйте `mode=none`** когда хотите "заглянуть вперёд" без изменения позиции
4. **Делайте копии** PatternSeeker (это дёшево) для пробных операций

```cpp
PatternSeeker ps("данные");
auto copy = ps;  // Дёшево, можно пробовать операции на копии

if (copy.to("что-то", move_after)) {
    // Успех, можем обновить оригинал
    ps = copy;
}
```

## 📄 Лицензия

MIT License — см. файл LICENSE для подробностей.

## 🤝 Вклад в проект

Приветствуются любые улучшения! Не стесняйтесь создавать Pull Request.

---

<div align="center">
Сделано с ❤️ для эффективного парсинга строк
</div>
