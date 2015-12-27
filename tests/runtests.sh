#!/usr/bin/env bash

if [ $# -lt 1 ]; then
    echo "usage: $0 <js-shell> [test-files ... ]";
fi

if [ $# = 1 ]; then
    tests=$(find $(dirname $0) -name *.js);
    else
    tests=${@:2};
fi

for t in $tests; do
    $1 $t > /dev/null;
    if [ $? = 0 ]; then
        echo "$t : passed";
    else
        echo "$t : failed";
    fi
done
