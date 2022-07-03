function setup-pandora() {
export PANDORA_PFA_VERSION=v03-13-03
export PANDORA_SDK_VERSION=v03-03-03
export PANDORA_LC_CONTENT_VERSION=v03-01-04
export PANDORA_MONITORING_VERSION=v03-04-03

export PANDORA_AREA=$CEPCSWEXTERNAL/Pandora
for pandorapkg in PandoraPFA PandoraSDK LCContent PandoraMonitoring; do
    export ${pandorapkg}=$PANDORA_AREA/${pandorapkg}
    export CMAKE_PREFIX_PATH=$PANDORA_AREA/${pandorapkg}:$CMAKE_PREFIX_PATH
    if [ -d "$PANDORA_AREA/${pandorapkg}/lib" ]; then
        export LD_LIBRARY_PATH=$PANDORA_AREA/${pandorapkg}/lib:$LD_LIBRARY_PATH
    fi
    if [ -d "$PANDORA_AREA/${pandorapkg}/include" ]; then
        export CPATH=$PANDORA_AREA/${pandorapkg}/include:$CPATH
        export ROOT_INCLUDE_PATH=$PANDORA_AREA/${pandorapkg}/include:$ROOT_INCLUDE_PATH
    fi
done
}

