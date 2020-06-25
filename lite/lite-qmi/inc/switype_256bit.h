#ifndef __H__SWITYPE_256BIT__H__
#define __H__SWITYPE_256BIT__H__

#include <stdint.h>
/* 256 bit data type for bitmask and helper functions */
#define SWI_UINT256_WORD_COUNT 16
#define SWI_UINT256_BITS_PER_WORD 16

#define SWI_UINT256_WORD_OFFSET(val) ((val) / SWI_UINT256_BITS_PER_WORD)
#define SWI_UINT256_BIT_OFFSET(val)  ((val) % SWI_UINT256_BITS_PER_WORD)

/**
 * SWI 256 bit data type
 */
typedef struct
{
       uint16_t word[SWI_UINT256_WORD_COUNT];
}swi_uint256_t;

/**
 * Set relevant bit in provided mask
 * @param[in] pMask Pointer to bit mask
 * @param[in] bit Bit to set
 */
void swi_uint256_set_bit(swi_uint256_t *pMask,uint8_t bit);

/**
 * Clear relevant bit in provided mask
 * @param[in] pMask Pointer to bitmask
 * @param[in] bit Bit to clear
 */
void swi_uint256_clear_bit(swi_uint256_t *pMask,uint8_t bit);

/**
 * Get relevant bit from provided mask
 * @param[in] Mask Bit mask
 * @param[in] bit Bit to extract
 * @return Bit value
 *         - 0 - bit is not set
 *         - 1 - bit is set
 */
uint8_t swi_uint256_get_bit(swi_uint256_t Mask, uint8_t bit);

/**
 * Print the log mask to syslog. Use only for debugging
 * @param[in] mask Bit mask
 */
void swi_uint256_print_mask(swi_uint256_t mask);

#define SWI_UINT256_INT_VALUE {{0}}

typedef void (* logger)(uint8_t lvl, const char* buff);

#endif //__H__SWITYPE_256BIT__H__

