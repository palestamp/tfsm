#!/bin/bash

echo "Running unit tests:"



for i in tests/*_tests
do
    if test -f $i
    then
        # check if "in" - "out" files exists
        if test -f "${i}.out"
        then
            asserted=$(cat "${i}.out")
            TMPFILE=$(mktemp "${i}.tmp")

            if $VALGRIND ./${i} 2>> tests/tests.log 1> ${TMPFILE}
            then
                diff "${TMPFILE}"  "${i}.out" > "${i}.diff"
                if [ $? -ne 0 ]
                then
                    echo "ERROR ${i}. generating diff in ${i}.diff"
                else
                    echo "PASS ${i}"
                    rm -f "${i}.diff"
                fi
            else
                echo "ERROR in test $i: here's tests/tests.log"
                echo "------"
                tail tests/tests.log
            fi
            
            rm -f $TMPFILE
        else
            if $VALGRIND ./$i 2>> tests/tests.log
            then
                echo "PASS ${i}"
            else
                echo "ERROR in test $i: here's tests/tests.log"
                echo "------"
                tail tests/tests.log
            fi
        fi
    fi
done

echo ""
