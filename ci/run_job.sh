#!/bin/bash

set -e

# Full path to the ci/ directory, where this script is located
CI_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
SRC_DIR=$CI_DIR/..
cd $CI_DIR

JOB_NAME=$1
JOB_NAME_SANITIZED=${JOB_NAME//-/_}
JOB_NAME_SANITIZED=${JOB_NAME_SANITIZED//./_}
OPTION=$2
IMAGE_VARIABLE=${JOB_NAME_SANITIZED}_image
LLVM_VERSION_VARIABLE=${JOB_NAME_SANITIZED}_variables_LLVM_VERSION
CROSS_BUILDING_VARIABLE=${JOB_NAME_SANITIZED}_variables_CROSS_BUILDING
SCRIPT_VARIABLE=${JOB_NAME_SANITIZED}_script


# Parse Giltab-ci config file
source yaml.sh
create_variables $SRC_DIR/.gitlab-ci.yml

IMAGE=${!IMAGE_VARIABLE}
LLVM_VERSION=${!LLVM_VERSION_VARIABLE}
LLVM_VERSION=${LLVM_VERSION//\"/}
CROSS_BUILDING=${!CROSS_BUILDING_VARIABLE}
CROSS_BUILDING=${CROSS_BUILDING//\"/}
declare -n SCRIPT=${SCRIPT_VARIABLE}

if [[ -z "$IMAGE" ]]; then
    >&2 echo No valid ci job named \"$JOB_NAME\" found
    exit 1
fi

if [[ ! -z "$CROSS_BUILDING" ]]; then
    CROSS_BUILDING_FLAG="-e CROSS_BUILDING=$CROSS_BUILDING"
fi

if [[ "$OPTION" == "--clean-build" ]]; then
    CLEAN_BUILD_FLAG="-e CLEAN_BUILD=YES"
elif [[ "$OPTION" == "--help" ]]; then
    echo run_job <job-name> [option]
    echo " "
    echo   option:
    echo     --clean-build : Clean buildtree before build
    echo     --help : Show this help
    exit
fi

if [[ ! -z "$LLVM_VERSION" ]]; then
    LLVM_VERSION_FLAG="-e LLVM_VERSION=$LLVM_VERSION"
fi

echo Running job $JOB_NAME
echo Using docker image $IMAGE
echo CROSS_BUILDING=$CROSS_BUILDING
echo LLVM_VERSION=$LLVM_VERSION
echo GENERATOR=$GENERATOR

script_file=/tmp/run_job.sh

echo "#!bin/sh" > $script_file
echo "set -e"  >> $script_file

for ((i = 0; i < ${#SCRIPT[@]}; i++)); do
    script_line=${SCRIPT[$i]}
    echo "$script_line" >> $script_file
done

chmod +x $script_file

docker run -ti --rm \
    --hostname $JOB_NAME \
    -v $script_file:$script_file \
    -v $SRC_DIR:/repo \
    -w /repo \
    -e CI_JOB_NAME=$JOB_NAME \
    $LLVM_VERSION_FLAG \
    $CROSS_BUILDING_FLAG \
    $CLEAN_BUILD_FLAG \
    -e GENERATOR="$GENERATOR" \
    $IMAGE bash $script_file
