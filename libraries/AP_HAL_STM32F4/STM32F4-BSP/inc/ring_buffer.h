#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include <stdint.h>
/**
 * Ring buffer type.
 *
 * The buffer is empty when head == tail.
 *
 * The buffer is full when the head is one byte in front of the tail,
 * modulo buffer length.
 *
 * One byte is left free to distinguish empty from full. */
typedef struct ring_buffer {
    volatile uint8_t *buf; /**< Buffer items are stored into */
    uint16_t head;         /**< Index of the next item to remove */
    uint16_t tail;         /**< Index where the next item will get inserted */
    uint16_t size;         /**< Buffer capacity */
	uint16_t count;
} ring_buffer;

/**
 * Initialise a ring buffer.
 *
 *  @param rb   Instance to initialise
 *
 *  @param size Number of items in buf.  The ring buffer will always
 *              leave one element unoccupied, so the maximum number of
 *              elements it can store will be size - 1.  Thus, size
 *              must be at least 2.
 *
 *  @param buf  Buffer to store items into
 */
static inline void rb_init(ring_buffer *rb, uint16_t size, uint8_t *buf) {
    rb->head = 0;
    rb->tail = 0;
	rb->count = 0;
    rb->size = size;
    rb->buf = buf;
}

/**
 * @brief Return the number of elements stored in the ring buffer.
 * @param rb Buffer whose elements to count.
 */
static inline uint16_t rb_full_count(ring_buffer *rb)
{
    __IO ring_buffer *arb = rb;
    return arb->count;
}

/**
 * @brief Returns true if and only if the ring buffer is full.
 * @param rb Buffer to test.
 */
static inline uint8_t rb_is_full(ring_buffer *rb) 
{
		if (rb->count == rb->size)
				return 1;
		return 0;
}

/**
 * @brief Returns true if and only if the ring buffer is empty.
 * @param rb Buffer to test.
 */
static inline uint8_t rb_is_empty(ring_buffer *rb) 
{
		if (rb->count == 0)
				return 1;
		return 0;
}

/**
 * Append element onto the end of a ring buffer.
 * @param rb Buffer to append onto.
 * @param element Value to append.
 */
static inline uint8_t rb_insert(ring_buffer *rb, uint8_t element) {
		if (rb->count == rb->size)
				return 0;
		else
		{
			rb->buf[rb->tail++] = element;
			rb->count++;
			if (rb->tail == rb->size)
					rb->tail = 0;
		}
		return 1;
}

/**
 * @brief Remove and return the first item from a ring buffer.
 * @param rb Buffer to remove from, must contain at least one element.
 */
static inline uint8_t rb_remove(ring_buffer *rb) 
{
    uint8_t ch;
	if (0 == rb->count)
			return 0;
	ch = rb->buf[rb->head++];
	rb->count--;
	if (rb->head == rb->size)
			rb->head = 0;
    return ch;
}

/**
 * @brief Append an item onto the end of a non-full ring buffer.
 *
 * If the buffer is full, removes its first item, then inserts the new
 * element at the end.
 *
 * @param rb Ring buffer to insert into.
 * @param element Value to insert into ring buffer.
 * @return On success, returns -1.  If an element was popped, returns
 *         the popped value.
 */
static inline uint8_t rb_push_insert(ring_buffer *rb, uint8_t element) {
    uint8_t ret = 0;
    if (rb_is_full(rb)) {
        ret = rb_remove(rb);
    }
    rb_insert(rb, element);
    return ret;
}

/**
 * @brief Discard all items from a ring buffer.
 * @param rb Ring buffer to discard all items from.
 */
static inline void rb_reset(ring_buffer *rb) {
    rb->tail = rb->head;
	rb->count = 0;
}

#ifdef __cplusplus
} // extern "C"
#endif

#endif
