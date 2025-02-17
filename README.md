# Network Programming

Program files and notes written to explore network programming
in Windows, MacOS and Linux with C++.


# Targets

After running the CMake with `run.ps1` on Windows or `run.sh` on Linux,
the targets will be generated in `targets` folder on the projects root directory.
By default, all targets will be built.

To build a specific target, for instance: a clean target, specify as follow: \
`cmake --build build --target clean`


# Execution

## Windows

- Launch developer powershell
- Link libraries: iphlpapi, ws2_32
