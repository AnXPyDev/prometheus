pre kompiláciu potrebujete kompilátor gcc,
použite nasladujúci príkaz

gcc -std=c99 -I lib/ccore/include src/main.c -o pthc

repozitár obsahuje už predkompilovaný exe súbor pthc.exe

pre spustenie použite príkaz

.\pthc.exe examples\factorial.pth
