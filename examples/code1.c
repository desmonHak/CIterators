#include "CSortting.h"
#include "CIterators.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Función de ejemplo para filtrar números pares
bool is_even(void *element) {
    int num = *(int *)element;
    return num % 2 == 0;
}

// Función de ejemplo para mapear un entero a su cuadrado
void *square(void *element) {
    int num = *(int *)element;
    int *result = malloc(sizeof(int));
    *result = num * num;
    return result;
}

// Función de ejemplo para imprimir un entero
void print_int(void *element) {
    printf("%d ", *(int *)element);
}

// Función de comparación para iterator_find
int compare_ints(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

int main() {
    // 1. Generic Array Iterator
    int arr[] = {1, 2, 3, 4, 5};
    size_t arr_size = sizeof(arr) / sizeof(arr[0]);
    Iterator array_iter = create_generic_array_iterator(arr, arr_size, sizeof(int));
    printf("Generic Array Iterator: ");
    while (array_iter.next(&array_iter)) {
        printf("%d ", *(int *)array_iter.deref(&array_iter));
    }
    printf("\n");
    array_iter.destroy(&array_iter);
   
    // 2. Range Iterator
    Iterator range_iter = create_range_iterator(0, 10, 2);
    printf("Range Iterator: ");
    while (range_iter.next(&range_iter)) {
        printf("%d ", *(int *)range_iter.deref(&range_iter));
    }
    printf("\n");
    range_iter.destroy(&range_iter);

    // 3. Zip Iterator (Multiple Arrays)
    int arr1[] = {1, 2, 3};
    int arr2[] = {4, 5, 6, 7};
    int arr3[] = {7, 8, 9};
    size_t arr_size1 = sizeof(arr1) / sizeof(arr1[0]);
    size_t arr_size2 = sizeof(arr2) / sizeof(arr2[0]);
    size_t arr_size3 = sizeof(arr3) / sizeof(arr3[0]);

    Iterator iter1 = create_generic_array_iterator(arr1, arr_size1, sizeof(int));
    Iterator iter2 = create_generic_array_iterator(arr2, arr_size2, sizeof(int));
    Iterator iter3 = create_generic_array_iterator(arr3, arr_size3, sizeof(int));

    Iterator iters[] = {iter1, iter2, iter3};
    size_t num_iters = sizeof(iters) / sizeof(iters[0]);

    Iterator zip_iter = multi_zip_iterators(iters, num_iters);
    printf("Multi Zip Iterator: ");
    while (zip_iter.next(&zip_iter)) {
        void** elements = (void**)zip_iter.deref(&zip_iter);
        printf("(%d, %d, %d) ", *(int*)elements[0], *(int*)elements[1], *(int*)elements[2]);
    }
    printf("\n");
    zip_iter.destroy(&zip_iter);

    // 4. Filter Iterator
    int arr4[] = {1, 2, 3, 4, 5, 6};
    size_t arr_size4 = sizeof(arr4) / sizeof(arr4[0]);
    Iterator iter4 = create_generic_array_iterator(arr4, arr_size4, sizeof(int));
    Iterator filter_iter = filter_iterator(iter4, is_even);
    printf("Filter Iterator (even numbers): ");
    while (filter_iter.next(&filter_iter)) {
        printf("%d ", *(int *)filter_iter.deref(&filter_iter));
    }
    printf("\n");
    filter_iter.destroy(&filter_iter);

    // 5. Map Iterator
    int arr5[] = {1, 2, 3, 4, 5};
    size_t arr_size5 = sizeof(arr5) / sizeof(arr5[0]);
    Iterator iter5 = create_generic_array_iterator(arr5, arr_size5, sizeof(int));
    Iterator map_iter = map_iterator(iter5, square);
    printf("Map Iterator (squares): ");
    while (map_iter.next(&map_iter)) {
        printf("%d ", *(int *)map_iter.deref(&map_iter));
    }
    printf("\n");
    map_iter.destroy(&map_iter);

     // 6. Iterator Advance
    int arr6[] = {10, 20, 30, 40, 50};
    size_t arr_size6 = sizeof(arr6) / sizeof(arr6[0]);
    Iterator advance_iter = create_generic_array_iterator(arr6, arr_size6, sizeof(int));
    printf("Iterator Advance: ");
    iterator_advance(&advance_iter, 2); // Avanza 2 posiciones
    printf("%d ", *(int *)advance_iter.deref(&advance_iter));
    iterator_advance(&advance_iter, 1); // Avanza 1 posición
    printf("%d ", *(int *)advance_iter.deref(&advance_iter));
    printf("\n");
    advance_iter.destroy(&advance_iter);

    // 7. Iterator Reset
    int arr7[] = {100, 200, 300, 400, 500};
    size_t arr_size7 = sizeof(arr7) / sizeof(arr7[0]);
    Iterator reset_iter = create_generic_array_iterator(arr7, arr_size7, sizeof(int));
    iterator_advance(&reset_iter, 3); // Avanza 3 posiciones
    printf("Iterator Reset: ");
    printf("Current value before reset: %d\n", *(int *)reset_iter.deref(&reset_iter));
    iterator_reset(&reset_iter); // Reinicia el iterador
    printf("Current value after reset: %d\n", *(int *)reset_iter.deref(&reset_iter));
    reset_iter.destroy(&reset_iter);
   
    // 8. String Array Iterator
    const char *strings[] = {"hello", "world", "iterators"};
    size_t string_count = sizeof(strings) / sizeof(strings[0]);
    Iterator string_iter = create_string_array_iterator(strings, string_count);

    printf("String Array Iterator: ");
    while (string_iter.next(&string_iter)) {
        const char** val = (const char**)string_iter.deref(&string_iter);
        printf("%s ", *val);
    }
    printf("\n");
    string_iter.destroy(&string_iter);
   

     // 9. iterator_to_array
    int arr8[] = {7, 8, 9, 10, 11};
    size_t arr_size8 = sizeof(arr8) / sizeof(arr8[0]);
    Iterator iter8 = create_generic_array_iterator(arr8, arr_size8, sizeof(int));
    size_t count;
    int **new_array = (int **)iterator_to_array(iter8, &count);

    printf("iterator_to_array: ");
    for (size_t i = 0; i < count; i++) {
        printf("%d ", *new_array[i]);
    }
    printf("\n");

    free(new_array);
    iter8.destroy(&iter8);
   
    // 10. iterator_foreach
    int arr9[] = {12, 13, 14, 15, 16};
    size_t arr_size9 = sizeof(arr9) / sizeof(arr9[0]);
    Iterator iter9 = create_generic_array_iterator(arr9, arr_size9, sizeof(int));

    printf("iterator_foreach: ");
    iterator_foreach(iter9, print_int);
    printf("\n");
    iter9.destroy(&iter9);

    // 11. iterator_find
    int arr10[] = {20, 21, 22, 23, 24};
    size_t arr_size10 = sizeof(arr10) / sizeof(arr10[0]);
    Iterator iter10 = create_generic_array_iterator(arr10, arr_size10, sizeof(int));
    int value_to_find = 22;
    int *found_element = (int *)iterator_find(iter10, &value_to_find, compare_ints);

    printf("iterator_find: ");
    if (found_element != NULL) {
        printf("Found element: %d\n", *found_element);
    } else {
        printf("Element not found\n");
    }
    iter10.destroy(&iter10);

    // 12. iterator_any
    int arr11[] = {25, 26, 27, 28, 29};
    size_t arr_size11 = sizeof(arr11) / sizeof(arr11[0]);
    Iterator iter11 = create_generic_array_iterator(arr11, arr_size11, sizeof(int));
    bool has_even = iterator_any(iter11, is_even);

    printf("iterator_any: ");
    if (has_even) {
        printf("Array has even number(s)\n");
    } else {
        printf("Array has no even numbers\n");
    }
    iter11.destroy(&iter11);
   
     // 13. iterator_all
    int arr12[] = {30, 32, 34, 36, 38}; // Todos son pares
    size_t arr_size12 = sizeof(arr12) / sizeof(arr12[0]);
    Iterator iter12 = create_generic_array_iterator(arr12, arr_size12, sizeof(int));
    bool all_even = iterator_all(iter12, is_even);

    printf("iterator_all: ");
    if (all_even) {
        printf("All numbers are even\n");
    } else {
        printf("Not all numbers are even\n");
    }
    iter12.destroy(&iter12);
   
    return 0;
}
