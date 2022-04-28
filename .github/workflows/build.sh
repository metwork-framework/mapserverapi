#!/bin/bash

set -x

yum -y install metwork-mfext-layer-mapserver
cd /src
/opt/metwork-mfext/bin/mfext_wrapper layer_wrapper --layers=mapserver@mfext -- make MAPSERVER_LIB_DIR=/opt/metwork-mfext/opt/mapserver/lib PREFIX=/opt/metwork-mfext/opt/mapserver clean all test install
