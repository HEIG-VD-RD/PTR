# Rapport - Laboratoire 1

Auteurs: Rafael Dousse

## cpu_loop
Dans cet exercice il nous faut écrire un programme qui compte le nombre d'itération d'une boucle exécuté durant un certain nombres de secondes. Ici, le calcul est fait durant 3 secondes.
### Tableau de nombres d'itérations mesurées sans optimisations

Nbr de mesures| addition (int) | multiplication (float) | division (float) |
|:---:|:---:|:---:|:---:|
|1. |426610 |496653 | 410210|
|2. | 422436| 486187| 492736|
|3. | 510918| 508722| 399900|
|4. | 402093| 484915| 405921|
|5. | 365423| 502004| 394751|
|6. |519026| 483548| 390598|
|7. |411227 |496503 | 413093|
|8. |508412 | 525489| 426224|
|9. |483736 | 366926| 404973|
|10. |522280 |458140 | 404442|
|Moyenne| 457216,1| 480908,7|414284,8|

### Tableau de nombres d'itérations mesurées en -O2

Nbr de mesures| addition (int) | multiplication (float) | division (float) |
|:---:|:---:|:---:|:---:|
|1. | 425981|497126 |401829 |
|2. | 382605| 392585| 505858|
|3. | 405125| 501270|533250 |
|4. |486302 | 517312|373751 |
|5. | 368808|506250 |528275 |
|6. | 373904| 410251| 378292|
|7. |387545 |359032 | 376475|
|8. | 368512| 521256|381246 |
|9. |374389 | 388831| 386247|
|10. |395807 |468927 |389940 |
|Moyenne|396,897.8 | 456,284.0| 425,516.3|


### Tableau de nombres d'itérations mesurées en -O3

Nbr de mesures| addition (int) | multiplication (float) | division (float) |
|:---:|:---:|:---:|:---:|
|1. |417180 |388009 |482911 |
|2. |535626 | 492755| 433465|
|3. |380320 | 505582| 527769|
|4. | 438863| 521018|431475 |
|5. |532709 |382439 | 518505|
|6. | 420381| 511415|538167 |
|7. |392884 |368261 |520447 |
|8. | 401587| 531895| 405059|
|9. | 381951| 518107|527290 |
|10. | 387486| 528271| 409436|
|Moyenne|  428,898.7|474,775.2 | 479,452.4|

Les mesures montre plusieurs itérations prises pour l'addition d'un int, la division d'un float et la multiplication d'un float. Une moyenne à été faite a chaque fois pour avoir une vision générale de ces valeurs entre chaque optimisation. Ces calculs ne sont pas d'une précision absolue étant donné que uniquement 10 mesures de chaques ont été prises mais peut déjà nous donner une idée de la différence entre les optimisations. On peut voir que l'optimisation -O3 est la plus rapide pour les 3 opérations. L'optimisation -O2 est la deuxième plus rapide et les calculs sans optimisation est la plus lente. Par contre une chose surprenante est que l'on s'attendrait é ce que les additions d'entiers soient plus rapides que les opérations avec des floats. Mais ici, on voit que les opérations avec des floats sont plus rapides que les opérations avec des entiers. Cela pourrait être du au fait que dans la FPGA se trouve une unité de calcul pour les floats qui est très rapide et ce qui fait que la différence entre les opérations avec des entiers et des floats est très faible. Ou même un pipelining qui est plus efficace pour les floats que pour les entiers.


## STAT

Quand on fait la command `ps` on obtient l'information des processus qui sont en cours d'exécution avec leur PID, le temps d'exécution le moment ou ils ont été lancés leur noms ou encore leur état avec le début de colonne STAT. Les états possibles sont selon la page du manuel de ps:
```bash
D    uninterruptible sleep (usually IO)
               I    Idle kernel thread
               R    running or runnable (on run queue)
               S    interruptible sleep (waiting for an event to complete)
               T    stopped by job control signal
               t    stopped by debugger during the tracing
               W    paging (not valid since the 2.6.xx kernel)
               X    dead (should never be seen)
               Z    defunct ("zombie") process, terminated but not reaped by its parent

       For BSD formats and when the stat keyword is used, additional characters may be displayed:

               <    high-priority (not nice to other users)
               N    low-priority (nice to other users)
               L    has pages locked into memory (for real-time and custom IO)
               s    is a session leader
               l    is multi-threaded (using CLONE_THREAD, like NPTL pthreads do)
               +    is in the foreground process group
```

## Ordonnement en temps partagé

Lorsque l'on exécute `./cpu_loop` sur le même cpu et qu'on execute 2 3 ou 4 fois l'instruction en "même temps" (comme ceci: `./cpu_loop & ./cpu_loop & ./cpu_loop & ./cpu_loop & ./cpu_loop`) alors seul un cpu execute le programme ce qui fait que le temps est partagé entre les différent processus et que les nombres d'itérations sont réduite plus on ajoute de processus. 

## Migration de tâches

Personnelement je ne voit rien de spécial lorsque j'execute `get_cpu_number` uniquement sur un cpu et une autre fois que sur le 2eme cpu en même temps puis `cpu_loop` sur un des 2 cpu. Par contre quand j'ouvre htop on voit que l'un des 2 cpu est utilisé à 100% pendant que `cpu_loop` ` est en cours d'exécution. Mais je ne vois pas de migration de tâches.

## Niceness

Quand j'utilise la commande `nice -n 5 ./cpu_loop & ./cpu_loop` les 2 programmes prennent un peu plus de temps a s'exécuter. et si je met une priorité de -20  alors les programmes s'exécutent normalement et on peut remarquer que dans les 2 autres terminals, les programmes `get_cpu_number` qui s'exécutent sur les 2 cpu sont ralentis et même s'arrêtent pendant l'exécution de `cpu_loop`. Cela est du au fait que `cpu_loop` a une priorité plus haute que `get_cpu_number`.

## Threads - niceness

L'exécution de nice va influencer l'ordonnancement des processus et donc au niveau user tandis que dans l'exercice des threads on parle de la priorité des threads pour des controles plus directes de ce qui se passe dans le programme. 

Pour l'exercice des threads je me serai attendu à ce que les threads avec une priorité plus haute aient plus d'itérations comparé à ceux avec une priorité plus basse mais ce n'est pas toujours le cas.