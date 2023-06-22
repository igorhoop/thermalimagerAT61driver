Задание переменных среды
export AT61F_CAPTURE_PATH=/home/hoop/MY_PROG_PROJECTS/AT61F/development/photos/
export AT61F_CONFIG_PATH=/home/hoop/MY_PROG_PROJECTS/AT61F/development/config/config


Программа ищет файл конфигурации по пути:
/etc/at61f/config
При запуске контейнера нужно смонтировать каталог с таким файлом по этому пути.

Требуется задать путь к config-файлу, посредством переменной среды:
AT61F_CONFIG_PATH=


Также требуется задать путь для сохранения снимков, посредством переменной среды
AT61F_CAPTURE_PATH
Контейнер создан с уже определенной перемен ной среды
AT61F_CAPTURE_PATH=./photos/
При запуске контейнера желаемую папку хост-машины монтируем в эту контейнерскую папку.


docker run -itp 5000:30001 --mount type=bind,src=./config/,target=/etc/at61f --mount type=bind,src=/home/hoop/TEPLOVIZOR-PHOTO/,target=/home/photos/ at61f /bin/bash






