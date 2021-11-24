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
    pkg-config \
    cmake \
    libzmqpp-dev

RUN pip3 install meson conan

COPY . .

RUN conan install .

RUN --mount=type=cache,target=build \
    conan build . \
    && mkdir bin \
    && cp build/server bin/ \
    && cp build/client bin/

# ******************************************************************************

FROM ubuntu as runner

WORKDIR /opt/transfer

RUN apt update && DEBIAN_FRONTEND="noninteractive" apt install --no-install-recommends -y  \
    libzmqpp4

COPY --from=builder /opt/transfer/bin .
