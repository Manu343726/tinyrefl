#!bin/sh

cat build/compile_commands.json | grep -oE "(\-isystem \S*)|( \-[^i]\S*)" | grep -Ev "\-c|\-o" | sort | uniq | sed -e 's/\\\\\\"/"/g' | xargs echo -x c++ | xargs -n1 | tee .color_coded
