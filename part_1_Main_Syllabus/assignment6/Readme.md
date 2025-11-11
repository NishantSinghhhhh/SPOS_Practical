You fixed the multiple-main issue ✅. Now:

g++ -std=c++17 -O2 main.cpp firstfit.cpp nextfit.cpp bestfit.cpp worstfit.cpp util.cpp -o memsim


That compile error
res.blockLeft doesn’t exist. In your Result it’s called remaining.
Edit util.cpp → in print_result() replace:

res.blockLeft[i]


with:

res.remaining[i]


(And anywhere else: blockLeft ➜ remaining, alloc ➜ allocation.)

Recompile:

g++ -std=c++17 -O2 main.cpp firstfit.cpp nextfit.cpp bestfit.cpp worstfit.cpp util.cpp -o memsim


You ran without a mode
The program needs an argument: first|next|best|worst|all. Also give input.

Create input.txt if you haven’t:

5
100 500 200 300 600
4
212 417 112 426


Run any of these:

./memsim first < input.txt
./memsim next  < input.txt
./memsim best  < input.txt
./memsim worst < input.txt
./memsim all   < input.txt


If you still see “not allocated” rows or weird numbers, show me your current Result struct and print_result() and I’ll align the names for you.