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
    python3 \

    # doxygen.install

export PATH="/c/Python37:/c/Python37/Scripts:$PATH"

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

# XXX msvc boost is enaough for header-only libraries
choco install \
    boost-msvc-14.1 \
    ninja

export BOOST_ROOT=c:\\local\\boost_1_67_0
export BOOST_INCLUDEDIR="${BOOST_ROOT}\\boost"
export BOOST_LIBRARYDIR="${BOOST_ROOT}\\lib64-msvc-14.1"

function winls {
    cmd.exe /C "dir \"$1\""
}

if [[ -z "${MSVC:-}" ]]
then
    # TODO build boost from sources, cache the build dir on travis?
    true
else
    choco install \
        windows-sdk-10.0

    # winls C:\\Program\ Files\ \(x86\)\\Windows\ Kits
    UniversalCRTSdkDir=C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\10\\
    # winls "${UniversalCRTSdkDir}"
    UCRTVersion=10.0.17134.0
    # winls C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\NETFXSDK
    NETFXSDKDir=C:\\Program\ Files\ \(x86\)\\Windows\ Kits\\NETFXSDK\\4.6.2\\
    # winls "${NETFXSDKDir}"
    # winls "${UniversalCRTSdkDir}Include\\"
    UCRTInclude="${UniversalCRTSdkDir}Include\\${UCRTVersion}\\"
    # winls "${UCRTInclude}"
    UCRTLib="${UniversalCRTSdkDir}lib\\${UCRTVersion}\\"
    # winls "${UCRTLib}"
    VSEdition=BuildTools
    VCVersion=14.16.27023
    # bin\\Hostx64\\x64\\
    VCToolsInstallDir=c:\\Program\ Files\ \(x86\)\\Microsoft\ Visual\ Studio\\2017\\${VSEdition}\\VC\\Tools\\MSVC\\${VCVersion}\\
    # winls "${VCToolsInstallDir}"
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
