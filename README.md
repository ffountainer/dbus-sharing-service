# dbus-sharing-service

## Краткое описание проекта

- Проект имплементирует DBus сервис для передачи сервисам-обработчикам файлов с определенными расширениями, с которыми они могут работать (в нашем случае, открывать).

## Структура

```bash
.
├── build/ # папка со сборкой (появится после прогонки cmake с помощью скрипта)
├── build.sh # скрипт для компиляции проекта
├── CMakeLists.txt # конфигурация для cmake
├── example/
│   ├── CMakeLists.txt
│   └── main.cpp # пример сервиса-обработчика
├── lib
│   ├── build/
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── dbussharing.hpp
│   └── src/
│       └── dbussharing.cpp # имплементация библиотеки
├── README.md
├── src/
│   ├── logic.cpp # вспомогательный файл, чтобы вынести часть логики из service.cpp
│   ├── main.cpp # точка входа, где инициализируется основной сервис
│   ├── service.cpp # имплементация методов
│   └── sharing_service.hpp
└── test-files/ # файлы для тестов
    ├── book.pdf
    └── video.mp4
```

## Компиляция осуществляется с помощью скрипта в корне проекта

- сначала предоставьте файлу право на запуск:

```bash
chmod +x ./build.sh
```

- затем запустите компиляцию:

```bash
./build.sh
```

- после этого проект полностью собран, и можно запускать сервисы;


## Тестирование

- зайдите в папку со сборкой:

```bash
cd build
```

- запустите основной сервис com.system.sharing:

```bash
./dbus-sharing-service
```

- далее в новом окне терминала запустите пример сервиса-обработчика:

```bash
cd build
./example/example-service
```
- откройте новое окно терминала;

### примеры успешных тестов (ожидаемый результат: ())

- для успешного тестового запуска вы можете использовать следующие команды:

```bash
# 1
gdbus call --session --dest com.system.sharing --object-path / --method com.system.Sharing.openFile "/repo-with-project/dbus-sharing-service/test-files/book.pdf"
```
repo-with-project -- это директория, в которую вы загрузили этот проект, например, для меня эта команда выглядит так:

```bash
 gdbus call --session --dest com.system.sharing --object-path / --method com.system.Sharing.openFile "/home/fountainer/dbus/dbus-sharing-service/test-files/book.pdf"
```
```bash
# 2
gdbus call --session \
  --dest com.example.reader \
  --object-path / \
  --method com.example.reader.openFile \
  "/home/fountainer/dbus/dbus-sharing-service/test-files/book.pdf"
```
- для запуска команды с одной из ошибок запустите:

```bash
 gdbus call --session --dest com.system.sharing --object-path / --method com.system.Sharing.openFile "/repo-with-project/dbus-sharing-service/test-files/video.mp4"
```
Ожидаемый результат:
```bash
Error: GDBus.Error:com.system.Sharing.Error: OpenFile: there is no service that can open your file :(
```
