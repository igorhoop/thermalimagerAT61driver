## Сборка
```
mkdir build
make
```



## Запуск без докера

1) Перед запуском нужно создать переменную среды **AT61F_CONFIG_PATH**, содержащую путь к файлу конфигурации ***config.json**. Я использовал для этого файл `forsourse`, который нужно ""сорснуть"":
```
source forsource
```
Если эту переменную не создать, программа завершится с уведомлением о необходимости наличия этой переменной.

2) В файле конфигурации задаются параметры для подключения к тепловизору и параметры относящиеся к работе программы: пути сохранения скриншотов, логов, записей видео, URL сервера, куда нужно заливать RTSP поток, чтобы сервер в свою очередь уже раздавал поток клиентам.

3) Запускаем
```
./at61f
```


## Запуск с докером

Dockerfile сделан таким образом, что создает переменную окружения с путем к конфигурационному файлу:
```
AT61F_CONFIG_PATH=/etc/at61f/config/config_for_run_on_docker.json
```
Соответственно по этому пути в контейнере нужно обеспечить существование этого файла. Это достигается путем монтирования каталога содержащего этот конфигурационный файл с хост-системы внутрь контейнера. То есть при запуске контейнера будем делать так:
```
--mount type=bind,src=./config/,target=/etc/at61f/config
```
где `src` - это путь к каталогу с файлом конфигурации на хост-системе (каталог должен содержать файл config_for_run_on_docker.json), а `target` - это путь куда примонтировать этот каталог в самом контейнере.

Здесь важно понимать что при запуске программа **at61f** будет искать конфигурационный файл по пути, указанному в переменной окружения `AT61F_CONFIG_PATH`. И так как докер-контейнер собран с этой переменной окружения, содержащей `/etc/at61f/config`, мы должны обеспечить чтобы этот файл там был.

```note
Можно изменить содержание этой переменной в Dockerfile на новый путь и пересобрать образ. Тогда при запуске контейнера нужно примонтировать каталог с конфигурационным файлом в новый путь.
```

Помимо монтирования каталога с конфигурационным файлом, нужно аналогичным образом примонтировать еще несколько каталогов: для фото, видео и логов. Это необходимо чтобы медиа-данные сохранялись на хост-системе, а не потерялись вместе с завершением работы контейнера. Конфигурационный файл мы тоже храним и редактируем в хост-системе, чтобы каждый раз не настраивать его внутри контейнера после запуска этого контейнера.


В итоге запускаем примерно так:

```

1) Запускаем контейнер и получаем только доступ к терминалу. Программа at61f не запускается, потому что подменена командой /bin/bash:

docker run -itp 5000:30001 --mount type=bind,src=./config/,target=/etc/at61f/config --mount type=bind,src=/home/hoop/TEPLOVIZOR-PHOTO/,target=/home/AT61f/photos/ --mount type=bind,src=/home/hoop/TEPLOVIZOR-VIDEO/,target=/home/AT61f/video/ --mount type=bind,src=/home/hoop/TEPLOVIZOR-LOGS/,target=/home/AT61f/logs/ at61f_rtsp_version /bin/bash

2) Запускаем контейнер и получаем только доступ к терминалу. Программа at61f запускается и мы видим ее вывод:

docker run -itp 5000:30001 --mount type=bind,src=./config/,target=/etc/at61f/config --mount type=bind,src=/home/hoop/TEPLOVIZOR-PHOTO/,target=/home/AT61f/photos/ --mount type=bind,src=/home/hoop/TEPLOVIZOR-VIDEO/,target=/home/AT61f/video/ --mount type=bind,src=/home/hoop/TEPLOVIZOR-LOGS/,target=/home/AT61f/logs/ at61f_rtsp_version

3) Запускаем контейнер в фоне, без получения доступа к терминалу:
docker run -dp 5000:30001 --mount type=bind,src=./config/,target=/etc/at61f/config --mount type=bind,src=/home/hoop/TEPLOVIZOR-PHOTO/,target=/home/AT61f/photos/ --mount type=bind,src=/home/hoop/TEPLOVIZOR-VIDEO/,target=/home/AT61f/video/ --mount type=bind,src=/home/hoop/TEPLOVIZOR-LOGS/,target=/home/AT61f/logs/ at61f_rtsp_version

4) Запуск на AGILEX NAV MODULE
docker run -dp 5000:30001 --mount type=bind,src=/home/gigaipc/at61f_rtsp_version/config/,target=/etc/at61f/config --mount type=bind,src=/home/gigaipc/igor_new/at61f_photos/,target=/home/AT61f/photos/ --mount type=bind,src=/home/gigaipc/igor_new/at61f_video/,target=/home/AT61f/video/ --mount type=bind,src=/home/gigaipc/igor_new/at61f_logs/,target=/home/AT61f/logs/ at61f_rtsp_version


```

Здесь происходит проброс портов в контейнер. То есть пакеты прилетающие на 5000 порт хост-системы перенаправляются на 30001 порт контейнера. 30001 порт используется программой для общения со внешним миром.


## Дополнительная информация

Построение контейнера: 
```
docker build -t at61f_rtsp_version .
```

Создание образа (архива) с контейнером (для дистрибьюции):
```
docker save -o at61f_rtsp_version.tar at61f_rtsp_version
```

Установка образа из архива:
```
docker load -i at61f_rtsp_version.tar
```












