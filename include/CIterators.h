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

typedef enum {
    INPUT_ITERATOR,
    FORWARD_ITERATOR,
    BIDIRECTIONAL_ITERATOR,
    RANDOM_ACCESS_ITERATOR,
    ZIP_ITERATOR,          
    FILTER_ITERATOR,       
    MAP_ITERATOR           
} IteratorCategory;

typedef struct Iterator {
    void* (*next)(struct Iterator*);
    bool  (*equal)(const struct Iterator*, const struct Iterator*);
    void* (*deref)(const struct Iterator*);
    void  (*destroy)(struct Iterator*);
    IteratorCategory category;
    void* impl;
    void* current;
} Iterator;

typedef struct {
    void** elements;  // Array de punteros void*
    size_t index;
    size_t size;
    size_t element_size;
} GenericArrayIterator;

typedef struct
{
    int start;    // Nuevo campo para almacenar inicio real
    int current;
    int end;
    int step;
} RangeIterator;

typedef struct {
    Iterator* iterators; // Array de iteradores
    size_t count;        // Número de iteradores
    bool* valid;         // Array para verificar la validez de cada iterador
    void** elements;     // Array para almacenar el elemento actual de cada iterador
    bool first;          // Bandera para la primera iteración
} MultiZipIterator;

typedef struct
{
    Iterator source;
    bool (*filter_fn)(void *);
} FilterIterator;

typedef struct
{
    Iterator source;
    void *(*map_fn)(void *);
} MapIterator;

void* generic_array_next(Iterator* it);
bool generic_array_equal(const Iterator* a, const Iterator* b);

void generic_array_destroy(Iterator* it);

Iterator create_generic_array_iterator(void* array, size_t size, size_t element_size);

/**
 * @brief Crea un iterador de rango numérico
 * @param start Valor inicial del rango
 * @param end Valor final del rango (exclusivo)
 * @param step Paso de iteración
 * @return Iterador configurado
 */
Iterator create_range_iterator(int start, int end, int step);

/**
 * @brief Combina dos iteradores en pares (elemento1, elemento2)
 * @param it1 Primer iterador
 * @param it2 Segundo iterador
 * @return Iterador zip que produce pares de elementos
 */
Iterator zip_iterators(Iterator it1, Iterator it2);

/**
 * @brief Filtra elementos de un iterador usando función predicado
 * @param it Iterador fuente
 * @param filter_fn Función de filtrado (devuelve true para elementos a mantener)
 * @return Iterador filtrado
 */
Iterator filter_iterator(Iterator it, bool (*filter_fn)(void *));

/**
 * @brief Transforma elementos de un iterador usando función de mapeo
 * @param it Iterador fuente
 * @param map_fn Función de transformación
 * @return Iterador transformado
 */
Iterator map_iterator(Iterator it, void *(*map_fn)(void *));

/**
 * @brief Avanza un iterador N posiciones
 * @param it Iterador a avanzar
 * @param n Número de posiciones a avanzar
 * @return true si el avance fue exitoso para todas las posiciones
 */
bool iterator_advance(Iterator *it, size_t n);

/**
 * @brief Reinicia un iterador a su posición inicial
 * @param it Iterador a reiniciar
 */
void iterator_reset(Iterator *it);


/**
    @brief Crea un iterador para un array de strings (char*)
    @param array Array de strings
    @param count Número de elementos en el array
    @return Iterador configurado para el array de strings
    **/
Iterator create_string_array_iterator(const char **array, size_t count);
   
void iterator_foreach(Iterator it, void(func)(void *));

void **iterator_to_array(Iterator it, size_t *count);
Iterator multi_zip_iterators(Iterator* iterators, size_t count);
void* iterator_find(Iterator it, const void *value, int(cmp)(const void *, const void *));
bool iterator_any(Iterator it, bool(pred)(void *));
bool iterator_all(Iterator it, bool(pred)(void *));

#endif // CITERATORS_H