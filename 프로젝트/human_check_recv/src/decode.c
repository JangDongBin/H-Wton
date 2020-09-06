#include <tizen.h>
#include <service_app.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <Ecore.h>
#include <time.h>
#include <unistd.h>
#include "decode.h"
#include "IR.h"

#define TICKS_LOW(us)   ((int) ((us)/67 ))     // (us) / ((MICROS_PER_TICK:50 / LTOL:75 ) * 100)
#define TICKS_HIGH(us)  ((int) ((us)/40 + 1))  // (us) / ((MICROS_PER_TICK:50 / UTOL:125) * 100) + 1
#define MARK_EXCESS_MICROS    100
#define RAW_BUFFER_LENGTH  101
#define REPEAT 0xFFFFFFFF

#define NEC_BITS          32
#define NEC_HDR_MARK    9000
#define NEC_HDR_SPACE   4500
#define NEC_BIT_MARK     560
#define NEC_ONE_SPACE   1690
#define NEC_ZERO_SPACE   560
#define NEC_RPT_SPACE   2250

int MATCH(int measured, int desired);
int MATCH_MARK(int measured_ticks, int desired_us);
int MATCH_SPACE(int measured_ticks, int desired_us);

int MATCH(int measured, int desired)
{
    bool passed = ((measured >= TICKS_LOW(desired)) && (measured <= TICKS_HIGH(desired)));
    return passed;
}

int MATCH_MARK(int measured_ticks, int desired_us) {
    bool passed = ((measured_ticks >= TICKS_LOW(desired_us + MARK_EXCESS_MICROS)) && (measured_ticks <= TICKS_HIGH(desired_us + MARK_EXCESS_MICROS)));
    return passed;
}

int MATCH_SPACE(int measured_ticks, int desired_us)
{
    bool passed = ((measured_ticks >= TICKS_LOW(desired_us - MARK_EXCESS_MICROS)) && (measured_ticks <= TICKS_HIGH(desired_us - MARK_EXCESS_MICROS)));
    return passed;
}

typedef struct {
    uint8_t rcvstate;        ///< State Machine state
    uint8_t recvpin;         ///< Pin connected to IR data from detector
    uint8_t blinkpin;
    uint8_t blinkflag;       ///< true -> enable blinking of pin on IR processing
    unsigned int rawlen;         ///< counter of entries in rawbuf
    unsigned int timer;           ///< State timer, counts 50uS ticks.
    unsigned int rawbuf[RAW_BUFFER_LENGTH];  ///< raw data
    uint8_t overflow;        ///< Raw buffer overflow occurred
} irparams_t;

irparams_t irparams;

bool decodeNEC(decode_result * result, int pin_num, uint32_t *out_value)
{
	//int ret = 0;
	long data = 0;
	int offset = 1;
	//ret = resource_read_ir_sensor(pin_num, &out_value);
	//if(ret != 0)
	//{
	//	return false;
	//}
	if(!MATCH_MARK(result->rawbuf[offset], NEC_HDR_MARK))
	{
		return false;
	}
	offset++;

	if((irparams.rawlen == 4) && MATCH_SPACE(result->rawbuf[offset], NEC_RPT_SPACE) && MATCH_MARK(result->rawbuf[offset + 1], NEC_BIT_MARK))
	{
		result->bits = 0;
		result->value = REPEAT;
		return true;
	}

	if(irparams.rawlen < (2 * NEC_BITS) + 4)
	{
		return false;
	}
	if (!MATCH_SPACE(result->rawbuf[offset], NEC_HDR_SPACE)) {
	        return false;
	    }
	    offset++;

	    // Build the data
	    for (int i = 0; i < NEC_BITS; i++) {
	        // Check data "mark"
	        if (!MATCH_MARK(result->rawbuf[offset], NEC_BIT_MARK)) {
	            return false;
	        }
	        offset++;

	        if (MATCH_SPACE(result->rawbuf[offset], NEC_ONE_SPACE)) {
	            data = (data << 1) | 1;
	        } else if (MATCH_SPACE(result->rawbuf[offset], NEC_ZERO_SPACE)) {
	            data = (data << 1) | 0;
	        } else {
	            return false;
	        }
	        offset++;
	    }

	    // Success
	    result->bits = NEC_BITS;
	    result->value = data;

	    return true;

}
