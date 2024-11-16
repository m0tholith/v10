#ifndef LIST_H
#define LIST_H

#define LIST(__type)                                                           \
    struct {                                                                   \
        size_t size, capacity;                                                 \
        __type *data;                                                          \
    } *
#define LIST_INIT(__list, __init_capacity)                                     \
    __list = malloc(sizeof(*__list));                                          \
    __list->size = 0;                                                          \
    __list->capacity = __init_capacity;                                        \
    __list->data = malloc(__list->capacity * sizeof(*__list->data));
#define LIST_IDX(__list, __idx) __list->data[__idx]
#define LIST_RESIZE(__list)                                                    \
    if (__list->size >= __list->capacity) {                                    \
        __list->capacity *= 2;                                                 \
        __list->data =                                                         \
            realloc(__list->data, __list->capacity * sizeof(*__list->data));   \
    } else if (__list->size < __list->capacity / 2) {                          \
        __list->capacity /= 2;                                                 \
        __list->data =                                                         \
            realloc(__list->data, __list->capacity * sizeof(*__list->data));   \
    }
#define LIST_APPEND(__list, __value)                                           \
    LIST_RESIZE(__list);                                                       \
    __list->data[__list->size++] = __value;
#define LIST_POP(__list, __idx)                                                \
    for (int __i = __idx + 1; __i < __list->size; __i++) {                     \
        LIST_IDX(__list, __i - 1) = LIST_IDX(__list, __i);                     \
    }                                                                          \
    __list->size--;                                                            \
    LIST_RESIZE(__list);
#define LIST_FREE(__list)                                                      \
    free(__list->data);                                                        \
    free(__list);

#endif // !LIST_H
