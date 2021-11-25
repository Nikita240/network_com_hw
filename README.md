# NFTP

This is an implementation of a custom file transfer protocol using ZeroMQ. We'll call it **Nikita's File Transfer Protocol**. It supports multiple concurrent upload clients, asynchronous packet queing, and handles out of order packets.

The `client` program will upload a specified file using NFTP, then download it back using FTP. It will then compare the hashes of the sent and received file and will display performance metrics.

Performance is about double of FTP (albeit it doesn't offer any of the security features). My measurements show an upload speed of 8-10 Gb/s over a TCP socket, compared to 4-4.5 Gb/s for FTP.

## Prerequisites

Using docker greatly simplifies the burden of getting the code running since we have a server/client model as well as multiple dependencies that have to be handled.

  - docker version 19.03+
  - docker compose version 1.28+
  - BuildKit enabled
  - BuildKit enabled for docker-compose

[docker installation guide](https://docs.docker.com/engine/install/ubuntu/)

[docker-compose installation guide](https://docs.docker.com/compose/install/)

To enable BuildKit, add these 3 environment variables to your .bashrc or equivalent:

```bash
export DOCKER_BUILDKIT=1 # Enable BuildKit
export COMPOSE_DOCKER_CLI_BUILD=1 # Enable BuildKit for docker-compose
export BUILDKIT_PROGRESS=plain # Full build output so you can see compiler build logs
```

## Building and Running

To build the code:

```sh
docker-compose build
```

To run the client with the provided `cad_mesh.stl` file:

```sh
docker-compose run --rm client
```

If you want to really the test the performance, you will need a bigger file:

```sh
# Create a 1 GB empty file
dd if=/dev/urandom of=testdata bs=1M count=1024
# Run the client with a specified endpoint and filepath
docker-compose run --rm "tcp://server:5557" "testdata"
```

You should see an output like this:

```sh
➜ docker-compose run --rm client client "tcp://server:5557" "files/testdata"
[+] Running 2/0
 ⠿ Container transfer-server-1  Running                                                                                                                                                                     0.0s
 ⠿ Container transfer-ftp-1     Running                                                                                                                                                                     0.0s
Uploading file with ZeroMQ: 100%
Transmission speed: 9.33 Gb/s
Done
Downloading file with FTP:
Transmission speed: 3.22 Gb/s
Done
Sent file hash:     J*�5����6#E���
Received file hash: J*�5����6#E���
```