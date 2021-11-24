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
    pkg-config

RUN pip3 install meson conan

COPY . .

RUN --mount=type=cache,target=build \
    conan install . --install-folder build -g deploy

RUN --mount=type=cache,target=build \
    conan build . --build-folder build \
    && mkdir bin \
    && cp -R build build_out

# ******************************************************************************

FROM ubuntu as runner

WORKDIR /opt/transfer

# Copy dependency libraries
COPY --from=builder /opt/transfer/build_out/*/lib/* /usr/lib

# Copy our binaries
COPY --from=builder /opt/transfer/build_out/server /usr/bin
