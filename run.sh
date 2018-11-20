#!/bin/bash

# set OSM_MAP variable to point path to used osm map
set -e

valhalla_build_config --mjolnir-tile-dir ${PWD}/tiles --mjolnir-tile-extract ${PWD}/tiles.tar --mjolnir-timezone ${PWD}/tiles/timezones.sqlite --mjolnir-admin ${PWD}/tiles/admins.sqlite > valhalla.json
valhalla_build_tiles -c valhalla.json ${OSM_MAP}
find tiles | sort -n | tar cf tiles.tar --no-recursion -T -
valhalla_service valhalla.json 4
