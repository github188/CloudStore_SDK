#!/bin/sh

echo "Setting Parameters ... ..."
# project param
PROJECT_PATH=$PWD
PATH=$PATH:/opt/hisi-linux-nptl/arm-hisiv100-linux/target/bin
# output files
CPPCHECK_OUT=$PROJECT_PATH/cppcheck_rslt.xml
GTEST_OUT=$PROJECT_PATH/gtest_rslt.xml
GCOVR_OUT=$PROJECT_PATH/gcovr_rslt.xml


# run cppcheck
echo "Running cppcheck ... ..."
cppcheck --enable=all --xml $PROJECT_PATH  2> $CPPCHECK_OUT

# run unit test
echo "Building Unit-Test ... ..."
cd ./test
make clean
make
echo "Running Unit-Test ... ..."
./ut.out --gtest_output=xml:$GTEST_OUT


# run gcovr
echo "Running gcovr ... ..."
gcovr --branches -x -r .. > $GCOVR_OUT

cd ..
echo "BUILD Completed!"