FROM ubuntu:18.04

RUN yes | unminimize

RUN apt-get update && \
  DEBIAN_FRONTEND=noninteractive apt-get install --yes --no-install-recommends \
    manpages-dev \
    make

RUN DEBIAN_FRONTEND=noninteractive apt-get install --yes \
  gcc

RUN wget https://github.com/Kitware/CMake/releases/download/v3.15.5/cmake-3.15.5-Linux-x86_64.sh \
      -q -O /tmp/cmake-install.sh \
      && chmod u+x /tmp/cmake-install.sh \
      && mkdir /usr/bin/cmake \
      && /tmp/cmake-install.sh --skip-license --prefix=/usr/bin/cmake \
      && rm /tmp/cmake-install.sh
ENV PATH="/usr/bin/cmake/bin:${PATH}"

WORKDIR /test

COPY c/CMakeLists.txt c/main.c c/
RUN cd c \
  && cmake -DCMAKE_C_COMPILER=gcc . \
  && make

COPY sh/pps.sh sh/

ENTRYPOINT ["bash"]
