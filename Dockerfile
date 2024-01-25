# syntax=docker/dockerfile:1

# берем базовый образ
FROM ubuntu:20.04

# что необходимо сделать в базовом образе
RUN apt update -y && apt install build-essential -y && apt-get install -y x11-apps && apt install -y iputils-ping

###

# сменить директорию для 2-х случаев: использовать путь при построении образа (дальнейшие команды RUN) а так же использовать этот путь после запуска контейнера
# однако в случае пути после запуска контейнера его можно изменить при запуске опцией -w /path
# если каталога не существует он создается
#WORKDIR /home/roboserv-install

WORKDIR /home/AT61f
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



# устанавливам SFML
RUN apt-get -y install libsfml-dev

# устанавливаем ffmpeg
RUN apt -y install ffmpeg

# устанавливаем графический сервер
#RUN apt-get -y install xorg openbox

#RUN make
#RUN mv ./build/roboserv /usr/bin && mkdir /home/roboserv && #mv www ../roboserv/
#RUN rm -r /home/roboserv-install

#WORKDIR /home/

ENV AT61F_CONFIG_PATH=/etc/at61f/config/config_for_run_on_docker.json
#ENV AT61F_CAPTURE_PATH=../photos999/
#ENV AT61F_VIDEO_PATH=../videos999/
#ENV AT61F_LOG_PATH=../logs999/log


### КУСОК ИЗ ПРИМЕРА ЧТОБ ЗАПУСКАТЬ X11
#ARG user=split
#ARG home=/home/$user
#RUN groupadd -g 1000 $user
#RUN useradd -d $home -s /bin/bash -m $user -u 1000 -g 1000 \
# && echo $user:ubuntu | chpasswd \
# && adduser $user sudo
#WORKDIR $home
#USER $user
#ENV HOME $home


#RUN touch file

CMD ["./at61f"]
