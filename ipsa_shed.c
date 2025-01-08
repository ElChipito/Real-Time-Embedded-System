#include "ipsa_shed.h"
#include <stdio.h>

/* Priorités des mes tâches */
#define RESET_TASK_PRIORITY         (tskIDLE_PRIORITY + 5)  // La plus haute priorité pour cette tache de RESET
#define PRINT_WORKING_PRIORITY      (tskIDLE_PRIORITY + 4)
#define MULTIPLY_NUMBERS_PRIORITY   (tskIDLE_PRIORITY + 3)
#define CONVERT_TEMP_PRIORITY       (tskIDLE_PRIORITY + 2)
#define BINARY_SEARCH_PRIORITY      (tskIDLE_PRIORITY + 1)

/* Toutes les fonctions statiques */
static void print_working(void *pvParameters);
static void vTaskConvertTemp(void *pvParameters);
static void vTaskMultiplyNumbers(void *pvParameters);      // Toutes ces fonctions sont internes au fichier
static void vTaskBinarySearch(void *pvParameters);
static void vTaskResetCheck(void *pvParameters);  

/* On crée deux petites fonctions utilitaires */
static void FahrenheitToCelsius(double fahrenheit);
static int  binary_search(int arr[], int l, int r, int x);

static int arr[50]; // On initialise un array pour pouvoir faire une recherche binaire

/* LA fonction ! Celle qu'on appelle pour lancer le scheduler et créer les tâches */
void ipsa_sched(void)
{
	for(int i = 0; i < 50; i++) {  // On rempli le tableau pour la recherche binaire (on l'utilise ensuite)
			arr[i] = i; 
		}

    xTaskCreate(print_working,       "PrintWorking",    configMINIMAL_STACK_SIZE, NULL, PRINT_WORKING_PRIORITY,      NULL);
    xTaskCreate(vTaskConvertTemp,    "ConvertTemp",     configMINIMAL_STACK_SIZE, NULL, CONVERT_TEMP_PRIORITY,       NULL);
    xTaskCreate(vTaskMultiplyNumbers,"MultiplyNumbers", configMINIMAL_STACK_SIZE, NULL, MULTIPLY_NUMBERS_PRIORITY,   NULL);
    xTaskCreate(vTaskBinarySearch,   "BinarySearch",    configMINIMAL_STACK_SIZE, NULL, BINARY_SEARCH_PRIORITY,      NULL);
    xTaskCreate(vTaskResetCheck,      "ResetCheck",      configMINIMAL_STACK_SIZE, NULL, RESET_TASK_PRIORITY,        NULL);

	vTaskStartScheduler(); // On lance le scheduler Free RTOS
}


/* Tâche 1 : On affiche "Working" périodiquement ! */
static void print_working(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(100);  // Toute les 100 millisecondes
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        printf("TACHE 1 : Working\n");
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* Tâche 2 : Elle permet la conversion de Fahrenheit à Celsius périodiquement */
static void vTaskConvertTemp(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(60); // Toutes les 60 millisecondes
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        double fahrenheit = 98.6;              // C'est la valeur à convertir (ça fait 37°C)
        FahrenheitToCelsius(fahrenheit);       // On appelle la fonction qui convertit
        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* On utilise cette fonction pour convertir */
static void FahrenheitToCelsius(double fahrenheit)
{
    double celsius = (fahrenheit - 32.0) * 5.0 / 9.0;   // C'est la formule de conversion
    printf("TACHE 2 : %.2f °F = %.2f °C\n", fahrenheit, celsius); // On print le résultat ici
}

/* Tâche 3 : Multiplication périodique */
static void vTaskMultiplyNumbers(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(150);  // On la fait toute les 150 millisecondes
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        long int x1 = 19838074558;                // J'ai mis deux nombre au hasard
        long int x2 = 65642864582;
        long int result = x1 * x2;  
        printf("TACHE 3 : Resultat de la multiplication: %ld\n", result); // Je print le résultat directement

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}


/* Tâche 4 : Recherche binaire périodiquement */
static void vTaskBinarySearch(void *pvParameters) 
{
    const TickType_t xDelay = pdMS_TO_TICKS(25); // On la fait toute les 25 ms, et c'est la moins prioritaire
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        int x = 25; // On cherche le nombre 25 dans le tableau de valeur définit précédemment
        int result = binary_search(arr, 0, 49, x); // On appelle la fonction de recherche

        if (result != -1)
            printf("TACHE 4 : Element trouvé à l'index : %d\n", result);   // Si on le trouve on renvoi son index
        else
            printf("TACHE 4 : J'ai pas trouvé !\n");                   // Sinon on renvoie une erreur

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}

/* Fonction pour la recherche binaire */
static int binary_search(int arr[], int l, int r, int x)
{
    while (l <= r)          // Tant que la partie non explorée est valide
    {
        int m = l + (r - l) / 2;    // On calcule l'indice milieu m
        if (arr[m] == x)            // Si l'élément à l'indice m est la valeur recherchée
            return m;               // Alors on la return
        else if (arr[m] < x)
            l = m + 1;              // On se restreint à la partie de droite (ou de gauche juste après)
        else
            r = m - 1;
    }
    return -1;
}


/* Tâche 5 : RESET */

static void vTaskResetCheck(void *pvParameters)
{
    const TickType_t xDelay = pdMS_TO_TICKS(200); // On laisse un délai de 200 millisecondes
    TickType_t xLastWakeTime = xTaskGetTickCount();

    int paramReset = 0; // 0 par défaut, passe à 1 si on appuie sur la touche '1'

    while(1)
    {
        printf("paramReset = %d\n", paramReset); // On affiche le paramètre d, moment

		// Pour Reset, il faut taper '1' puis 'entrée' 

        int c = fgetc(stdin); // Lecture bloquante

		// boucle while, on sors de la boucle si on rencontre un retour à la ligne alors si on veux sortir (ctrl+d = EOF c'est fin de fichier)
        while ( (c != '\n') && (c != EOF) ) 
        {
            if(c == '1') // Condition principale, si on donne 1 alors RESET
            {
                paramReset = 1;
                printf("=============================================================== \n");
                printf("======                RESET déclenché !!                 ====== \n");
                printf("=============================================================== \n");

            }
            c = fgetc(stdin);
        }

        paramReset = 0; // On remet paramReset à 0 pour la prochaine itération

        vTaskDelayUntil(&xLastWakeTime, xDelay);
    }
}


/*
 Les étapes à suivre à chaque TP pour compiler et faire run les codes

 cd /mnt/c/Users/mateo/Downloads/FreeRTOSv202107.00/FreeRTOS/Demo/Posix_GCC/
 Faire 'make'  pour compiler
 Pour run, on va dans le dossier : cd build/ 
 Pour executer: ./posix_demo 

*/