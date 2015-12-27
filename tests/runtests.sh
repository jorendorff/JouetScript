#!/usr/bin/env bash

if [ $# -lt 1 ]; then
    echo "usage: $0 <js-shell> [test-files ... ]";
fi

if [ $# = 1 ]; then
    tests=$(find $(dirname $0) -name *.js);
    else
    tests=${@:2};
fi

failure=false;
for t in $tests; do
    $1 $t > /dev/null;
    if [ $? = 0 ]; then
        echo "$t : passed";
    else
        echo "$t : failed";
        failure=true;
    fi
done

if [ $failure = true ]; then
    exit 1;
else
    exit 0;
fi
