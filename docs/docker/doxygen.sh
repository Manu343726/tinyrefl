#!/bin/sh

docker run -ti -u 1000:1000 --workdir=$(pwd) -v$(pwd):$(pwd) -v$HOME/Documents/m.css/:/m.css/ manu343726/mcss-doxygen-generator $@
