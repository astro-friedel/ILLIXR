This directory contains Docker files which can be used to create Docker image for Ubuntu 22.04
Each container has the default user `illixr`, and a script called `cmk.sh` in the home directory. This script can be used to configure, build, and install ILLIXR in the container.
Note that this requires [Docker](https://docker.com) to be installed.

To build the container run the following:

```bash
docker build -t illixrubuntu:22 -f ubuntu/22/Dockerfile .
```

To run the container
```bash
docker run -ti -p 8181:8181 illixrubuntu:22
```
