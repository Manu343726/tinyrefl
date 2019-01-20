#/bin/bash
set -e

# from https://stackoverflow.com/a/3879077/1609356
require_clean_work_tree () {
    # Update the index
    git update-index -q --ignore-submodules --refresh
    err=0

    # Disallow unstaged changes in the working tree
    if ! git diff-files --quiet --ignore-submodules --
    then
        echo >&2 "cannot $1: you have unstaged changes."
        git diff-files --name-status -r --ignore-submodules -- >&2
        err=1
    fi

    # Disallow uncommitted changes in the index
    if ! git diff-index --cached --quiet HEAD --ignore-submodules --
    then
        echo >&2 "cannot $1: your index contains uncommitted changes."
        git diff-index --cached --name-status -r --ignore-submodules HEAD -- >&2
        err=1
    fi

    if [ $err = 1 ]
    then
        echo >&2 "Please commit or stash them."
        err=1
    fi

    return $err
}

CLANG_FORMAT_SCRIPT=$(pwd)/run-clang-format.sh

if [ ! -e $CLANG_FORMAT_SCRIPT ]; then
    echo clang-format script not found
    exit 3
else
    echo using clang-format script $CLANG_FORMAT_SCRIPT
fi

if bash $CLANG_FORMAT_SCRIPT; then
    echo clang-format run ok

    if require_clean_work_tree; then
        echo no changes after clang-format, ok
    else
        echo changes found after running clang-format, please run clang-format before submitting your changes
        exit 1
    fi

else
    echo an error ocurred while running clang-format
    exit 2
fi
