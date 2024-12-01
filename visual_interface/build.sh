#!/bin/bash

mkdir -p build
cd build

cmake ..
make

if [ -f BankingSystem ]; then
    ./BankingSystem
else
    echo "Ошибка: Исполняемый файл BankingSystem не создан."
fi
