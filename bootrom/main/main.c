/*
* Copyright (c) 2026 KonataBSD. All rights reserved
*
* written by KonataBSD
*
* This code is licensed under the GNU GPL v2 or later
*/


// main.c

/* does heavy initialization
* still under development
* may have bugs
* I mostly focused on building right now than fixing bugs/code and potential vulnerability.
*/

#include <stdint.h>
#include <utils.h>
#include <uart.h>
#include <efuse.h>
#include <crf.h>
#include <usb.h>
#include <apu.h>
#include <pmu.h>
#include <sd.h>
#include <spi.h>
#include <i2c.h>
#include <boot_mode.h>
#include <ams.h>
#include <ocm.h>
#include "sdhci.h"
#include <stdarg.h>
#include <partition.h>
#include <mio.h>
#include <wdt.h>
#include <qspi.h>

#define TIMEOUT_MAX 100000
#define STACK_BASE ((uint32_t*)0xFFFFFFFEUL)
#define STACK_END ((uint32_t*)0xFFFFBFFEUL)
int timeout = 0;

struct sdhci host;
struct xmpu_ocm ocm_x;
struct partition_hdr h;

#define EXPCTD_PATTERN 0x5AA55AA5UL
#define BASE 0xFFFFFAFC
#define SIZE 256

static int wait_bit_set_32(volatile uint32_t* addr, unsigned long bit) {

if(bit >= 32) {

return -1;

}

timeout = 0;
uint32_t mask = BIT(bit);

while(!(*addr & mask)) {
asm volatile("nop\n" ::: "memory");

if(++timeout >= TIMEOUT_MAX) {
return -2;
}

}

return 0;

}


static int wait_bit_clear_32(volatile uint32_t* addr, unsigned long bit) {

if(bit >= 32) {

return -1;

}

timeout = 0;

uint32_t mask = BIT(bit);

while((*addr & mask) != 0) {
asm volatile("nop\n" ::: "memory");

if(++timeout >= TIMEOUT_MAX) {
return -2;
}

}

return 0;

}

static int wait_bit_clear_16(volatile uint16_t* addr, unsigned long bit) {

if(bit >= 16) {

return -1;

}

timeout = 0;
uint16_t mask = BIT(bit);

while((*addr & mask) != 0) {
asm volatile("nop\n" ::: "memory");


if(++timeout >= TIMEOUT_MAX) {
return -2;
}

}

return 0;

}


void main_boot(void) {

volatile uint32_t val = 0;
volatile uint16_t val2 = 0;

val = BIT(0) | BIT(1);
writel(&WDT_MODE, val);
writel(&WDT_RESTART, 0x1999);

uint32_t *st_canary = STACK_END;

writel(&st_canary, 0xAA55AA55);

int a = 0xABCD;
int b = 0xEF12;

if((a + b) != 0x19ADF) {

wfi();

}

volatile uint32_t* sram = (volatile uint32_t*)BASE;

for(size_t i = 0; i < SIZE; i++) {
sram[i] = EXPCTD_PATTERN;

}

for(size_t i = 0; i < SIZE; i++) {

if(sram[i] != EXPCTD_PATTERN) while(1);

}


writeb(&RESET_CTRL, BIT(0));

val2 = 0xDF0D;

writew(&EFUSE_WR_LOCK, val2);
if((wait_bit_clear_16(&EFUSE_WR_LOCK, 0)) != 0) wfi();

writeb(&EFUSE_CFG, 0x2);

xmpu_ocm_init(&ocm_x, XMPU_OCM_ADDR, 0xffff0000, 0xffffbfff, val);

writew(&EFUSE_CFG, BIT(1));
// BIT_CLEAR(EFUSE_PGM_ADDR, 10);
// BIT_CLEAR(EFUSE_PGM_ADDR, 11);

writew(&EFUSE_SECCTRL, BIT(6));

val = (3 << 20) | (3 << 24);
writel(&IOPLL_CTRL, val);
writel(&RPLL_CTRL, val);

writel(&IOPLL_CTRL, BIT(3));
writel(&RPLL_CTRL, BIT(3));

writel(&IOPLL_CTRL, BIT(0));
writel(&RPLL_CTRL, BIT(0));

if((wait_bit_clear_32(&CRL_PLL_STS, 0)) != 0) wfi();

BIT_CLEAR(IOPLL_CTRL, 3);
BIT_CLEAR(RPLL_CTRL, 3);

if((wait_bit_clear_32(&CRL_PLL_STS, 1)) != 0) wfi();

val = (3 << 20) | (3 << 24);
writel(&DPLL_CTRL, val);
writel(&DPLL_CTRL, BIT(3));
writel(&DPLL_CTRL, BIT(0));

wait_bit_set_32(&PLL_STS, 1);

BIT_CLEAR(DPLL_CTRL, 3);

if((wait_bit_set_32(&PLL_STS, 4)) != 0) wfi();

val = (3 << 20) | (3 << 24);
writel(&VPLL_CTRL, val);
writel(&VPLL_CTRL, BIT(3));
writel(&VPLL_CTRL, BIT(0));

if((wait_bit_set_32(&PLL_STS, 2)) != 0) wfi();
BIT_CLEAR(VPLL_CTRL, 3);
if((wait_bit_set_32(&PLL_STS, 5)) != 0) wfi();


val = (3 << 20) | (3 << 24);
writel(&APLL_CTRL, val);
writel(&APLL_CTRL, BIT(3));
writel(&APLL_CTRL, BIT(0));


if((wait_bit_set_32(&PLL_STS, 0)) != 0) wfi();
BIT_CLEAR(APLL_CTRL, 3);

if((wait_bit_set_32(&PLL_STS, 3)) != 0) wfi();

writel(&SPI_REF_CTRL, BIT(24));
writel(&USB0_REF_CTRL, BIT(24));
val = BIT(1) | BIT(24) | BIT(28);
// writel(&QSPI_REF_CTRL, cfg);
writel(&I2C1_REF_CTRL, BIT(24));

val = (6 << 5);
writel(&MIO_18, val);
writel(&MIO_19, val);

writel(&UART_REF_CTRL, BIT(24));

val = BIT(3) | BIT(5);
writel(&UART_CONTROL, val);

val = BIT(12) | BIT(13);
writel(&UART_MODE, val);

val = BIT(2) | (3 << 3) | BIT(5);
writel(&UART_MODE, val);

BIT_CLEAR(UART_MODE, 6);
BIT_CLEAR(UART_MODE, 7);

int val3 = 124;
writel(&UART_BAUD_RT_GEN, val3);
val3 = 6;
writel(&UART_BAUD_RT_DIV, val3);

val = BIT(0) | BIT(1);
writel(&UART_CONTROL, val);

val = BIT(2) | BIT(4);

writel(&UART_CONTROL, val);

printf("bootrom\n");
printf("uart test2\n");

uint64_t perhi_base = MRS(S3_1_C15_C3_0);
printf("mmio start: ");
puthex(perhi_base);
putc('\n');

writel(&APU_PWRCTL, BIT(16));


if((wait_bit_clear_32(&APU_PWRSTS, 16)) != 0) nop();

printf("L2 hardware flush done\n");

val = readl(&AMS_VCC_PSBATT);
uint16_t voltage = val >> 6;
puthex(val);
puthex(voltage);
putc('\n');

val = (6 << 2);

writel(&MIO_16, val);
writel(&MIO_17, val);

val2 = BIT(6) | BIT(3);
writew(&I2C1_CTRL, val2);
asm volatile("isb\n" ::: "memory");
printf("I2C1 reset\n");

val2 = 0xFFFF;
writew(&I2C1_INTSTS, val2);

if((uint32_t*)st_canary != 0xAA55AA55) {
printf("stack overflow\n");
while(1);
}

spi_init();
usb_init();
sdhci_init(&host, SDHCI0_BASE);

writel(&RST_LPD_IOU2, BIT(20));

partition_map_init(&h);
char *test = "testpartitionmapentry1";
create_partition(test, 2, 2, 1, 0x4000, 0x2000);

val2 = readw(&BOOT_MODE_USER);

uint16_t boot_mode = (val2 >> 12) & 0xF;

// old code, still thinking plans at this section
// do not modify

switch(boot_mode) {

case SPI_BOOT:
printf("booting via SPI\n");
// spi_boot();
break;

case SD_BOOT:
printf("Booting via SD card\n");
// sd_boot);
break;

case SATA_BOOT:
printf("booting via SATA ahci\n");
// sata_boot);
break;

case USB_BOOT:
printf("booting via USB\n");
// usb_boot();
break;

default:
printf("Unknown Boot mode: ");
puthex(boot_mode);
printf("\n");
printf("Defaulting to SPI\n");
spi_init();
while(1) wfi();

}

}
