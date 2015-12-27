#!/usr/bin/env bash

set -e

if [ $# -lt 1 ]; then
    echo "usage: $0 <js-shell> [test-files ... ]";
fi

if [ $# = 1 ]; then
    tests=$(find $(dirname $0) -name *.js);
    else
    tests=${@:2};
fi

for t in $tests; do
    $1 $t
    echo "$t : passed";
done
