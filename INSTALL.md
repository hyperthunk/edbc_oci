# Installation

This document provides instructions on installing *edbc_oci* either from a
release tarball or from sources. It is highly recommended that you install
a binary release rather than building from sources, as the process is far
less complicated.

## Installing from release tarball

TBC

## Installing from sources

###  Prerequisites

You will need the Oracle Call Interface (OCI) libraries on your system in order
to use this driver. These can be obtained from Oracle as either a *full* client
install or using the instant client.

### Oracle Feature Compatibility

Certain driver features will only work with a specific version of the oracle
client libraries. For example, session pool support (i.e., Database Resident
Connection Pooling) is only available when you install OCI 11g (or higher).

### Configure

The sources come with an autotools style *configure* script, which should be
familiar to most unix users, and a `configure.bat` batch script for windows
users. Running this script will generate a make script (amongst other things)
using sensible defaults, trying to detect things in your environment where
doing so is feasible.

You can override the defaults (and detection) in the script using various
flags. For details, run the script with the `--help` to list the options
that are available.

Example:

    $ ./configure --prefix=$ERL_LIBS --disable-64bit

And for Windows users:

    C:\Users\xn\w\edbc_oci> configure.bat --prefix=%ERL_LIBS% --disable-64bit

### Erlang Dependencies

You will need to fetch any missing dependencies in order to build from sources.
You can do this by running the `bootstrap` script in the top directory.
Running this script will install missing dependencies locally (into the `lib`
directory), without affecting your `code:lib_dir` and/or `ERL_LIBS` directories.

### Compiling and Installing

Once you've configured the build, you may install the project's sources with
the shell commands:

    $ make
    $ make install

If you do not have the *Make* program installed on your system, do not panic.
The *configure and make* stages are provided to ease compatibility with the
various package management systems in the unix world. The generated make
file simply delegates to some underlying scripts anyway, so you may simply
run the scripts directly if you prefer. On unix:

    $ ./build.sh --skip-tests
    $ ./install.sh

The equivalent windows commands:

    C:\Users\xn\w\edbc_oci> build.bat --skip-tests
    C:\Users\xn\w\edbc_oci> install.bat
