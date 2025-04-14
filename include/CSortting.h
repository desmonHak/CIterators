/**
 * @file CSorting.h
 * @brief Implementación del algoritmo Introsort usando iteradores genéricos
 * 
 * Este archivo contiene la implementación de un algoritmo de ordenación híbrido
 * (Introsort) que combina QuickSort, HeapSort e Insertion Sort, diseñado para
 * trabajar con la estructura de iteradores genéricos definida en CIterators.h
 */

#ifndef CSORTING_H
#define CSORTING_H

#include "CIterators.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>


/**
 * @typedef CompareFunc
 * @brief Tipo de función para comparar elementos
 * @param a Primer elemento a comparar
 * @param b Segundo elemento a comparar
 * @return Entero negativo si a < b, cero si a == b, positivo si a > b
 */
typedef int (*CompareFunc)(const void*, const void*);
 

void generic_sort(Iterator *it, CompareFunc compare);

#endif