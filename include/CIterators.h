/**
 * @file CIterators.h
 * @brief Sistema de iteradores genéricos para C
 */

#ifndef CITERATORS_H
#define CITERATORS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

/**
 * @enum IteratorCategory
 * @brief Categorías de iteradores compatibles.
 *
 * Define los diferentes tipos de iteradores que pueden implementarse.
 */
typedef enum {
    INPUT_ITERATOR,         /**< Iterador de entrada, solo permite una pasada hacia adelante. */
    FORWARD_ITERATOR,       /**< Iterador de avance, permite múltiples pasadas hacia adelante. */
    BIDIRECTIONAL_ITERATOR, /**< Iterador bidireccional, permite avanzar y retroceder. */
    RANDOM_ACCESS_ITERATOR, /**< Iterador de acceso aleatorio, permite saltos arbitrarios. */
    ZIP_ITERATOR,           /**< Iterador que agrupa elementos de varios iteradores. */
    FILTER_ITERATOR,        /**< Iterador que filtra elementos según una condición. */
    MAP_ITERATOR            /**< Iterador que transforma elementos mediante una función. */
} IteratorCategory;

/**
 * @struct Iterator
 * @brief Interfaz genérica para iteradores.
 *
 * Define las funciones básicas que debe tener cualquier iterador.
 */
typedef struct Iterator {
    void* (*next)(struct Iterator*);                                /**< Avanza al siguiente elemento y lo devuelve. */
    bool  (*equal)(const struct Iterator*, const struct Iterator*); /**< Compara dos iteradores. */
    void* (*deref)(const struct Iterator*);                         /**< Devuelve el elemento actual sin avanzar. */
    void  (*destroy)(struct Iterator*);                             /**< Libera recursos del iterador. */
    IteratorCategory category;                                      /**< Categoría del iterador. */
    void* impl;                                                     /**< Implementación interna del iterador (puntero a struct concreta). */
    void* current;                                                  /**< Elemento actual del iterador. */
} Iterator;

/**
 * @struct GenericArrayIterator
 * @brief Iterador para recorrer arrays genéricos de punteros.
 *
 * Permite recorrer un array de punteros void* con soporte para tamaños y posición actual.
 */
typedef struct GenericArrayIterator {
    void** elements;      /**< Array de punteros a elementos. */
    size_t index;         /**< Índice actual del iterador. */
    size_t size;          /**< Número total de elementos en el array. */
    size_t element_size;  /**< Tamaño en bytes de cada elemento. */
} GenericArrayIterator;

/**
 * @struct RangeIterator
 * @brief Iterador para generar secuencias numéricas.
 *
 * Similar a la función `range` en Python. Útil para bucles simples sobre rangos de enteros.
 */
typedef struct RangeIterator {
    int start;    /**< Valor inicial de la secuencia. */
    int current;  /**< Valor actual del iterador. */
    int end;      /**< Valor final (no inclusivo) de la secuencia. */
    int step;     /**< Incremento entre valores sucesivos. */
} RangeIterator;

/**
 * @struct MultiZipIterator
 * @brief Iterador para combinar múltiples iteradores en paralelo.
 *
 * Itera sobre varios iteradores al mismo tiempo, devolviendo una tupla con sus elementos actuales.
 */
typedef struct MultiZipIterator {
    Iterator* iterators; /**< Array de iteradores a combinar. */
    size_t count;        /**< Número total de iteradores. */
    bool* valid;         /**< Array que indica si cada iterador tiene un valor válido. */
    void** elements;     /**< Array que guarda el valor actual de cada iterador. */
    bool first;          /**< Bandera que indica si es la primera llamada a `next`. */
} MultiZipIterator;

/**
 * @struct FilterIterator
 * @brief Iterador que filtra elementos según una función de predicado.
 *
 * Permite omitir elementos que no cumplen con una condición especificada.
 */
typedef struct FilterIterator {
    Iterator source;               /**< Iterador fuente del que se obtienen los elementos. */
    bool (*filter_fn)(void *);     /**< Función que determina si un elemento debe incluirse. */
} FilterIterator;

/**
 * @struct MapIterator
 * @brief Iterador que transforma elementos usando una función de mapeo.
 *
 * Aplica una función a cada elemento del iterador fuente y devuelve el resultado.
 */
typedef struct MapIterator {
    Iterator source;              /**< Iterador fuente del que se obtienen los elementos. */
    void *(*map_fn)(void *);      /**< Función que transforma un elemento. */
} MapIterator;

void* generic_array_next(Iterator* it);
bool generic_array_equal(const Iterator* a, const Iterator* b);

void generic_array_destroy(Iterator* it);

Iterator create_generic_array_iterator(void* array, size_t size, size_t element_size);

Iterator create_range_iterator(int start, int end, int step);

Iterator filter_iterator(Iterator it, bool (*filter_fn)(void *));

Iterator map_iterator(Iterator it, void *(*map_fn)(void *));

bool iterator_advance(Iterator *it, size_t n);

void iterator_reset(Iterator *it);

Iterator create_string_array_iterator(const char **array, size_t count);
   
void iterator_foreach(Iterator it, void(func)(void *));

void **iterator_to_array(Iterator it, size_t *count);
Iterator multi_zip_iterators(Iterator* iterators, size_t count);
void* iterator_find(Iterator it, const void *value, int(cmp)(const void *, const void *));
bool iterator_any(Iterator it, bool(pred)(void *));
bool iterator_all(Iterator it, bool(pred)(void *));

#endif // CITERATORS_H