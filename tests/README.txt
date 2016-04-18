Sposób użycia:
W katalogu z testami umieścić skompilowany program `raise`. Następnie należy
uruchomić `make all`.
Przykładowy poprawny wynik:
  $ make all
  ./test_exitcode
  ./test_stdio
  ./test_syscall
  ./test_malloc
  ./test_bigmalloc
  ./test_stack
  ./test_pageprot
  ./test_pie

Można również uruchomić test pojedynczo (o ile został wcześniej skompilowany
przy pierwszym `make all`):
  $ ./test_stdio; echo $?
  0

Punktacja: każdy test jest warty 1.25 punktów.
