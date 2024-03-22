"""Module containing shared code for generating Dockerfile files and building Docker images."""

# The following dictionary contains the Docker images to be built.
# The lists in the "pre" entries will be inserted into the Dockerfile before the main package install command, usually setting
# up 3rd party repositories.
# The lists in the "post" entries will be inserted into the Dockerfile after the main package install command, usually
# getting and installing 3rd party libraries which are not available in the standard repositories.
docker_images = {"Ubuntu": {"versions": {"22": {"image": "ubuntu:jammy",
                                                "pre": ["apt-get update",
                                                        "apt-get -y install curl apt-transport-https lsb-core",
                                                        "sudo add-apt-repository ppa:ecal/ecal-latest -y -u",
                                                        "sudo apt-get update",
                                                        "mkdir -p /etc/apt/keyrings",
                                                        "curl -sSf https://librealsense.intel.com/Debian/librealsense.pgp | tee /etc/apt/keyrings/librealsense.pgp > /dev/null"
                                                        "echo \"deb [signed-by=/etc/apt/keyrings/librealsense.pgp] https://librealsense.intel.com/Debian/apt-repo `lsb_release -cs` main\" | tee /etc/apt/sources.list.d/librealsense.list > /dev/null",
                                                        "more /etc/apt/sources.list.d/librealsense.list",
                                                        "apt-get update"],
                                                "post": ["wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-ubuntu2204.pin",
                                                         "mv cuda-ubuntu2204.pin /etc/apt/preferences.d/cuda-repository-pin-600",
                                                         "wget https://developer.download.nvidia.com/compute/cuda/11.8.0/local_installers/cuda-repo-ubuntu2204-11-8-local_11.8.0-520.61.05-1_amd64.deb",
                                                         "dpkg -i cuda-repo-ubuntu2204-11-8-local_11.8.0-520.61.05-1_amd64.deb",
                                                         "cp /var/cuda-repo-ubuntu2204-11-8-local/cuda-*-keyring.gpg /usr/share/keyrings/",
                                                         "apt-get update",
                                                         "apt-get -y install cuda",
                                                         "rm cuda-repo-ubuntu2204-11-8-local_11.8.0-520.61.05-1_amd64.deb",
                                                         "wget https://stereolabs.sfo2.cdn.digitaloceanspaces.com/zedsdk/4.0/ZED_SDK_Ubuntu22_cuda11.8_v4.0.8.zstd.run",
                                                         "chmod +x ZED_SDK_Ubuntu22_cuda11.8_v4.0.8.zstd.run",
                                                         "./ZED_SDK_Ubuntu22_cuda11.8_v4.0.8.zstd.run -- silent"
                                                         "rm ZED_SDK_Ubuntu22_cuda11.8_v4.0.8.zstd.run"
                                                ]},
                                         },
                            "cmd": "apt-get -y install",
                            "arg": "DEBIAN_FRONTEND=noninteractive"}
                 }


def make_list():  # ops, opers, choices):
    """Generate a list of operating systems from the inputs. Does error checking and expansion of general OS names.
       e.g. "Ubuntu" -> "Ubuntu-20", "Ubuntu-22"

       NOTE: Currently only Ubuntu 22.04 is supported.

       # :param ops: listing of operating systems to build
       # :type ops: list
       # :param opers: listing of operating systems to choose from (e.g. "Ubuntu", "Fedora", "CentOS")
       # :type opers: list
       # :param choices: listing of specific operating system versions to choose from (e.g. "Ubuntu-20", "Ubuntu-22", "Fedora-37", "Fedora-38", "CentOS-9")

       :return: listing of operating systems to build
       :rtype: list
    """
    # to_build = set()
    # found = False
    # for op in ops:
    #    for osc in opers:
    #        if op.upper() == osc.upper():
    #            to_build.union(set([f"{osc}-{y}" for y in docker_images[osc]["versions"]]))
    #            found = True
    #            continue
    #    if found:
    #        continue
    #    for osc in choices:
    #        if op.upper() == osc.upper():
    #            to_build.add(osc)
    #            found = True
    #            continue
    #    if not found:
    #        raise RuntimeError(f"Unknown operating system: {op}")
    # return list(to_build)
    return ["Ubuntu-22"]
