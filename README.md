Esericitazioni di Sistemi Operativi M
=====================================

In questo repository sono presenti tutt le esercitazioni del corso di Sistemi
Operativi M (http://lia.deis.unibo.it/Courses/som1314/) dell'Università di
Bologna.

Per compilare questi programmi è necessario utilizzare la stringa:

gcc -D_REENTRANT -o prog prog.c -lpthread

Per comodità sono solito eseguire: (es. Es1.c)

watch "clear && gcc -Wall -D_REETRANT -lpthread -o Es1.exe Es1.c && ./Es1.exe"
