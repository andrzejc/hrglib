#!/usr/bin/env bash
set -eux
set -o pipefail

function choco {
    local cmd="$1"
    shift
    "$(command -v choco.exe)" "$cmd" --cache "C:\\Users\\$USER\\.choco_cache" "$@"
}

mkdir -p "$HOME/.choco_cache"
choco install \
    python3 --params "/InstallDir:C:\\Python3"

    # doxygen.install

export PATH="/c/Python3:/c/Python3/Scripts:$PATH"

# emulate sudo for script portability
function sudo {
    [[ "$1" != -H ]] || shift
    "$@"
}

# alias for python3
function python3 {
    python "$@"
}

curl https://bootstrap.pypa.io/get-pip.py | sudo -H python3

# XXX msvc boost is enough for header-only libraries
choco install \
    boost-msvc-14.1 \
    ninja

export BOOST_ROOT=c:\\local\\boost_1_67_0
export BOOST_INCLUDEDIR="${BOOST_ROOT}\\boost"
export BOOST_LIBRARYDIR="${BOOST_ROOT}\\lib64-msvc-14.1"

if [[ -z "${MSVC:-}" ]]
then
    # TODO build boost from sources, cache the build dir on travis?
    true
else
    choco install \
        windows-sdk-10.1 --version 10.1.18362.1 || cat $HOME/.choco_cache/windows-sdk-10.1.log

    kits="/c/Program Files (x86)/Windows Kits"
    # ls -l "$kits"
    # ls -l "$kits/10"
    UniversalCRTSdkDir=C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\10\\
    UCRTVersion=10.0.18362.0
    # ls -l "$kits/NETFXSDK"
    NETFXSDKDir=C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\NETFXSDK\\4.8\\
    # ls -l "$kits/NETFXSDK/4.8"
    # ls -l "$kits/10/include"
    # ls -l "$kits/10/lib"
    UCRTInclude="${UniversalCRTSdkDir}Include\\${UCRTVersion}\\"
    UCRTLib="${UniversalCRTSdkDir}lib\\${UCRTVersion}\\"
    # ls -l "$kits/10/include/$UCRTVersion"
    # ls -l "$kits/10/lib/$UCRTVersion"
    VSEdition=BuildTools
    VCVersion=14.16.27023
    VCToolsInstallDir=c:\\Program\ Files\ \(x86\)\\Microsoft\ Visual\ Studio\\2017\\${VSEdition}\\VC\\Tools\\MSVC\\${VCVersion}\\
    INCLUDE="${VCToolsInstallDir}include"
    INCLUDE+=";${UCRTInclude}ucrt;${NETFXSDKDir}include\\um;${UCRTInclude}shared;${UCRTInclude}um;${UCRTInclude}winrt"
    LIB="${VCToolsInstallDir}lib\\x64"
    LIB+=";${UCRTLib}ucrt\\x64;${NETFXSDKDir}lib\\um\\x64;${UCRTLib}um\\x64"
    export INCLUDE LIB

    export CC="${MSVC}"
    export CXX="${MSVC}"

    VSINSTALLDIR="/c/Program Files (x86)/Microsoft Visual Studio/2017/${VSEdition}"
    VCINSTALLDIR="$VSINSTALLDIR/VC"
    VCToolsInstallDir="$VCINSTALLDIR/Tools/MSVC/$VCVersion"
    VS140COMNTOOLS="$VSINSTALLDIR/Common7/Tools"
    VCVARS_PATH="$VCToolsInstallDir/bin/Hostx64/x64:$VS140COMNTOOLS:/c/Program Files (x86)/Windows Kits/10/bin/x64:/c/Program Files (x86)/Windows Kits/10/bin/x86"
    export PATH="$VCVARS_PATH:$PATH"
fi
