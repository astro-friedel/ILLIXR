# Unit Tests

Each [plugin][1] should have a set of unit tests which should exercise as much code as possible.
Plugins which are part of the main [ILLIXR][2] repository use [GTest][3] as their testing framework.
For plugins which are external to the main repository (those which are not part of the main [ILLIXR][2] repo) can use any test framework as long as it is easily installable (preferably via `apt-get` or `yum`).
The following sections go through how to set up and include unit tests for a plugin.

## Requirements

The Google Test framework is needed to compile and run the tests. Use one of the following to install it on your system:

- Ubuntu
```bash
  sudo apt-get install libgtest-dev libgmock-dev
```
- Fedora
```bash
  sudo dnf install gtest-devel gmock-devel
```
- CentOS
```bash
  sudo yum install gtest-devel gmock-devel
```
- Source
```bash
  git clone https://github.com/google/googletest.git
  cd googletest
  cmake -S . -B build <YOUR CMAKE OPTIONS>
  cmake --build build
  cmake --install <BINARY INSTALL DIR> <OTHER OPTIONS>
```

## CMake

In the CMakeLists.txt file for the plugin add the following to the bottom

```cmake
if(BUILD_TESTS)
    add_test(${PLUGIN_NAME})
endif()
```

## Writing the test

The name of the test code file needs to be named `test_plugin.cpp`, as this name will work automatically with the ILIXR test framework. For specifics on how to use Google Test see their [documentation][3].

### Mocks

We provide some mocking infrastructure for the [switchboard][4] and [phonebook][5] classes. Below is an example of how to use them.

[1]:   glossary.md#plugin
[2]:   https://github.com/ILLIXR/ILLIXR
[3]:   https://google.github.io/googletest/
[4]:   glossary.md#switchboard
[5]:   glossary.md#phonebook
