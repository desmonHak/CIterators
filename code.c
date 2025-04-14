#include "CIterators.h"
#include "CSortting.c"

// Funciones de comparación para diferentes tipos
int compare_int(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int compare_str(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

int main() {
    // Ejemplo con enteros
    int int_arr1[] = {10, 20, 30, 40};
    Iterator int_it1 = create_generic_array_iterator(int_arr1, 4, sizeof(int));
    
    printf("Enteros:\n");
    do {
        int* val = (int*)int_it1.deref(&int_it1);
        printf("%d\n", *val);
    } while (int_it1.next(&int_it1));
    int_it1.destroy(&int_it1);

    // Ejemplo con strings
    const char* str_arr1[] = {"Hola", "Mundo", "de", "Iteradores"};
    Iterator str_it1 = create_generic_array_iterator(str_arr1, 4, sizeof(char*));
    
    printf("\nStrings:\n");
    do {
        const char** val = (const char**)str_it1.deref(&str_it1);
        printf("%s\n", *val);
    } while (str_it1.next(&str_it1));
    str_it1.destroy(&str_it1);

    int int_arr[] = {10, 20, 30, 40, 25, 15, 5};
    Iterator int_it = create_generic_array_iterator(int_arr, 7, sizeof(int));
    
    printf("Antes de ordenar (enteros):\n");
    do {
        int* val = (int*)int_it.deref(&int_it);
        printf("%d ", *val);
    } while (int_it.next(&int_it));
    
    generic_sort(&int_it, compare_int);
    
    printf("\nDespues de ordenar (enteros):\n");
    do {
        int* val = (int*)int_it.deref(&int_it);
        printf("%d ", *val);
    } while (int_it.next(&int_it));
    
    int_it.destroy(&int_it);
    
    // Ejemplo con strings
    const char* str_arr[] = {"banana", "apple", "orange", "grape", "kiwi"};
    Iterator str_it = create_generic_array_iterator(str_arr, 5, sizeof(char*));
    
    printf("\n\nAntes de ordenar (strings):\n");
    do {
        const char** val = (const char**)str_it.deref(&str_it);
        printf("%s ", *val);
    } while (str_it.next(&str_it));
    
    generic_sort(&str_it, compare_str);
    
    printf("\nDespues de ordenar (strings):\n");
    do {
        const char** val = (const char**)str_it.deref(&str_it);
        printf("%s ", *val);
    } while (str_it.next(&str_it));
    
    str_it.destroy(&str_it);
    
}