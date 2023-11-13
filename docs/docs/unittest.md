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
if (BUILD_TESTS)
    add_illixr_test(${PLUGIN_NAME})
endif ()
```

## Writing the test

The name of the test code file needs to be named `plugins/<PLUGIN>/test/test_plugin.cpp`, as this name will work automatically with the ILIXR test framework. 
Setting up a test is straight forward as the bulk of the work is done with macros. The basic structure is:

```C++
1  #include <gtest/gtest.h>
2  #include "../plugin.hpp"
3  #include "test_macros.hpp"
4
5  START_SETUP
6
7  END_SETUP
8
9  TEST_F(ILLIXRTest, TestPlugin) {
10     auto plg = plugin("plugin", pb);
11 }
```

| Line #   | Notes                                                                                                                                      |
|----------|--------------------------------------------------------------------------------------------------------------------------------------------|
| 1        | Brings in the GTest framework.                                                                                                             |
| 2        | Include the header for your plugin.                                                                                                        |
| 3        | This header contains the macros for setting up your tests.                                                                                 |
| 5        | Macro which sets up the initial test class.                                                                                                |
| 7        | Macro which finishes setting up the test class.                                                                                            |
| 9        | Example of test instance, The `ILLIXRTest` is required, but the name of the test (`TestPlugin`) should be changed to something meaningful. |
| 10       | Crete an instance of the plugin being tested. `plugin` needs to be changed to the name of your plugin,in both instances.                   |
| 10 - 11  | The actual testing code should be put between these lines.                                                                                 | 

### Tests with plugin dependencies

In instances where the plugin being tested depends on another plugin being loaded (timewarp_gl needs a pose plugin) there are a few adjustments to the above procedures. There are mocked versions of several plugins which can be used instead of the actual plugins. THe following have mocked versions.

- app
- display
- pose
- timewarp_vk

In the CMake call to add the test, the dependent plugins should be given as follows:

```cmake
    add_illixr_test(${PLUGIN_NAME} mock_pose;mock_display)
```
Note that the dependent plugins are separated by a `;`.

In your `test_plugin.cpp` file these dependent plugins can be loaded with a macro call to

```C++
ADD_DEPENDENCY(mock_pose)
ADD_DEPENDENCY(mock_display)
```
before the call to `END_SETUP`.

This will automatically load the mocked version of these plugins, so they are available to the plugin being tested.

For plugins which require the `extended_window` this can be loaded with the macro call

```C++
WITH_GL
```
before the call to `END_SETUP`.

For specifics on how to use Google Test/Mock see their [documentation][3].

### Mocks

We provide some mocking infrastructure for the [switchboard][4] and [phonebook][5] classes. Below is an example of how to use them.

[1]:   glossary.md#plugin
[2]:   https://github.com/ILLIXR/ILLIXR
[3]:   https://google.github.io/googletest/
[4]:   glossary.md#switchboard
[5]:   glossary.md#phonebook
