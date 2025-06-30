/*
 * Public API wrapper for the read-only SPIROM configuration table.
 *
 * NOTE: The real implementation lives in lib/tenstorrent/bh_arc/read_only_table.c
 * and its private header within the same directory.  We avoid including that
 * internal header here so that application code does not depend on the
 * nanopb-generated files.  Only forward declarations and enums required by
 * callers are provided.
 */
#ifndef TT_READ_ONLY_TABLE_PUBLIC_H_
#define TT_READ_ONLY_TABLE_PUBLIC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward-declare the protobuf structure so users can keep a const pointer if
 * they need to pass it around.  The full definition is private to the bh_arc
 * library. */
typedef struct ReadOnly ReadOnly;

/* Enumeration of supported PCB types.  Keep in sync with bh_arc implementation. */
typedef enum {
    TT_PCB_TYPE_ORION   = 0,
    TT_PCB_TYPE_P100    = 1,
    TT_PCB_TYPE_P150    = 2,
    TT_PCB_TYPE_P300    = 3,
    TT_PCB_TYPE_UBB     = 4,
    TT_PCB_TYPE_UNKNOWN = 0xFF
} tt_pcb_type_t;

/* Load the table from SPIROM into an internal static structure.
 * Returns 0 on success or negative error code.
 */
int      load_read_only_table(uint8_t *buffer_space, uint32_t buffer_size);

/* Get a pointer to the decoded table (const – read-only). */
const ReadOnly *get_read_only_table(void);

/* Query utilities */
tt_pcb_type_t get_pcb_type(void);

/*
 * For firmware that does not link the full bh_arc library (e.g. DMC), provide
 * a lightweight inline implementation of is_p300_left_chip().  When the full
 * library is present this will be optimised away in favour of the external
 * definition, but if it is absent the inline keeps the symbol resolved at
 * compile-time (ODR-safe because it is 'static inline').
 */
static inline bool is_p300_left_chip(void)
{
    /* GPIO6 strap (bit 6) is only tied high on the P300 left-side chip. */
#define TT_RESET_UNIT_STRAP_REG_L 0x80030D20U
    return ((*((volatile uint32_t *)TT_RESET_UNIT_STRAP_REG_L)) & BIT(6)) != 0;
#undef TT_RESET_UNIT_STRAP_REG_L
}

uint32_t      get_asic_location(void);

#ifdef __cplusplus
}
#endif

#endif /* TT_READ_ONLY_TABLE_PUBLIC_H_ */ 