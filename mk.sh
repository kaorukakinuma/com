#!/bin/bash -e

# argument for option
OPTIONS="${@:1}"

# path to this file
MY_FILE=`readlink -f "${0}"`
MY_DIR=`dirname ${MY_FILE}`

# path to top directory
TOP_DIR=`readlink -f "${MY_DIR}"`
TOP_DIRNAME=`basename ${TOP_DIR}`

# working directory
WORK_ROOT_DIR=`readlink -f "${TOP_DIR}/../_build"`
WORK_DIR="${WORK_ROOT_DIR}/${TOP_DIRNAME}"

function build()
{
    pushd "${WORK_DIR}" > /dev/null
        if [ ! -e ./Makefile ]; then
            autoreconf --install --force
            ./configure
        fi
        make uninstall || true
        make
        make install
    popd > /dev/null
}

function main()
{
    mkdir -p "${WORK_ROOT_DIR}"
    cp -a "${TOP_DIR}" "${WORK_ROOT_DIR}"

    if [ "${OPTIONS}" = "clean" ]; then
        rm -rf "${WORK_DIR}"
        echo " >>> Cleaned."
        return 0
    fi

    echo " >>> Now building..."

    build
    rm -rf "${WORK_ROOT_DIR}"

    echo " >>> Build completed."
}

main
