cd sum
gcc main.c
mv a.out ../
cd ..
./a.out
./a.out > README
rm -f a.out

git add .
git commit -m "new"
git push
