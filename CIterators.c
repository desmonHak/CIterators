/**
 * @file CIterators.c
 * @brief Implementación de iteradores genéricos para estructuras de datos en C
 *
 * Este archivo contiene la implementación de un sistema de iteradores genéricos
 * que puede trabajar con cualquier tipo de estructura de datos, proporcionando
 * operaciones básicas de iteración y nuevas funcionalidades extendidas.
 */

#ifndef CITERATORS_C
#define CITERATORS_C

#include "CIterators.h"
#include <stdlib.h>
#include <assert.h>

/* Implementación básica de iterador para arrays genéricos */
void *generic_array_next(Iterator *it) {
    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;
    if(iter->index == -1){
        iter->index = 0;
        it->current = iter->elements[iter->index];
        return it;
    }
    if (iter->index + 1 < iter->size) {
        iter->index++;
        it->current = iter->elements[iter->index];
        return it;
    }
    it->current = NULL;
    return NULL;
}


bool generic_array_equal(const Iterator *a, const Iterator *b)
{
    const GenericArrayIterator *ia = (GenericArrayIterator *)a->impl;
    const GenericArrayIterator *ib = (GenericArrayIterator *)b->impl;
    return ia->index == ib->index && ia->elements == ib->elements;
}


void generic_array_destroy(Iterator *it)
{
    GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;
    free(iter->elements);
    free(iter);
    it->impl = NULL;
}

Iterator create_generic_array_iterator(void *array, size_t size, size_t element_size) {
    void **elements = malloc(size * sizeof(void *));
    if (!elements)
        return (Iterator){0};

    for (size_t i = 0; i < size; i++) {
        elements[i] = (char *)array + i * element_size;
    }

    GenericArrayIterator *impl = malloc(sizeof(GenericArrayIterator));
    if (!impl) {
        free(elements);
        return (Iterator){0};
    }

    impl->elements = elements;
    impl->index = -1;  // Inicializar a -1
    impl->size = size;
    impl->element_size = element_size;

    Iterator iter = {
        .next = generic_array_next,
        .equal = generic_array_equal,
        .deref = generic_array_deref,
        .destroy = generic_array_destroy,
        .category = FORWARD_ITERATOR,
        .impl = impl,
        .current = NULL  // Inicializar a NULL
    };
    return iter;
}

/* Implementación de iterador de rango */
static void *range_next(Iterator *it) {
    RangeIterator *iter = (RangeIterator *)it->impl;
    int next_val = iter->current + iter->step;
    if ((iter->step > 0 && next_val >= iter->end) ||
        (iter->step < 0 && next_val <= iter->end)) {
        return NULL;
    }
    iter->current = next_val;
    it->current = &iter->current; // it->current apunta a la dirección de iter->current
    return it;
}

static bool range_equal(const Iterator *a, const Iterator *b)
{
    const RangeIterator *ia = (RangeIterator *)a->impl;
    const RangeIterator *ib = (RangeIterator *)b->impl;
    return ia->current == ib->current && ia->end == ib->end && ia->step == ib->step;
}

static void *range_deref(const Iterator *it)
{
    return it->current;
}

static void range_destroy(Iterator *it)
{
    free(it->impl);
    it->impl = NULL;
}

Iterator create_range_iterator(int start, int end, int step)
{
    if (step == 0)
        return (Iterator){0};

    RangeIterator *impl = malloc(sizeof(RangeIterator));
    if (!impl)
        return (Iterator){0};

    *impl = (RangeIterator){
        .start = start,    // Almacenar valor inicial
        .current = start - step, // Se ajusta para que el primer next() sea correcto
        .end = end,
        .step = step};

    Iterator iter = {
        .next = range_next,
        .equal = range_equal,
        .deref = range_deref,
        .destroy = range_destroy,
        .is_valid = range_is_valid,
        .category = INPUT_ITERATOR,
        .impl = impl,
        .current = NULL};

    return iter;
}

/* Implementación de zip de iteradores */
static void *multi_zip_next(Iterator *it) {
    MultiZipIterator *iter = (MultiZipIterator *)it->impl;
    bool all_valid = true;
    void **elements = malloc(iter->count * sizeof(void *));  // Asignar memoria aquí

    for (size_t i = 0; i < iter->count; i++) {
        if (iter->iterators[i].next(&iter->iterators[i])) {
            elements[i] = iter->iterators[i].deref(&iter->iterators[i]);
        } else {
            all_valid = false;
            break; // Si un iterador se agota, salir del bucle
        }
    }

    if (!all_valid) {
        free(elements); // Liberar memoria si no todos son válidos
        it->current = NULL;
        return NULL;
    }

    it->current = elements; // Asignar el array de elementos a it->current
    return it;
}



static bool multi_zip_equal(const Iterator *a, const Iterator *b) {
    MultiZipIterator *ia = (MultiZipIterator *)a->impl;
    MultiZipIterator *ib = (MultiZipIterator *)b->impl;

    if (ia->count != ib->count) {
        return false;
    }

    for (size_t i = 0; i < ia->count; i++) {
        if (!ia->iterators[i].equal(&ia->iterators[i], &ib->iterators[i])) {
            return false;
        }
    }

    return true;
}

static void *multi_zip_deref(const Iterator *it) {
    return it->current;
}

static void multi_zip_destroy(Iterator *it) {
    MultiZipIterator *iter = (MultiZipIterator *)it->impl;
    for (size_t i = 0; i < iter->count; i++) {
        iter->iterators[i].destroy(&iter->iterators[i]);
    }
    free(iter->iterators);
    free(iter);
    it->impl = NULL;
}

Iterator filter_iterator(Iterator it, bool (*filter_fn)(void *)) {
    FilterIterator *impl = malloc(sizeof(FilterIterator));
    if (!impl)
        return (Iterator){0};

    *impl = (FilterIterator){
        .source = it,
        .filter_fn = filter_fn};

    Iterator iter = {
        .next = filter_next,
        .equal = filter_equal,
        .deref = filter_deref,
        .destroy = filter_destroy,
        .is_valid = filter_is_valid,
        .category = FILTER_ITERATOR, // Corrección
        .impl = impl,
        .current = NULL};

    return iter;
}

/* Implementación de iterador de filtro */
static void *filter_next(Iterator *it)
{
    FilterIterator *iter = (FilterIterator *)it->impl;

    while (iter->source.next(&iter->source))
    {
        void *element = iter->source.deref(&iter->source);
        if (iter->filter_fn(element))
        {
            it->current = element;
            return it;
        }
    }

    it->current = NULL;
    return NULL;
}


Iterator multi_zip_iterators(Iterator* iterators, size_t count) {
    MultiZipIterator *impl = malloc(sizeof(MultiZipIterator));
    if (!impl)
        return (Iterator){0};

    Iterator* owned_iterators = malloc(count * sizeof(Iterator));
    bool* valid = malloc(count * sizeof(bool));
    void** elements = malloc(count * sizeof(void*));

    if (!owned_iterators || !valid || !elements) {
        free(owned_iterators);
        free(valid);
        free(elements);
        free(impl);
        return (Iterator){0};
    }

    for (size_t i = 0; i < count; i++) {
        owned_iterators[i] = iterators[i];
        valid[i] = true;
        elements[i] = NULL;
    }

    // Inicializamos la bandera de primera iteración
    *impl = (MultiZipIterator){
        .iterators = owned_iterators,
        .count = count,
        .valid = valid,
        .elements = elements,
        .first = true
    };

    Iterator iter = {
        .next = multi_zip_next,
        .equal = multi_zip_equal,
        .deref = multi_zip_deref,
        .destroy = multi_zip_destroy,
        .is_valid = multi_zip_is_valid,
        .category = ZIP_ITERATOR,
        .impl = impl,
        .current = NULL   // Se inicializa en NULL, ya que se actualizará en next
    };

    return iter;
}


static bool filter_equal(const Iterator *a, const Iterator *b)
{
    const FilterIterator *ia = (FilterIterator *)a->impl;
    const FilterIterator *ib = (FilterIterator *)b->impl;
    return ia->source.equal(&ia->source, &ib->source);
}

static void *filter_deref(const Iterator *it)
{
    return it->current;
}

static void filter_destroy(Iterator *it)
{
    FilterIterator *iter = (FilterIterator *)it->impl;
    iter->source.destroy(&iter->source);
    free(iter);
    it->impl = NULL;
}

/* Implementación de iterador de mapeo */
static void *map_next(Iterator *it) {
    MapIterator *iter = (MapIterator *)it->impl;

    if (iter->source.next(&iter->source)) {
        void *element = iter->source.deref(&iter->source);
        it->current = iter->map_fn(element);
        return it;
    }

    it->current = NULL;
    return NULL;
}

static bool map_equal(const Iterator *a, const Iterator *b)
{
    const MapIterator *ia = (MapIterator *)a->impl;
    const MapIterator *ib = (MapIterator *)b->impl;
    return ia->source.equal(&ia->source, &ib->source);
}

static void *map_deref(const Iterator *it)
{
    return it->current;
}

static void map_destroy(Iterator *it)
{
    MapIterator *iter = (MapIterator *)it->impl;
    iter->source.destroy(&iter->source);
    free(iter);
    it->impl = NULL;
}

Iterator map_iterator(Iterator it, void *(*map_fn)(void *)) {
    MapIterator *impl = malloc(sizeof(MapIterator));
    if (!impl)
        return (Iterator){0};

    *impl = (MapIterator){
        .source = it,
        .map_fn = map_fn};

    Iterator iter = {
        .next = map_next,
        .equal = map_equal,
        .deref = map_deref,
        .destroy = map_destroy,
        .is_valid = map_is_valid,
        .category = MAP_ITERATOR, // Corrección
        .impl = impl,
        .current = NULL};

    return iter;
}

/* Implementación de avance rápido */
bool iterator_advance(Iterator *it, size_t n)
{
    if (!it || !it->impl)
        return false;

    for (size_t i = 0; i < n; i++)
    {
        if (!it->next(it))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Reinicia un iterador a su posición inicial
 * @param it Iterador a reiniciar
 */
void iterator_reset(Iterator *it) {
    if (!it || !it->impl) return;

    switch (it->category) {
        case INPUT_ITERATOR: { // RangeIterator
            RangeIterator *range = (RangeIterator *)it->impl;
            range->current = range->start;  // Reiniciar al valor inicial
            it->current = &range->current; // Establecer el valor actual
            break;
        }
        case BIDIRECTIONAL_ITERATOR:
        case FORWARD_ITERATOR:
        case RANDOM_ACCESS_ITERATOR: {
            GenericArrayIterator *iter = (GenericArrayIterator *)it->impl;
            iter->index = 0;
            it->current = iter->elements[0];  // Apuntar al primer *elemento*
            break;
        }
        case ZIP_ITERATOR: {
            MultiZipIterator *zip_iter = (MultiZipIterator *)it->impl;
            for (size_t i = 0; i < zip_iter->count; ++i) {
                iterator_reset(&zip_iter->iterators[i]);
                zip_iter->valid[i] = true;
            }
            it->next(it); //Avanzar al primer elemento
            break;
        }
        case FILTER_ITERATOR: {
            FilterIterator *filter_iter = (FilterIterator *)it->impl;
            iterator_reset(&filter_iter->source);
            break;
        }
        case MAP_ITERATOR: {
            MapIterator *map_iter = (MapIterator *)it->impl;
            iterator_reset(&map_iter->source);
            break;
        }
        default:
            break;
    }
}


/**
    @brief Convierte un iterador en un array dinámico
    @param it Iterador a convertir
    @param count Puntero para almacenar el número de elementos
    @return Array dinámico con los elementos del iterador
    El llamador es responsable de liberar la memoria del array devuelto.
**/
void **iterator_to_array(Iterator it, size_t *count) {
    size_t n = 0;
    Iterator temp = it;
    while (temp.next(&temp)) {
        n++;
    }

    void **array = malloc(n * sizeof(void *));
    if (!array)
        return NULL;

    size_t i = 0;
    iterator_reset(&it);
    while (it.next(&it) && i < n) {
        array[i++] = it.deref(&it);
    }

    if (count)
        *count = n;
    return array;
}


/**
    @brief Aplica una función a cada elemento del iterador
    @param it Iterador a procesar
    @param func Función a aplicar a cada elemento
    */
void iterator_foreach(Iterator it, void(func)(void *)) {
    while (it.next(&it)) {
        func(it.deref(&it));
    }
}

/**
    @brief Busca un elemento en el iterador
    @param it Iterador donde buscar
    @param value Valor a buscar
    @param cmp Función de comparación
    @return Puntero al elemento encontrado o NULL si no se encuentra
    */
void* iterator_find(Iterator it, const void *value, int(cmp)(const void *, const void *))
{
    while (it.next(&it))
    {
        void *current = it.deref(&it);
        if (cmp(current, value) == 0)
        {
            return current;
        }
    }
    return NULL;
}

/**
    @brief Verifica si algún elemento cumple con una condición
    @param it Iterador a verificar
    @param pred Función predicado
    @return true si algún elemento cumple la condición, false en caso contrario
*/
bool iterator_any(Iterator it, bool(pred)(void *))
{
    while (it.next(&it))
    {
        if (pred(it.deref(&it)))
        {
            return true;
        }
    }
    return false;
}

/**
    @brief Verifica si todos los elementos cumplen con una condición
    @param it Iterador a verificar
    @param pred Función predicado
    @return true si todos los elementos cumplen la condición, false en caso contrario
*/
bool iterator_all(Iterator it, bool(pred)(void *))
{
    while (it.next(&it))
    {
        if (!pred(it.deref(&it)))
        {
            return false;
        }
    }
    return true;
}

/**
    @brief Crea un iterador para un array de strings (char*)
    @param array Array de strings
    @param count Número de elementos en el array
    @return Iterador configurado para el array de strings
    **/
Iterator create_string_array_iterator(const char **array, size_t count) {

    return create_generic_array_iterator((void *)array, count, sizeof(char *));
}

bool generic_array_is_valid(const Iterator* it) {
    const GenericArrayIterator* iter = (const GenericArrayIterator*)it->impl;
    return iter->index < iter->size;
}

bool range_is_valid(const Iterator* it) {
    const RangeIterator* iter = (const RangeIterator*)it->impl;
    if (iter->step > 0) {
        return iter->current < iter->end;
    } else {
        return iter->current > iter->end;
    }
}


bool multi_zip_is_valid(const Iterator* it) {
    const MultiZipIterator* iter = (const MultiZipIterator*)it->impl;
    for (size_t i = 0; i < iter->count; i++) {
        if (!iter->valid[i]) {
            return false;
        }
    }
    return true;
}
bool filter_is_valid(const Iterator* it) {
    const FilterIterator* iter = (const FilterIterator*)it->impl;
    return iter->source.is_valid ? iter->source.is_valid(&iter->source) : (iter->source.current != NULL);
}

bool map_is_valid(const Iterator* it) {
    const MapIterator* iter = (const MapIterator*)it->impl;
    return iter->source.is_valid ? iter->source.is_valid(&iter->source) : (iter->source.current != NULL);
}





#endif // CITERATORS_C