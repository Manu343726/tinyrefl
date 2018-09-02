#!/bin/bash

# Full path to the ci/ directory, where this script is located
CI_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
SRC_DIR=$CI_DIR/..
OUTPUT_FILE=$SRC_DIR/.gitlab-ci.yml

echo "variables:"             > $OUTPUT_FILE
echo "  GIT_STRATEGY: clone" >> $OUTPUT_FILE


source $CI_DIR/yaml.sh
parse_yaml $CI_DIR/matrix.yml
create_variables $CI_DIR/matrix.yml

for stage in "${stages[@]}"; do
    echo generating \"$stage\" stage...

    tags_var=${stage}_tags[@]
    tags=${!tags_var}
    toolchains_var=${stage}_toolchains[@]
    toolchains=${!toolchains_var}
    llvm_versions_var=${stage}_llvm_versions[@]
    llvm_versions=${!llvm_versions_var}
    script_var=${stage}_script[@]
    script=${!script_var}

    echo $tags

    for toolchain in ${toolchains}; do
        for llvm_version in ${llvm_versions}; do
            job_name=$stage-$toolchain-llvm$llvm_version
            docker_image=$docker_repo/$docker_name:$toolchain

            echo $toolchain $llvm_version

            echo "$job_name:"                      >> $OUTPUT_FILE
            echo "  stage: $stage"                 >> $OUTPUT_FILE
            echo "  tags:"                         >> $OUTPUT_FILE

            for tag in ${tags}; do
                echo "    - $tag"                  >> $OUTPUT_FILE
            done

            echo "  image: $docker_image"          >> $OUTPUT_FILE
            echo "  variables:"                    >> $OUTPUT_FILE
            echo "    LLVM_VERSION: $llvm_version" >> $OUTPUT_FILE
            echo "  script:"                       >> $OUTPUT_FILE

            for script_line in ${script}; do
                echo "    - $script_line"          >> $OUTPUT_FILE
            done
        done
    done
done
