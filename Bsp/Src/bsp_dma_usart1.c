#include "bsp.h"


//this is communication outside diplay board information .
void usart1_dma_send(uint8_t *txbuf,uint16_t txlen)
{

    if(txbuf ==NULL || txlen ==0) return ;

	gpro_t.usart1_dma_done = 0;
		
    LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_2);

    LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_2,
                            (uint32_t)txbuf,
                            (uint32_t)&USART1->TDR,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_2,txlen);

    LL_DMA_ClearFlag_TC2(DMA1);//TX-LL_DMA_CHANNEL_2 ;TC1 -> clear transfer complete flag
    LL_DMA_ClearFlag_TE2(DMA1);

	//Enable interrupt 
    LL_DMA_EnableIT_TC(DMA1,LL_DMA_CHANNEL_2);
	LL_DMA_EnableIT_TE(DMA1,LL_DMA_CHANNEL_2);

	//start DMA
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_2);

    //Enable USART1 DMA TX request
    LL_USART_EnableDMAReq_TX(USART1);

}

