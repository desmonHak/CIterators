/**
 * @file CSorting.c
 * @brief Implementación del algoritmo Introsort usando iteradores genéricos
 *
 * Este archivo contiene la implementación de un algoritmo de ordenación híbrido
 * (Introsort) que combina QuickSort, HeapSort e Insertion Sort, diseñado para
 * trabajar con la estructura de iteradores genéricos definida en CIterators.h
 */

#ifndef CSORTING_C
#define CSORTING_C

#include "CSortting.h"

/**
 * @brief Intercambia dos elementos en el array del iterador
 * @param iter Puntero al iterador genérico
 * @param i Índice del primer elemento
 * @param j Índice del segundo elemento
 */
static void swap_elements(GenericArrayIterator *iter, size_t i, size_t j)
{
    void *temp = iter->elements[i];
    iter->elements[i] = iter->elements[j];
    iter->elements[j] = temp;
}

/**
 * @brief Ordenación por inserción para pequeños segmentos
 * @param it Puntero al iterador
 * @param compare Función de comparación
 *
 * Esta función es eficiente para arrays pequeños (<16 elementos) y se usa
 * como optimización en Introsort para estos casos.
 */
static void insertion_sort(Iterator *it, CompareFunc compare)
{
    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;

    for (size_t i = 1; i < iter->size; i++)
    {
        size_t j = i;
        while (j > 0 && compare(iter->elements[j - 1], iter->elements[j]) > 0)
        {
            swap_elements(iter, j, j - 1);
            j--;
        }
    }
}

/**
 * @brief Función auxiliar para HeapSort que mantiene la propiedad de heap
 * @param it Puntero al iterador
 * @param n Tamaño del heap
 * @param i Índice del elemento a heapify
 * @param compare Función de comparación
 */
static void heapify(Iterator *it, size_t n, size_t i, CompareFunc compare)
{
    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;

    size_t largest = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;

    if (left < n && compare(iter->elements[left], iter->elements[largest]) > 0)
        largest = left;

    if (right < n && compare(iter->elements[right], iter->elements[largest]) > 0)
        largest = right;

    if (largest != i)
    {
        swap_elements(iter, i, largest);
        heapify(it, n, largest, compare);
    }
}

/**
 * @brief Implementación de HeapSort
 * @param it Puntero al iterador
 * @param compare Función de comparación
 *
 * Se usa cuando la recursión en QuickSort es demasiado profunda para garantizar
 * O(n log n) en el peor caso.
 */
static void heap_sort(Iterator *it, CompareFunc compare)
{
    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;
    size_t n = iter->size;

    // Construir el heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(it, n, i, compare);

    // Extraer elementos del heap
    for (int i = n - 1; i > 0; i--)
    {
        swap_elements(iter, 0, i);
        heapify(it, i, 0, compare);
    }
}

/**
 * @brief Función de partición para QuickSort
 * @param it Puntero al iterador
 * @param low Índice inferior
 * @param high Índice superior
 * @param compare Función de comparación
 * @return Índice del pivote después de la partición
 */
static size_t partition(Iterator *it, size_t low, size_t high, CompareFunc compare)
{
    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;

    void *pivot = iter->elements[high];
    size_t i = low;

    for (size_t j = low; j < high; j++)
    {
        if (compare(iter->elements[j], pivot) <= 0)
        {
            swap_elements(iter, i, j);
            i++;
        }
    }
    swap_elements(iter, i, high);
    return i;
}

/**
 * @brief Implementación principal de Introsort
 * @param it Puntero al iterador
 * @param begin Índice inicial
 * @param end Índice final
 * @param depth_limit Límite de profundidad de recursión
 * @param compare Función de comparación
 *
 * Esta función implementa el algoritmo Introsort que comienza con QuickSort,
 * cambia a HeapSort si la recursión es demasiado profunda, y usa InsertionSort
 * para subarrays pequeños.
 */
static void introsort_impl(Iterator *it, size_t begin, size_t end,
                           int depth_limit, CompareFunc compare)
{
    //GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;
    size_t size = end - begin + 1;

    // Usar insertion sort para arrays pequeños
    if (size < 16)
    {
        insertion_sort(it, compare);
        return;
    }

    // Si la profundidad es cero, usar heap sort
    if (depth_limit == 0)
    {
        heap_sort(it, compare);
        return;
    }

    // Caso normal: Quick Sort
    size_t pivot = partition(it, begin, end, compare);
    if (pivot > begin + 1)
        introsort_impl(it, begin, pivot - 1, depth_limit - 1, compare);
    if (pivot + 1 < end)
        introsort_impl(it, pivot + 1, end, depth_limit - 1, compare);
}

/**
 * @brief Función pública de ordenación genérica
 * @param it Puntero al iterador a ordenar
 * @param compare Función de comparación para determinar el orden
 *
 * Esta función ordena los elementos del iterador usando el algoritmo Introsort.
 * Calcula automáticamente la profundidad máxima de recursión y selecciona el
 * mejor algoritmo para cada caso particular.
 */
void generic_sort(Iterator *it, CompareFunc compare)
{
    if (!it || !it->impl)
        return;

    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;
    if (iter->size <= 1)
        return;

    // Calcular la profundidad máxima como 2 * log2(n)
    int depth_limit = 2 * log2(iter->size);

    introsort_impl(it, 0, iter->size - 1, depth_limit, compare);

    // Resetear el índice después de ordenar
    iter->index = 0;
    it->current = iter->elements[0];
}

#endif // CSORTING_C
