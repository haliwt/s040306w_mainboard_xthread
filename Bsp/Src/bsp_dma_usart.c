#include "bsp.h"

void usart1_dma_send(uint8_t *txbuf,uint16_t txlen)
{

    LL_DMA_DisableChannel(DMA1,LL_DMA_CHANNEL_2);

    LL_DMA_ConfigAddresses(DMA1,LL_DMA_CHANNEL_2,
                            (uint32_t)txbuf,
                            (uint32_t)&USART1->TDR,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_2,txlen);

    LL_DMA_ClearFlag_TC1(DMA1);//TC1 -> clear transfer complete flag
    LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_2);

    LL_USART_EnableDMAReq_TX(USART1);

}

