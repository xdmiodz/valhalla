FROM ubuntu:xenial

RUN mkdir -p /code/valhalla/build /code /valhalla
COPY . /code/valhalla
COPY ./run.sh /valhalla/run.sh

RUN  apt-get update && \
    apt-get install -y \
    cmake \
    make \
    libtool \
    pkg-config \
    g++ \
    gcc \
    lcov \
    protobuf-compiler \
    libboost-all-dev \
    libcurl4-openssl-dev \
    zlib1g-dev \
    liblz4-dev \
    libprotobuf-dev \
    libgeos-dev \
    libgeos++-dev \
    liblua5.2-dev \
    libspatialite-dev \
    libsqlite3-dev \
    git-core \
    automake \
    libzmq3-dev \
    libczmq-dev \
    python \
    wget \
    ccache && \
    cd /code && git clone https://github.com/kevinkreiser/prime_server.git && \
    cd prime_server && git submodule update --init --recursive && \
    ./autogen.sh && ./configure && make install && \
    cd /code/valhalla/build && cmake  -DENABLE_NODE_BINDINGS=Off .. && \
    make all install && \
    apt-get remove -y cmake \
    make libtool g++ gcc lcov \
    git-core \
    automake && \
    rm -rf /var/lib/apt/lists/* && \
    rm -rf /root/.cache && \
    rm -rf /code && \
    apt autoremove  -y

WORKDIR /valhalla
