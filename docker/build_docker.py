#!/usr/bin/env python3
"""
Buiuld docker images for the specified operating systems and versions
"""
import argparse
import json
import os
import sys
from docker import APIClient
from docker.errors import DockerException

from images import docker_images, make_list

script_directory = os.path.dirname(os.path.abspath(sys.argv[0]))


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
        client = APIClient(base_url=f'unix://{os.getenv("HOME")}/.docker/desktop/docker.sock')
    except DockerException as _:
        try:
            client = APIClient(base_url='unix:///var/run/docker.sock')
        except DockerException as _:
            try:
                client = APIClient(base_url='tcp://127.0.0.1:1234')
            except DockerException as _:
                raise RuntimeError("Could not connect to a Docker daemon. Please start Docker and try again.")
    print("Searching for ILLIXR Docker images...")
    for image in images:
        ops, ver = image.lower().split("-")
        img = client.images(f"illixr_{ops}:{ver}")
        if img:
            img = img[0]
            print(f"Image illixr_{ops}:{ver} already exists. Removing previous version...")
            client.remove_image(img['Id'], force=True, noprune=True)
    print("Building Docker images...")
    for image in images:
        ops, ver = image.split("-")
        print(f"Building illixr_{ops}:{ver}...")
        for line in client.build(path=f"{os.path.join(script_directory, 'OS', ops, ver)}", tag=f"illixr_{ops.lower()}:{ver}"):
            data = line.decode("utf-8").replace("\\n", "")
            data = data.split("\r")
            for d in data:
                d = d.strip()
                if not d:
                    continue
                report = json.loads(d)
                if "stream" in report:
                    print(report["stream"])
                elif "error" in report:
                    raise RuntimeError(f"Error: {report['error']}")
                else:
                    print("\n")
    print("  Done")


if __name__ == "__main__":
    opers = list(docker_images.keys())
    choices = [f"{x}-{y}" for x, z in docker_images.items() for y in z["versions"].keys()]
    args = parse_args(opers + choices)

    if args.os is None or args.os.upper() == 'ALL':
        to_build = set(choices)
    else:
        to_build = make_list()  # args.os.split(','), opers, choices)

    build(to_build)
