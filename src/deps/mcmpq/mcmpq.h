#include <stddef.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdalign.h>

#define NUM_SLOTS(x) (sizeof(x.slots) / sizeof(x.slots[0]))

#define CACHE_LINE 64
#define HEAD(q) atomic_load_explicit(&(q)->head, memory_order_acquire)
#define TAIL(q) atomic_load_explicit(&(q)->tail, memory_order_acquire)

#define QUEUE(type, num_slots) struct {             \
        alignas(CACHE_LINE) _Atomic size_t head;    \
        alignas(CACHE_LINE) _Atomic size_t tail;    \
        alignas(CACHE_LINE) struct {                \
            _Atomic size_t turn;                    \
            type data;                              \
        } slots[num_slots];                         \
    }

#define QUEUE_DEFINE(type, num_slots, name) typedef QUEUE(type, num_slots) name

#define enqueue(queue, item) do {                                       \
        size_t head = atomic_fetch_add_explicit(&(queue.head), 1, memory_order_acq_rel); \
        while ((head / NUM_SLOTS(queue)) * 2 != atomic_load_explicit(&(queue.slots[head % NUM_SLOTS(queue)].turn), memory_order_acquire)) { /* busy-wait */ } \
        queue.slots[head % NUM_SLOTS(queue)].data = item;               \
        atomic_store_explicit(&(queue.slots[head % NUM_SLOTS(queue)].turn), (head / NUM_SLOTS(queue)) * 2 + 1, memory_order_release); \
    } while(0)

#define dequeue(queue, item) do {                                       \
        size_t tail = atomic_fetch_add_explicit(&(queue.tail), 1, memory_order_acq_rel); \
        while ((tail / NUM_SLOTS(queue)) * 2 + 1 != atomic_load_explicit(&(queue.slots[tail % NUM_SLOTS(queue)].turn), memory_order_acquire)) { /* busy-wait */ } \
        item = queue.slots[tail % NUM_SLOTS(queue)].data;               \
        atomic_store_explicit(&(queue.slots[tail % NUM_SLOTS(queue)].turn), (tail / NUM_SLOTS(queue)) * 2 + 2, memory_order_release); \
    } while(0)

/* static inline bool try_enqueue(queue_t *queue, const void *item) { */
/*     size_t head = atomic_load_explicit(&queue->head, memory_order_acquire); */
/*     for (;;) { */
/*         slot_t *slot = &queue->slots[head % SLOTS]; */
/*         if ((head / SLOTS) * 2 == atomic_load_explicit(&slot->turn, memory_order_acquire)) { */
/*             if (atomic_compare_exchange_strong_explicit(&queue->head, &head, head + 1, memory_order_acq_rel, memory_order_acquire)) { */
/*                 memcpy(slot->data, item, SLOT); */
/*                 atomic_store_explicit(&slot->turn, (head / SLOTS) * 2 + 1, memory_order_release); */
/*                 return true; */
/*             } */
/*         } else { */
/*             size_t prev_head = head; */
/*             head = atomic_load_explicit(&queue->head, memory_order_acquire); */
/*             if (head == prev_head) { */
/*                 return false; */
/*             } */
/*         } */
/*     } */
/* } */

#define try_enqueue(queue, item, success) do {                     \
        size_t head = atomic_load_explicit(&(queue.head, memory_order_acquire)); \
        for (;;) {                                                      \
            slot_t *slot = &(queue.slots[head % NUM_SLOTS(queue)]);     \
            if ((head / NUM_SLOTS(queue)) * 2 == atomic_load_explicit(&(queue.slots[head % NUM_SLOTS(queue)].turn), memory_order_acquire)) { \
                if (atomic_compare_exchange_strong_explicit(&(queue.head), &head, head + 1, memory_order_acq_rel, memory_order_acquire)) { \
                    queue.slots[head % NUM_SLOTS(queue)].data = item;   \
                    atomic_store_explicit(&queue.slots[head % NUM_SLOTS(queue)].turn, (head / NUM_SLOTS(queue)) * 2 + 1, memory_order_release); \
                    return_value = true;                                \
                    break;                                              \
                }                                                       \
            } else {                                                    \
                size_t prev_head = head;                                \
                head = atomic_load_explicit(&(queue.head), memory_order_acquire); \
                if (head == prev_head) {                                \
                    return_value = false;                               \
                    break;                                              \
                }                                                       \
            }                                                           \
        }                                                               \
    } while(0)

/* static inline bool try_dequeue(queue_t *queue, void *item) { */
/*     size_t tail = atomic_load_explicit(&queue->tail, memory_order_acquire); */
/*     for (;;) { */
/*         slot_t *slot = &queue->slots[tail % SLOTS]; */
/*         if ((tail / SLOTS) * 2 + 1 == atomic_load_explicit(&slot->turn, memory_order_acquire)) { */
/*             if (atomic_compare_exchange_strong_explicit(&queue->tail, &tail, tail + 1, memory_order_acq_rel, memory_order_acquire)) { */
/*                 memcpy(item, slot->data, SLOT); */
/*                 atomic_store_explicit(&slot->turn, (tail / SLOTS) * 2 + 2, memory_order_release); */
/*                 return true; */
/*             } */
/*         } else { */
/*             size_t prev_tail = tail; */
/*             tail = atomic_load_explicit(&queue->tail, memory_order_acquire); */
/*             if (tail == prev_tail) { */
/*                 return false; */
/*             } */
/*         } */
/*     } */
/* } */

#define try_dequeue(queue, item, success) do {                          \
        size_t tail = atomic_load_explicit(&queue->tail, memory_order_acquire); \
        for (;;) {                                                      \
            if ((tail / NUM_SLOTS(queue)) * 2 + 1 == atomic_load_explicit(&(queue->slots[tail % NUM_SLOTS(queue)].turn), memory_order_acquire)) { \
                if (atomic_compare_exchange_strong_explicit(&(queue.tail), &tail, tail + 1, memory_order_acq_rel, memory_order_acquire)) { \
                    queue.slots[head % NUM_SLOTS(queue)].data = item;   \
                    atomic_store_explicit(&slot->turn, (tail / SLOTS) * 2 + 2, memory_order_release); \
                    return_value = true;                                \
                    break;                                              \
                }                                                       \
            } else {                                                    \
                size_t prev_tail = tail;                                \
                tail = atomic_load_explicit(&(queue.tail), memory_order_acquire); \
                if (tail == prev_tail) {                                \
                    return_value = false;                               \
                    break;                                              \
                }                                                       \
            }                                                           \
        }                                                               \
    } while(0)
