#include "rp2350_i2s.h"
#include "rp2350_i2s_out_16bit_2ch.pio.h"



void i2s_init(rp2350_i2s_t *obj){


	obj->pio = pio0;
	obj->sm = pio_claim_unused_sm(obj->pio, true);
	const unsigned offset = pio_add_program(obj->pio, &i2s_out_16bit_2ch_program);
	
	pio_gpio_init(obj->pio, obj->bck_pin);
	pio_gpio_init(obj->pio, obj->wsel_pin);
	pio_gpio_init(obj->pio, obj->din_pin);

	pio_sm_set_consecutive_pindirs(obj->pio, obj->sm, obj->bck_pin, 3, true);

	//pio_sm_set_consecutive_pindirs(obj->pio, obj->sm, obj->bck_pin, 3, true);
	obj->sys_cycle_256ths_per_pio_clock = 4096;

	pio_sm_config sm_config = i2s_out_16bit_2ch_program_get_default_config(offset);
	sm_config_set_sideset_pins(&sm_config, obj->bck_pin);
	sm_config_set_out_pins(&sm_config, obj->din_pin, 1);
	sm_config_set_clkdiv_int_frac8(&sm_config, obj->sys_cycle_256ths_per_pio_clock / 256U, obj->sys_cycle_256ths_per_pio_clock % 256U);

	/* restore original config */
	pio_sm_init(obj->pio, obj->sm, offset, &sm_config);


	/* setup a DMA channel */
	dma_channel_claim(obj->dma_channel);
	dma_channel_config cfg = dma_channel_get_default_config(obj->dma_channel);
	channel_config_set_dreq(&cfg, pio_get_dreq(obj->pio, obj->sm, true));
	channel_config_set_read_increment(&cfg, true);
	channel_config_set_write_increment(&cfg, false);
	channel_config_set_ring(&cfg, false, BUFFER_WRAP_BITS);
	channel_config_set_transfer_data_size(&cfg, DMA_SIZE_32);

	dma_channel_configure(obj->dma_channel, &cfg, &(obj->pio)->txf[obj->sm], &(obj->buffer[0]), SAMPLES_PER_CHUNK | (1U << 28), false);

	/* enable interrupt for dma, but leave it disabled in nvic */
	dma_channel_acknowledge_irq0(obj->dma_channel);
	dma_channel_set_irq0_enabled(obj->dma_channel, true);
	__dsb();
	irq_set_enabled(DMA_IRQ_0, false);
	dma_channel_start(obj->dma_channel);

}



