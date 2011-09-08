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

### Erlang Dependencies

You will need to fetch any missing dependencies in order to build from sources.
You can do this by running the `bootstrap` script in the top directory.
Running this script will install missing dependencies locally (into the `lib`
directory), without affecting your `code:lib_dir` and/or `ERL_LIBS` directories.

### Compiling

You may install the project's sources with the shell command:

    make && make install
