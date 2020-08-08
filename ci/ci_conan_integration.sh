#/bin/sh
set -e

echo LLVM version: $LLVM_VERSION

conan create tool --build=missing --build=tinyrefl-tool
conan create .    --build=missing --build=tinyrefl

if [[ -z "$CI_COMMIT_TAG" ]]; then
    conan upload tinyrefl-tool --confirm --all -r manu343726
    conan upload tinyrefl      --confirm --all -r manu343726
fi
