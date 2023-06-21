# syntax=docker/dockerfile:1

# ФАЙЛ ДЛЯ СОЗДАНИЯ ОБРАЗА СЕРВЕРА АКТИВАЦИИ И ЛИЦЕНЗИРОВАНИЯ

# берем базовый образ
FROM ubuntu:20.04

# что необходимо сделать в базовом образе
RUN apt update -y && apt install build-essential -y


# сменить директорию для 2-х случаев: использовать путь при построении образа (дальнейшие команды RUN) а так же использовать этот путь после запуска контейнера
# однако в случае пути после запуска контейнера его можно изменить при запуске опцией -w /path
# если каталога не существует он создается
#WORKDIR /home/roboserv-install

WORKDIR /home/AT61F
RUN mkdir /usr/local/lib/IRTInfraredSDK

# похоже что это копирование: с машины на которой происходит build в сам образ
#COPY . .

COPY ./build/at61f .
COPY ./AT61F_SDK_v105/libs/* /usr/local/lib/IRTInfraredSDK
COPY ./AT61F_SDK_v105/additional_libs/* /lib/x86_64-linux-gnu

#RUN touch /etc/ld.so.
RUN echo "/usr/local/lib/IRTInfraredSDK" > /etc/ld.so.conf.d/IRTInfrared.conf
RUN ldconfig

# устанавливаем необходимое для настройки часового пояса
ENV TZ=Europe/Moscow
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y tzdata




#RUN make
#RUN mv ./build/roboserv /usr/bin && mkdir /home/roboserv && #mv www ../roboserv/
#RUN rm -r /home/roboserv-install

#WORKDIR /home/

ENV AT61F_CAP_PATH=../photos999/

CMD ["./at61f"]
