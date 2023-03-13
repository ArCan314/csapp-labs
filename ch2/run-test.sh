# g++ -fopenmp ./homework.cc -o ./temp.out -O1 -std=c++1z
# ./temp.out

# g++ -fopenmp ./2.13.cc -o ./temp.out -O1 -std=c++1z
# ./temp.out

# g++ ./2.21.cc -o ./temp.out -O3 -std=c++1z
# if [ $? == "0" ]
# then
#     echo "2.21 TEST COMPLETE"
# else
#     echo "2.21 TEST FAILED"
# fi

g++ -fopenmp ./2.27,30,31,32,42,43,74.cc -o ./temp.out -O1 -std=c++1z
./temp.out

rm ./temp.out