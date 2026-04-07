#include "bsp.h"

#include "stm32g0xx.h"

#define PAGE_STATUS_ERASED    0xFFFF
#define PAGE_STATUS_VALID     0xAAAA


#define EEPROM_PAGE_SIZE     2048U        // one page is 2KB
#define EEPROM_PAGE0_ADDR     0x0800F000    // 最后一页 Page30

#define EEPROM_PAGE1_ADDR     0x0800F800  // 最后一页 Page31
#define EEPROM_PAGE_ADDR      0x0800F800  // 最后一页 Page31

#define RECORD_SIZE          8           // 双字记录

typedef struct {
    uint16_t virtAddr;  // 虚拟地址
    uint32_t data;      // 数据
    uint8_t  bcc;       // 校验
    uint8_t  reserved;  // 对齐
} EEPROM_Record_t;

/* 解锁 Flash */
static void FLASH_Unlock(void)
{
    if (FLASH->CR & FLASH_CR_LOCK) {
        FLASH->KEYR = 0x45670123U;
        FLASH->KEYR = 0xCDEF89ABU;
    }
}

/* 上锁 Flash */
static void FLASH_Lock(void)
{
    FLASH->CR |= FLASH_CR_LOCK;
}

/* 等待空闲 */
static void FLASH_WaitBusy(void)
{
    while (FLASH->SR & FLASH_SR_BSY1) {}
}

static uint32_t EEPROM_GetActivePage(void)
{
    uint16_t status0 = *(uint16_t*)EEPROM_PAGE0_ADDR;
    uint16_t status1 = *(uint16_t*)EEPROM_PAGE1_ADDR;

    if (status0 == PAGE_STATUS_VALID) return EEPROM_PAGE0_ADDR;
    if (status1 == PAGE_STATUS_VALID) return EEPROM_PAGE1_ADDR;

    // 默认 Page0
    return EEPROM_PAGE0_ADDR;
}


/* 擦除最后一页 */
static void FLASH_ErasePage(void)
{
    FLASH_Unlock();
    FLASH_WaitBusy();

    FLASH->CR &= ~FLASH_CR_PNB;
    FLASH->CR |= FLASH_CR_PER;

    FLASH->CR |= (((EEPROM_PAGE_ADDR - 0x08000000) / EEPROM_PAGE_SIZE) << FLASH_CR_PNB_Pos);
	
    FLASH->CR |= FLASH_CR_STRT;

    FLASH_WaitBusy();
    FLASH->CR &= ~FLASH_CR_PER;

    FLASH_Lock();
}

/* 双字写入 */
static void FLASH_ProgramDoubleWord(uint32_t addr, uint64_t data)
{
	   FLASH_Unlock();
	   FLASH_WaitBusy();
	
	   FLASH->CR |= FLASH_CR_PG;
	   *(volatile uint64_t*)addr = data;
	   FLASH_WaitBusy();
	   FLASH->CR &= ~FLASH_CR_PG;
	
	   FLASH_Lock();

}

/* 计算BCC */
static uint8_t EEPROM_CalcBCC(uint16_t virtAddr, uint32_t data)
{
    return (uint8_t)(virtAddr & 0xFF) ^
           (uint8_t)(virtAddr >> 8) ^
           (uint8_t)(data & 0xFF) ^
           (uint8_t)(data >> 8) ^
           (uint8_t)(data >> 16) ^
           (uint8_t)(data >> 24);
}

/* 读取虚拟地址对应的最新值 */
uint32_t EEPROM_Read(uint16_t virtAddr)
{
  #if 1
    uint32_t addr = EEPROM_PAGE_ADDR;
    uint32_t found = 0xFFFFFFFF;
   #if 1
    while (addr < EEPROM_PAGE1_ADDR + EEPROM_PAGE_SIZE) {
        EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
        if (rec.virtAddr == 0xFFFF) break; // 空记录
        if (rec.virtAddr == virtAddr) {
            found = rec.data; // 最新值
        }
        addr += RECORD_SIZE;
    }
    return found;
	#else 

	 while (addr < EEPROM_PAGE1_ADDR + EEPROM_PAGE_SIZE) {
		   EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
		   if (rec.virtAddr == 0xFFFF) break;
		   if (rec.virtAddr == virtAddr) {
			   if (rec.bcc == EEPROM_CalcBCC(rec.virtAddr, rec.data)) {
				   found = rec.data;
			   }
		   }
		   addr += RECORD_SIZE;
	   }
	   return found;



	#endif 
 #else

	uint32_t activePage = EEPROM_GetActivePage();
	uint32_t addr = activePage + sizeof(uint16_t);
	uint32_t found = 0xFFFFFFFF;
	
	   while (addr < activePage + EEPROM_PAGE_SIZE) {
		   EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
		   if (rec.virtAddr == 0xFFFF) break;
		   if (rec.virtAddr == virtAddr) {
			   if (rec.bcc == EEPROM_CalcBCC(rec.virtAddr, rec.data)) {
				   found = rec.data;
			   }
		   }
		   addr += RECORD_SIZE;
	   }
	   return found;

 #endif 
}

/* 写入（追加模式 + 虚拟地址） */
void EEPROM_Write(uint16_t virtAddr, uint32_t data)
{
  #if  0
    uint32_t addr = EEPROM_PAGE_ADDR;

    // 找到空白位置
    while (addr < EEPROM_PAGE1_ADDR + EEPROM_PAGE_SIZE) {
        EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
        if (rec.virtAddr == 0xFFFF) {
            EEPROM_Record_t newRec;
            newRec.virtAddr = virtAddr;
            newRec.data = data;
            newRec.bcc = EEPROM_CalcBCC(virtAddr, data);
            newRec.reserved = 0xFF; // 保留字节

            uint64_t packed;
            memcpy(&packed, &newRec, sizeof(newRec));
            FLASH_ProgramDoubleWord(addr, packed);
            return;
        }
        addr += RECORD_SIZE;
    }

    // 页满 → 擦除并重写最新值
    // 这里可以遍历所有虚拟地址并重写最新值
   //  FLASH_ErasePage();
   // EEPROM_Write(virtAddr, data);
}

#endif 
#if 0

	uint32_t addr = EEPROM_PAGE_ADDR;
	
		// 1. 在当前页找空位
		while (addr < EEPROM_PAGE_ADDR + EEPROM_PAGE_SIZE) {
			EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
			if (rec.virtAddr == 0xFFFF) {
				// 空位 → 写入新记录
				EEPROM_Record_t newRec;
				newRec.virtAddr = virtAddr;
				newRec.data = data;
				newRec.bcc = 0xFF; //EEPROM_CalcBCC(virtAddr, data);
				newRec.reserved = 0xFF;
	
				uint64_t packed;
				memcpy(&packed, &newRec, sizeof(newRec));
				FLASH_ProgramDoubleWord(addr, packed);
				return;
			}
			addr += RECORD_SIZE;
		}
	
		// 2. 页满 → 擦除整页，只写本次数据
		FLASH_ErasePage();
	
		addr = EEPROM_PAGE_ADDR;
		EEPROM_Record_t newRec;
		newRec.virtAddr = virtAddr;
		newRec.data = data;
		newRec.bcc = EEPROM_CalcBCC(virtAddr, data);
		
		newRec.reserved = 0xFF;
	
		uint64_t packed;
		memcpy(&packed, &newRec, sizeof(newRec));
		FLASH_ProgramDoubleWord(addr, packed);

#endif 

}


#if 0
    uint8_t page_num;
  	uint32_t activePage = EEPROM_GetActivePage();
    uint32_t addr = activePage + sizeof(uint16_t); // 跳过状态字

    // 找空位
    while (addr < activePage + EEPROM_PAGE_SIZE) {
        EEPROM_Record_t rec = *(EEPROM_Record_t*)addr;
        if (rec.virtAddr == 0xFFFF) {
            // 写入新记录
            EEPROM_Record_t newRec;
            newRec.virtAddr = virtAddr;
            newRec.data = data;
            newRec.bcc = EEPROM_CalcBCC(virtAddr, data);
            newRec.reserved = 0xFF;

            uint64_t packed;
            memcpy(&packed, &newRec, sizeof(newRec));
            FLASH_ProgramDoubleWord(addr, packed);
            return;
        }
        addr += RECORD_SIZE;
    }

    // 页满 → 切换到另一页，只写新数据
    uint32_t newPage = (activePage == EEPROM_PAGE0_ADDR) ? EEPROM_PAGE1_ADDR : EEPROM_PAGE0_ADDR;

    FLASH_ErasePage(newPage);
    FLASH_ProgramDoubleWord(newPage, PAGE_STATUS_VALID); // 页头状态

    // 写入本次数据
    addr = newPage + sizeof(uint16_t);
    EEPROM_Record_t newRec;
    newRec.virtAddr = virtAddr;
    newRec.data = data;
    newRec.bcc = EEPROM_CalcBCC(virtAddr, data);
    newRec.reserved = 0xFF;

    uint64_t packed;
    memcpy(&packed, &newRec, sizeof(newRec));
    FLASH_ProgramDoubleWord(addr, packed);

    // 擦除旧页
    if(activePage==EEPROM_PAGE1_ADDR)page_num =1;
	else page_num = 0;
    FLASH_ErasePage(page_num);


#endif

uint32_t read_flash_value(void)
{
	

	return EEPROM_Read(0x0001);

}
