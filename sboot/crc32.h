#ifndef _CRC32_H_
#define _CRC32_H_

#include "types.h"

BEGIN_DECLS

/**
 * @brief calculate crc value
 * @param[in] prev_crc: previous calculated crc value
 * @param[in] pbuf: data need to be calculated
 * @param[in] len: data length
 * @return crc value
 */
uint32_t crc32(uint32_t prev_crc, const uint8_t *pbuf, uint32_t len);

END_DECLS

#endif /* _CRC32_H_ */
