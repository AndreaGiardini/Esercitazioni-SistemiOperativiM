FAQ
===

* Come distinguo un thread da un altro visto che hanno tutti lo stesso PID?
E' necessario utilizzare la fuzione pthread_self() per ricevere il ThreadIdentifier (TID)

* [Esercitazione2/Es1.c] Perchè se inizializzo srand(time(NULL)) dentro il thread i numeri si ripetono e invece se lo inizializzo nel padre ciò non accade (vedi commit 94dfc8ac1516043fdb33a5b4cf4c8e470aa76969) ?