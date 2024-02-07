#!/usr/bin/env python3

import argparse
import json
import os
import sys

from images import docker_images, make_list

script_directory = os.path.dirname(os.path.abspath(sys.argv[0]))


def read_modules(filename: str):
    with open(os.path.join(script_directory, filename), 'r') as fh:
        return json.load(fh)


def generate_file(ops: str, vers: str, mods: dict):
    deps = set()
    for dep in mods['dependencies']:
        for item in list(dep.values())[0].values():
            if item[ops][vers]['pkg']:
                deps.add(item[ops][vers]['pkg'])
    os.makedirs(f"{os.path.join(ops, vers)}", exist_ok=True)
    with open(os.path.join(script_directory, ops, vers, "Dockerfile"), 'w') as fh:
        fh.write(f"FROM {docker_images[ops][vers]['image']}\n")
        fh.write("USER root\n\n")
        if 'arg' in docker_images[ops]:
            fh.write("# set up environment vars\n")
            fh.write(f"ARG {docker_images[ops]['arg']}\n\n")
        if docker_images[ops][vers]['pre']:
            fh.write("# setup and install global dependencies and 3rd party repos and libraries\n")
            for cmd in docker_images[ops][vers]['pre']:
                fh.write(f"RUN {cmd}\n")
            fh.write("\n")
        fh.write("# install dependencies\n")
        fh.write(f"RUN {docker_images[ops]['cmd']} {' '.join(deps)}\n\n")
        if docker_images[ops][vers]['post']:
            fh.write("# post install steps\n")
            for cmd in docker_images[ops][vers]['post']:
                fh.write(f"RUN {cmd}\n")
            fh.write("\n")
        fh.write("RUN useradd -ms /bin/bash illixr\n")

        fh.write("USER illixr\n")
        fh.write("WORKDIR /home/illixr\n")
        fh.write("RUN git clone -b convertBuildToCmake https://github.com/ILLIXR/ILLIXR.git\n")
        fh.write("COPY --chown=illixr cmk.sh .\n")
        fh.write("RUN chmod 744 cmk.sh\n")


def parse_args(ch):
    parser = argparse.ArgumentParser(description="Generate Dockerfiles for ILLIXR")
    parser.add_argument("-f", "--file", action='store',
                        default='modules-comp.json', help="JSON file containing module dependencies")
    parser.add_argument("--os", action='store', default='all',
                        help="Operating system(s) to generate Dockerfiles for. "
                             "If not specified, all operating systems will be generated. "
                             f"Choices are: {', '.join(ch)}, all")
    return parser.parse_args()


def worker(o_systems, choice, mods):
    if choice is None or args.os.upper() == 'ALL':
        for op, versions in o_systems.items():
            for version in versions:
                generate_file(op, version, mods)
                created_versions.append(f"{op}-{version}")
    else:
        selected = choice.split(',')
        for item in selected:
            if "-" in item:
                sel_op, sel_ver = item.split("-")
            else:
                sel_op = item
                sel_ver = None
            found = False
            for op, versions in o_systems.items():
                if sel_op.upper() == op.upper():
                    if sel_ver is None:
                        for version in versions:
                            generate_file(op, version, mods)
                            found = True
                            created_versions.append(f"{op}-{version}")
                    elif sel_ver in versions:
                        generate_file(op, sel_ver, mods)
                        found = True
                        created_versions.append(f"{op}-{sel_ver}")
                    else:
                        raise ValueError(f"Version {sel_ver} not found for {sel_op}")
            if not found:
                raise ValueError(f"Operating system {sel_op} not found")
        print(f"   {','.join(created_versions)}")
        print("  Done")
    return created_versions


if __name__ == "__main__":
    opers = list(docker_images.keys())
    choices = [f"{x}-{y}" for x, z in docker_images.items() for y in z.keys()]

    args = parse_args(opers + choices)
    if args.os.upper() == 'ALL':
        to_create = choices
    else:
        to_create = make_list(args.os.split(","), opers, choices)

    modules = read_modules(args.file)
    print("Generating Dockerfiles for requested operating systems...")
    created_versions = []
    operating_systems = {}
    for i in modules['systems']:
        operating_systems[i['name']] = i['versions']

    created_versions = worker(operating_systems, to_create, modules)
