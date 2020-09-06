#ifndef DECODE_H_
#define DECODE_H_

typedef struct decode_result{
	unsigned int address;       ///< Used by Panasonic & Sharp [16-bits]
	unsigned long value;        ///< Decoded value [max 32-bits]
	unsigned int magnitude;     ///< Used by MagiQuest [16-bits]
	int bits;                   ///< Number of bits in decoded value
	volatile unsigned int *rawbuf;      ///< Raw intervals in 50uS ticks
	unsigned int rawlen;        ///< Number of records in rawbuf
	int overflow;               ///< true if IR raw code too long
}decode_result;

extern int MATCH(int measured, int desired);
extern int MATCH_MARK(int measured_ticks, int desired_us);
extern int MATCH_SPACE(int measured_ticks, int desired_us);
extern bool decodeNEC(decode_result *result, int pin_num, uint32_t * out_value);


#endif /* DECODE_H_ */
