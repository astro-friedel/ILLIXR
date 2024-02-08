#!/usr/bin/env python3
"""
Generate Dockerfiles for a given set of operating systems and versions
"""
import argparse
import json
import os
import sys

from images import docker_images, make_list

script_directory = os.path.dirname(os.path.abspath(sys.argv[0]))


def read_modules(filename: str):
    """Read in the JSON file containing the operating system descriptions and ILLIXR dependencies

    :param filename: name of the JSON file to read
    :type filename: str
    :return: dictionary containing the operating system descriptions and ILLIXR dependencies
    :rtype: dict
    """
    with open(os.path.join(script_directory, filename), 'r') as fh:
        return json.load(fh)


def generate_file(ops: str, vers: str, mods: dict):
    """Generate a Dockerfile for a given operating system and version

       :param ops: operating system to generate the Dockerfile for
       :type ops: str
       :param vers: version of the operating system to generate the Dockerfile for
       :type vers: str
       :param mods: dictionary containing the operating system descriptions and ILLIXR dependencies
       :type mods: dict
    """
    deps = set()
    # Get the dependencies for the given operating system and version
    for dep in mods['dependencies']:
        for item in list(dep.values())[0].values():
            if item[ops][vers]['pkg']:
                deps.add(item[ops][vers]['pkg'])
    os.makedirs(f"{os.path.join('OS', ops, vers)}", exist_ok=True)
    # Write the Dockerfile for the given operating system and version
    with open(os.path.join(script_directory, 'OS', ops, vers, "Dockerfile"), 'w') as fh:
        fh.write(f"FROM {docker_images[ops]['versions'][vers]['image']}\n")
        fh.write("USER root\n\n")
        if 'arg' in docker_images[ops]:
            fh.write("# set up environment vars\n")
            fh.write(f"ARG {docker_images[ops]['arg']}\n\n")
        # if there are things that need to be done before installing dependencies
        if docker_images[ops]['versions'][vers]['pre']:
            fh.write("# setup and install global dependencies and 3rd party repos and libraries\n")
            for cmd in docker_images[ops]['versions'][vers]['pre']:
                fh.write(f"RUN {cmd}\n")
            fh.write("\n")
        fh.write("# install dependencies\n")
        fh.write(f"RUN {docker_images[ops]['cmd']} {' '.join(deps)}\n\n")
        # if there are things that need to be done after installing dependencies
        if docker_images[ops]['versions'][vers]['post']:
            fh.write("# post install steps\n")
            for cmd in docker_images[ops]['versions'][vers]['post']:
                fh.write(f"RUN {cmd}\n")
            fh.write("\n")
        fh.write("RUN useradd -ms /bin/bash illixr\n")

        fh.write("USER illixr\n")
        fh.write("WORKDIR /home/illixr\n")
        fh.write("RUN git clone https://github.com/ILLIXR/ILLIXR.git\n")
        fh.write("COPY --chown=illixr --chmod=744 ../../../build.sh .\n")
        #fh.write("RUN chmod 744 build.sh\n")
        fh.write("RUN echo \"export PATH=$PATH:/home/illixr/ilxr/bin\" >> ~/.bashrc\n")
        fh.write("RUN echo \"export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/illixr/ilxr/lib\" >> ~/.bashrc\n")


def parse_args(ch):
    """Parse the command line arguments

    :param ch: list of choices for the operating systems
    :type ch: list

    :return: parsed command line arguments
    :rtype: argparse.Namespace
    """
    parser = argparse.ArgumentParser(description="Generate Dockerfiles for ILLIXR")
    parser.add_argument("-f", "--file", action='store',
                        default='modules-comp.json', help="JSON file containing module dependencies")
    parser.add_argument("--os", action='store', default='all',
                        help="Operating system(s) to generate Dockerfiles for. "
                             "If not specified, all operating systems will be generated. "
                             f"Choices are: {', '.join(ch)}, all")
    return parser.parse_args()


def worker(o_systems, choice, mods):
    """Generate the Dockerfiles for the given operating systems and versions

    :param o_systems: dictionary containing the operating system descriptions
    :type o_systems: dict
    :param choice: list of operating systems and versions to generate Dockerfiles for
    :type choice: list
    :param mods: dictionary containing the operating system descriptions and ILLIXR dependencies
    :type mods: dict
    """
    cver = []
    # if all operating systems are to be generated
    print(choice)
    if choice is None or choice == 'ALL':
        for op, versions in o_systems.items():
            for version in versions:
                generate_file(op, version, mods)
                cver.append(f"{op}-{version}")
    else:
        if isinstance(choice, str):
            selected = choice.split(',')
        else:
            selected = choice
        for item in selected:
            print(item)
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
                            cver.append(f"{op}-{version}")
                    elif sel_ver in versions:
                        generate_file(op, sel_ver, mods)
                        found = True
                        cver.append(f"{op}-{sel_ver}")
                    else:
                        raise ValueError(f"Version {sel_ver} not found for {sel_op}")
            if not found:
                raise ValueError(f"Operating system {sel_op} not found")

        print(f"   {','.join(cver)}")
        print("  Done")
    return cver


if __name__ == "__main__":
    opers = list(docker_images.keys())
    choices = [f"{x}-{y}" for x, z in docker_images.items() for y in z["versions"].keys()]

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
