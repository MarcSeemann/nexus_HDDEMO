#!/bin/bash

# ============================================================
# Geant4
# ============================================================

export G4INSTALL=/home/investigator/Documents/simulation_software/geant4-v11.0.2/install

export PATH=$G4INSTALL/bin:$PATH
export LD_LIBRARY_PATH=$G4INSTALL/lib:$LD_LIBRARY_PATH

source $G4INSTALL/bin/geant4.sh


# ============================================================
# Qt 5.15.2
# ============================================================

export QT_PATH=/home/investigator/Qt/5.15.2/gcc_64

export PATH=$QT_PATH/bin:$PATH
export LD_LIBRARY_PATH=$QT_PATH/lib:$LD_LIBRARY_PATH


# ============================================================
# GSL
# ============================================================

export GSL_PATH=/usr/local

export PATH=$GSL_PATH/bin:$PATH
export LD_LIBRARY_PATH=$GSL_PATH/lib:$LD_LIBRARY_PATH


# ============================================================
# HDF5
# ============================================================

# Your current HDF5 installation is the Debian /usr installation
export HDF5_PATH=/usr
export HDF5_LIB=/usr/lib/x86_64-linux-gnu/hdf5/serial
export HDF5_INC=/usr/include/hdf5/serial

export LD_LIBRARY_PATH=$HDF5_LIB:$LD_LIBRARY_PATH


# ============================================================
# This NEXUS installation
# ============================================================

export NEXUSDIR=$HOME/Documents/HD_Demo/simulation/nexus_HDDEMO

export PATH=$NEXUSDIR/bin:$PATH
export LD_LIBRARY_PATH=$NEXUSDIR/lib:$LD_LIBRARY_PATH


echo "=========================================="
echo "NEXUS HD-DEMO environment loaded"
echo "=========================================="
echo "Geant4: $G4INSTALL"
echo "Qt:     $QT_PATH"
echo "GSL:    $GSL_PATH"
echo "HDF5:   $HDF5_PATH"
echo "NEXUS:  $NEXUSDIR"
echo "=========================================="


compile_function() {
    echo "=========================================="
    echo "Building NEXUS HD-DEMO..."
    echo "=========================================="

    cd "$NEXUSDIR/build" || {
        echo "✗ Could not find build directory!"
        return 1
    }

    cmake --build . --target install -j$(nproc)
    local build_status=$?

    if [ $build_status -eq 0 ]; then
        echo ""
        echo "=========================================="
        echo "✓ NEXUS HD-DEMO build successful!"
        echo "=========================================="
        echo ""
        echo "Executable:"
        echo "  $NEXUSDIR/bin/nexus"
        echo ""
        echo "Library:"
        echo "  $NEXUSDIR/lib/libnexus.so"
        echo ""
    else
        echo ""
        echo "=========================================="
        echo "✗ NEXUS HD-DEMO build failed!"
        echo "=========================================="
        return 1
    fi

    cd "$NEXUSDIR" || return 1
}
