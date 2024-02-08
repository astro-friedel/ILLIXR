#!/bin/bash
# this script builds a full version of illixr

help() {
  echo "Usage: $0 -p <profile> -b <branch>"
  echo "  -p <profile>  The profile to use"
  echo "  -b <branch>   The branch to build, default is master"
  echo "  -r            Build in Release mode, default is Debug"
  echo "  -j <jobs>     The number of jobs to use, default is 4"
  echo "  -h            Display this help message"
  exit 1
}

branch=master
jobs=4
buildtype=Debug

while getopts hp:b:j:r flag

do
  case "${flag}" in
    p) profile=${OPTARG};;
    b) branch=${OPTARG};;
    r) buildtype=Release;;
    j) jobs=${OPTARG};;
    h) help
      exit 0;;
    *) help
      exit 1;;
  esac
done

cd ILLIXR
rm -rf build
git switch $branch
git pull

mkdir build
cd build

cmake .. -DYAML_FILE=$profile -DCMAKE_BUILD_TYPE=$buildtype -DBUILD_PARALLEL_LEVEL=$jobs -DCMAKE_INSTALL_PREFIX=/home/illixr/ilxr

make -j$jobs
make install
