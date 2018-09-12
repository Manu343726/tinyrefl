#!/bin/bash

# Full path to the ci/ directory, where this script is located
CI_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null && pwd )"
SRC_DIR=$CI_DIR/..
OUTPUT_FILE=$SRC_DIR/.gitlab-ci.yml

source $CI_DIR/yaml.sh
parse_yaml $CI_DIR/matrix.yml
create_variables $CI_DIR/matrix.yml

echo "variables:"             > $OUTPUT_FILE
echo "  GIT_STRATEGY: clone" >> $OUTPUT_FILE
echo "stages:"               >> $OUTPUT_FILE

for stage in "${stages[@]}"; do
    echo "  - $stage"        >> $OUTPUT_FILE
done

for stage in "${stages[@]}"; do
    echo generating \"$stage\" stage...
    stage_sanitized=${stage//-/_}
    stage_sanitized=${stage_sanitized//./_}

    tags_var=${stage_sanitized}_tags[@]
    tags=${!tags_var}
    toolchains_var=${stage_sanitized}_toolchains[@]
    toolchains=${!toolchains_var}
    llvm_versions_var=${stage_sanitized}_llvm_versions[@]
    llvm_versions=${!llvm_versions_var}
    declare -n script=${stage_sanitized}_script

    if [ -z "${toolchains}" ]; then
        job_name=$stage
        docker_image_var=${stage_sanitized}_image
        docker_image=${!docker_image_var}

        echo  - $stage \(single job stage\)

        echo "$job_name:"                      >> $OUTPUT_FILE
        echo "  stage: $stage"                 >> $OUTPUT_FILE
        echo "  tags:"                         >> $OUTPUT_FILE

        for tag in ${tags}; do
            echo "    - $tag"                  >> $OUTPUT_FILE
        done

        echo "  image: $docker_image"          >> $OUTPUT_FILE
        echo "  script:"                       >> $OUTPUT_FILE

        for ((i = 0; i < ${#script[@]}; i++)); do
            script_line=${script[$i]}
            echo "    - $script_line"          >> $OUTPUT_FILE
        done
    else
        for toolchain in ${toolchains}; do
            for llvm_version in ${llvm_versions}; do
                job_name=$stage-$toolchain-llvm$llvm_version
                docker_image=$docker_repo/$docker_name:$toolchain

                echo  - $stage $toolchain $llvm_version

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

                for ((i = 0; i < ${#script[@]}; i++)); do
                    script_line=${script[$i]}
                    echo "    - $script_line"          >> $OUTPUT_FILE
                done
            done
        done
    fi

    echo End of $stage stage
done
