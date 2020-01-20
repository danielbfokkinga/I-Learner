# I-Learner
Prototype tool to build automata models from C code with an implementation of the L* algorithm from Dana Angluin.
Abstractions have been proposed to make the model tractable. The resulting automaton plays a similar role of UML interaction diagrams, but the current implementation does not allow for modelling recursive calls.

For more information see the following [thesis](https://theses.liacs.nl/pdf/DanielFokkinga.pdf).


To build, for a given a piece of C code, `X`, run
```
$ gcc -finstrument-functions -g -c X.o X.c
$ gcc -c -o trace.o trace.c
$ gcc X.o trace.o -o X
```
Next, define a file `alphabet.txt` containing the names of all the functions in `X` seperated by linebreaks.
Finally, compile `angluin.cc` with a compiler of your choice, provided you have defined the name of `X` as `EXECUTABLE` (see `angluin.cc` line 10).