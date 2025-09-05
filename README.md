# Advanced system programming skeleton

# Environment setup
We will provide two options for setting up your programming environment:
 * QEMU
 * docker
 * DIY (we may not be able to help)

## QEMU setup
To run the project with QEMU, place the received `image.qcow2` in the directory with the 
`./qemu.sh` or `./qemu.ps1` script. The VM can be run using the script.

## Docker setup
For a docker setup, m1-m4 is able to run on win/mac/linux. However, for m5
you may need access to the perf and bpf kernel subsystems, which can get complicated
on docker desktop as kernel headers and perf may not be
available. We reccomend to use Docker only on linux for m5.

to bring up the environment, run:
```bash
docker compose run dev_cont
```

To force a build instead of a pull use `docker compose build` (**WARNING: building takes ~1h**).
To extend the image (as a student), you can avoid build times by creating a new 
dockerfile with something like:

```Dockerfile
FROM joachimb65/aspv8:latest
RUN apt-get update
RUN apt-get install -y valgrind ...
```

## DIY setup
The only things you will need are:
* libv8_monolith.a (and includes)
* bpftrace & perf for m5 (or dtrace but we cannot help)
* USDT probe support for m5

To build v8, check out (the v8 docs on building)[https://v8.dev/docs/build-gn#:~:text=Building%20V8%3A%20the%20raw%2C%20manual%20workflow%20%23]. 
The build process takes ~1h to complete.

USDT probes need the `<sys/sdt.h>` header which is usually available under a package
named `systemtap-sdt-dev` or `stap-dev` or similar. 

The project expects you to place v8 in /home/asp/v8 using v8 file structure.

# Compilation
This is a cmake project and can be built using:
Make sure to place v8 in /home/asp/v8
```bash
cmake .
make
```

# Codegrade: setup & submission
Codegrade should be supplied with the tests and the test running script. This
can easily be zipped with `make codegrade_tests`. Similarly, students can make the
submission zip using `make submission`.

Codegrade should be configured to repair the project file structure from the submission
and the test. Then, runtests.py, generates json files grades, which can be used
in the codegrade "custom test" blocks.