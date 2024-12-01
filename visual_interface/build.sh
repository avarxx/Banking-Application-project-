#!/bin/bash

# Создаём папку build, если она не существует
mkdir -p build
cd build

# Запускаем CMake и сборку
cmake ..
make

# Запускаем приложение, если сборка успешна
if [ -f BankingSystem ]; then
    ./BankingSystem
else
    echo "Ошибка: Исполняемый файл BankingSystem не создан."
fi
