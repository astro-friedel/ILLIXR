#!/usr/bin/env python3

import argparse
import os
import docker
from docker.errors import DockerException, ImageNotFound

from images import docker_images, make_list


def parse_args(ch):
    parser = argparse.ArgumentParser(description="Generate Dockerfiles for ILLIXR")
    parser.add_argument("--os", action='store', default='all',
                        help="Operating system(s) to generate Dockerfiles for. "
                             "If not specified, all operating systems will be generated. "
                             f"Choices are: {', '.join(ch)}, all")
    return parser.parse_args()


def build(images):
    # try to connect to Docker daemon
    try:
        client = docker.DockerClient(base_url=f'unix://{os.getenv("HOME")}/.docker/desktop/docker.sock')
    except DockerException as _:
        try:
            client = docker.DockerClient(base_url='unix:///var/run/docker.sock')
        except DockerException as _:
            try:
                client = docker.DockerClient(base_url='tcp://127.0.0.1:1234')
            except DockerException as _:
                raise RuntimeError("Could not connect to a Docker daemon. Please start Docker and try again.")
    print("Searching for ILLIXR Docker images...")
    for image in images:
        ops, ver = image.lower().split("-")
        try:
            img = client.images.get(f"illixr_{ops}:{ver}")
            print(f"Image illixr_{ops}:{ver} already exists. Removing previous version...")
            images.remove(img)
        except ImageNotFound as _:
            pass
    print("Building Docker images...")
    for image in images:
        ops, ver = image.lower().split("-")
        print(f"Building illixr_{ops}:{ver}...")
        client.images.build(path=f"{ops}/{ver}", tag=f"illixr_{ops}:{ver}")
    print("  Done")


if __name__ == "__main__":
    opers = list(docker_images.keys())
    choices = [f"{x}-{y}" for x, z in docker_images.items() for y in z.keys()]
    args = parse_args(opers + choices)

    if args.os is None or args.os.upper() == 'ALL':
        to_build = set(choices)
    else:
        to_build = make_list(args.os.split(','), opers, choices)

    build(to_build)
