for file in testy/*.php
do
    echo ${file%%.*}
    ./gigachad_compiler < ${file%%.*}.php > ${file%%.*}.ifjcode
    # echo "compiler:   " $?
    if [ $? -ne 0 ]
    then
        echo "Compiler error!"
        exit 1
    fi
    ./ic22int ${file%%.*}.ifjcode > ${file%%.*}.cmp
    # echo "interpreter:" $?
    if [ $? -ne 0 ]
    then
        echo "Compiler error!"
        exit 1
    fi
    diff ${file%%.*}.res ${file%%.*}.cmp
    # echo "diff       :" $?
    if [ $? -ne 0 ]
    then
        echo "Compiler error!"
        exit 1
    fi
    echo "OK!"
done

rm testy/*.ifjcode
rm testy/*.cmp
