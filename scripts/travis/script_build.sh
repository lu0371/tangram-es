#!/usr/bin/env bash

set -e
set -o pipefail

if [[ ${PLATFORM} == "osx" ]]; then
    # Build osx project
    echo "Building osx project"
    CMAKE_OPTIONS="-DUNIT_TESTS=1 -DBENCHMARK=1" make -j osx
fi

if [[ ${PLATFORM} == "linux" ]]; then
    # Build linux project
    echo "Building linux project"
    CMAKE_OPTIONS="-DUNIT_TESTS=1 -DBENCHMARK=1" make -j 4 linux
fi

if [[ ${PLATFORM} == "ios" ]]; then
    # Build ios project
    echo "Building ios demo app and framework"
    make ios

    #if [ "${TRAVIS_PULL_REQUEST}" = "false" ] && [ "${TRAVIS_BRANCH}" = "master" ]; then
    #    # Build demo app
    #    make ios

    #    # TODO: upload binary framework
    #else
    #    make ios-framework-sim
    #fi
fi

if [[ ${PLATFORM} == "android" ]]; then
    # Build android project
    echo "Building android project"
    export TERM=dumb
    make android
fi

