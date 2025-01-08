# -*- coding: utf-8 -*-
"""
Created on Tue Nov 26 08:54:57 2024

@author: mateo
"""

from math import gcd
from functools import reduce
from itertools import permutations 


#%% Test de schedulability

def is_schedulable(Ci, Ti) :
    """
    Simple fonction qui test la schedulability, avec pour rappel, ces deux paramètres en entrée :
    Ci : Liste des temps d'exécution (Ci)
    Ti : Liste des périodes (Ti)
    """
    if len(Ci) != len(Ti) :
        raise ValueError("Pas la même longueur !")
    
    U = 0       # On calcule la charge U
    for i in range(len(Ci)) :
        U += Ci[i] / Ti[i]    # C'est la formule qu'on connait depuis l'A4
    
    print(f"Charge du système (U) : {U}")
    
    return U <= 1     # La tâche est shedulable si U <= 1

# TEST pour voir si tout fonctionne

task = ["t1","t2","t3","t4","t5","t6","t7"]
Ci = [2, 2, 2, 2, 2, 2, 3]
Ti = [10, 10, 20, 20, 40, 40, 80]
if is_schedulable(Ci, Ti):
    print("Les tâches sont shedulables.")
else :
    print("Les tâches ne sont pas shedulables.")
    

#%%

# On doit faire un shceduler sur les jobs, lorsque la première tâche (t1) finit son execution + la période
# Alors elle a finit son premier job (J11), le deuxième job peut alors commencer (J12)
# Le but est alors de faire un scheduler sur les job et pas les tasks, et ce jusqu'à l'hyperperiode
# Il faut que ce soit shedulable jusqu'à l'hyperpériode, donc il faut la calculer aussi

def lcm(a, b):
    """C'est une fonction qui calcule le plus petit commun multiple (LCM) de deux nombres."""
    return abs(a * b) // gcd(a, b)

def hyperperiod(Ti):
    return reduce(lcm, Ti) # Comme ça c'est efficace

def is_valid_schedule(order, Ci, Ti):
    """
    Cette fonction me permet de determiner si un ordre de tâches est valide en 
    respectant les contraintes.
    On return le moment d'exécution des jobs et les temps de réponse.
    """
    H = hyperperiod(Ti)
    num_tasks = len(order)
    execution_log = []             # Log pour stocker les moments d'exécution et temps de réponse
    job_release = [0] * num_tasks  # Le temps de release des jobs de chaque tâche
    job_list = []                  # Liste des jobs exécutés sous la forme (Jji)
    total_waiting_time = 0         # Le temps total d'attente

    current_time = 0               # Et puis le temps actuel DANS la simulation

    while current_time < H:
        executed = False           # Pour savoir si on a exécuté une tâche à ce moment

        for t in order:
            task_id = int(t[1]) - 1  # On récupère l'index de la tâche
            
            # Il faut vérifier si la tâche est prête à s'exécuter
            if current_time >= job_release[task_id]:
                start_time = current_time
                end_time = start_time + Ci[task_id]
                
                # On vérifie la deadline
                if end_time > job_release[task_id] + Ti[task_id]:
                    return False, [], [], 0
                
                # On calcule le temps d'attente, c'est l'une des taches demandées (pour trouver le min)
                waiting_time = start_time - job_release[task_id]
                total_waiting_time += waiting_time
                
                response_time = end_time - job_release[task_id]
                execution_log.append((t, start_time, response_time))      # Log l'exécution et calculer le temps de réponse
                job_list.append(f"J{task_id+1}{(job_release[task_id] // Ti[task_id])}")  # Ajout dans la liste des jobs

                # On met à jour le temps de release pour le prochain job de cette tâche
                job_release[task_id] += Ti[task_id]
                current_time = end_time
                executed = True
                break  # Et enfin, on passe à la prochaine itération du temps
            
        if not executed:
            current_time += 1  # Si aucune tâche n'est prête on avance le temps

    return True, execution_log, job_list, total_waiting_time


def scheduling_tasks(task, Ci, Ti):
    """
    Cette petite fonction génère toutes les permutations des tâches 
    et renvoie les combinaisons valides.
    """
    valid_schedules = []                      # Liste pour stocker toutes les permutations valides
    min_waiting_time = float('inf')           # On initialise le temps d'attente minimal avec l'infini
    best_schedule = None                      

    all_orders = permutations(task)           # On génère toutes les permutations possible des tâches

    for order in all_orders:                  # On parcourt chaque permutation !
    
        # On vérifie si cette permutation est "valide" (respecte les deadlines)
        # et on récupère le log d'exécution, la liste des jobs, et le temps d'attente total
        is_valid, execution_log, job_list, total_waiting_time = is_valid_schedule(order, Ci, Ti)

        if is_valid:                          # Si la permutation est valide
            # On l'ajoute à la liste des permutations valides avec toutes ses infos
            valid_schedules.append((order, execution_log, job_list, total_waiting_time))
            
            # On compare son temps d'attente total avec le min_waiting_time connu
            if total_waiting_time < min_waiting_time:
                min_waiting_time = total_waiting_time                                   # Si c'est plus petit, on met à jour le minimum
                best_schedule = (order, execution_log, job_list, total_waiting_time)    # et on mémorise cette permutation comme étant la meilleure

    return valid_schedules, best_schedule

# TEST sur les tâches données en TP

task = ["t1", "t2", "t3", "t4", "t5", "t6", "t7"]
Ci = [2, 2, 2, 2, 2, 2, 3]
Ti = [10, 10, 20, 20, 30, 30, 40]

valid_schedules, best_schedule = scheduling_tasks(task, Ci, Ti)

print("\n=== Résultats des combinaisons valides ===")
for schedule, execution_log, job_list, waiting_time in valid_schedules:
    print(f"\nPermutation : {schedule}")
    print("Moment d'exécution des tâches :")
    for t, exec_time, resp_time in execution_log:
        print(f"  Tâche {t} exécutée à t = {exec_time}, temps de réponse : {resp_time}")
    print("Liste des jobs exécutés :", job_list)
    print(f"Temps total d'attente : {waiting_time}")

print("\n=======================================================")
print("\n=== Meilleure permutation (temps d'attente min) =======")
print("\n=======================================================")

if best_schedule:
    order, execution_log, job_list, waiting_time = best_schedule
    print(f"Permutation : {order}")
    
    print("Liste des jobs exécutés :", job_list)
    print(f"Temps total d'attente : {waiting_time}")

