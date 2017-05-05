#!/usr/bin/env bash

abort()
{
    echo "An error occurred. Exiting..." >&2
    exit 1
}

trap 'abort' 0

set -e

OMPI_VER=openmpi-2.1.0
OMPI_TAR=${OMPI_VER}.tar.bz2
OMPI_ULR=https://www.open-mpi.org/software/ompi/v2.1/downloads/$OMPI_TAR
OMPI_SHA=3e1e02d4c4b3e22a9cc33deddab546cb990d0ee3

cd /tmp
wget $OMPI_URL
echo "$OMPI_SHA $OMPI_TAR" | sha1sum -c
echo "Extracting tarball...this will take a few minutes"
tar -xf $OMPI_TAR
cd $OMPI_VER
mkdir -p build
cd build

echo "Build directory is $PWD"

trap : 0
set +e

OMPI_CONFIG="CFLAGS=-march=armv7-a CCASFLAGS=-march=armv7-a ../configure"

echo -n "Select prefix under which to install (/usr/local/): "
read OMPI_PREFIX
if [[ -z $OMPI_PREFIX ]] ; then
    OMPI_CONFIG=$OMPI_CONFIG' --prefix=/usr/local'
else
    OMPI_CONFIG=$OMPI_CONFIG" --prefix=$OMPI_PREFIX"
fi

echo -n "Enable OpenMPI for FORTRAN (n): "
read E_FORTRAN
case $E_FORTRAN in
    'y')
        sudo apt-get install gfortran &&
        OMPI_CONFIG=$OMPI_CONFIG" --enable-mpi-fortran"
        ;;
    *)
        echo "FORTRAN support for Open MPI will not be built"
        OMPI_CONFIG=$OMPI_CONFIG" --enable-mpi-fortran=no"
        ;;
esac

echo -n "Enable OpenMPI for C++ (n): "
read E_CXX
case $E_CXX in
    'y')
        OMPI_CONFIG=$OMPI_CONFIG" --enable-mpi-cxx --enable-cxx-exceptions"
        ;;
    *)
        OMPI_CONFIG=$OMPI_CONFIG" --disable-mpi-cxx --disable-mpi-cxx-seek"
        echo "C++ support for Open MPI will not be built"
        ;;
esac

echo -n "Enable OpenMPI for Java (n): "
read E_JAVA
case $E_JAVA in
    'y')
        sudo apt-get install openjdk-7-jdk &&
        OMPI_CONFIG=$OMPI_CONFIG" --enable-mpi-java"
        ;;
    *)
        OMPI_CONFIG=$OMPI_CONFIG" --disable-java"
        echo "Java support for Open MPI will not be built"
        ;;
esac

echo -n "Enable IPv6 (y): "
read E_IPV6
if [[ -z $E_IPV6 ]] ; then E_IPV6='y' ; fi
case $E_IPV6 in
    'y')
        OMPI_CONFIG=$OMPI_CONFIG' --enable-ipv6'
        ;;
    *)
        OMPI_CONFIG=$OMPI_CONFIG' --disable-ipv6'
        echo "IPv6 support will not be built"
        ;;
esac

echo -n "Enable valgrind (n): "
read E_VALGRIND
case $E_VALGRIND in
    'y')
        sudo apt-get install valgrind
        OMPI_CONFIG=$OMPI_CONFIG" --enable-memchecker"
        ;;
    *)
        echo "Valgrind support not built-in but valgrind can still be used"
        ;;
esac

echo "OpenMPI will be built with these options:"
echo $OMPI_CONFIG

echo -n "Confirm (y/n): "
read CONFIRM
if [[ $CONFIRM != 'y' ]] ; then echo "Aborting" ; exit ; fi

echo $OMPI_CONFIG > $HOME/openmpi-configuration.txt &&
echo "Configuration saved as $HOME/openmpi-configuration.txt"
echo ""
sleep 1

$OMPI_CONFIG  &&
make -j 3     &&
sudo make install
