#ifndef RP2350_I2S
#define RP2350_I2S

#include <math.h>
#include <complex.h>

#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/gpio.h"

#include "rp2350_i2s_out_16bit_2ch.pio.h"


#define BUFFER_WRAP_BITS 13
#define BYTES_PER_CHUNK 4096
#define CHANNELS 2
#define BUFFER_WRAP_BITS 13
#define SAMPLES_PER_CHUNK (BYTES_PER_CHUNK / (CHANNELS * sizeof(int16_t)))



struct rp2350_i2s{


	unsigned long sys_cycle_256ths_per_pio_clock;
	uint8_t dma_channel;
	uint8_t bck_pin, wsel_pin, din_pin;
	unsigned sm; //state machine for PIO

	PIO pio;
	int16_t *buffer;
};
typedef struct rp2350_i2s rp2350_i2s_t;


void i2s_init(rp2350_i2s_t *obj);



#endif
