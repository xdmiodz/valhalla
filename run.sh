#!/bin/bash

set -e

mkdir -p ${PWD}/tiles
wget https://download.geofabrik.de/asia/malaysia-singapore-brunei-latest.osm.pbf
valhalla_build_config --mjolnir-tile-dir ${PWD}/tiles --mjolnir-tile-extract ${PWD}/tiles.tar --mjolnir-timezone ${PWD}/tiles/timezones.sqlite --mjolnir-admin ${PWD}/tiles/admins.sqlite > valhalla.json
valhalla_build_tiles -c valhalla.json malaysia-singapore-brunei-latest.osm.pbf
find tiles | sort -n | tar cf tiles.tar --no-recursion -T -
rm -rf /valhalla/tiles /valhalla/malaysia-singapore-brunei-latest.osm.pbf
valhalla_service valhalla.json 4
