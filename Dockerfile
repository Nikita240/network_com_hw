# syntax = docker/dockerfile:experimental

FROM ubuntu as builder

WORKDIR /opt/transfer

RUN apt update && DEBIAN_FRONTEND="noninteractive" apt install --no-install-recommends -y  \
    build-essential \
    python3 \
    python3-pip \
    python3-setuptools \
    python3-wheel \
    ninja-build \
    libzmqpp-dev

RUN pip3 install meson

COPY . .

RUN --mount=type=cache,target=builddir meson setup builddir --backend ninja

RUN --mount=type=cache,target=builddir \
    meson compile -C builddir \
    && cp builddir/sender .

CMD ./sender

# RUN meson test -C builddir