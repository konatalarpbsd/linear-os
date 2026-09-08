#include <utils.h>
#include <spi.h>

static uint32_t cfg = 0;


size_t spi_xfer(unsigned char *tx_buf, unsigned char *rx_buf, size_t length) {

if(!length) return 0;
if(!tx_buf || !rx_buf) return 0;

size_t sent = 0;
size_t received = 0;
uint32_t tmp = 0;

while(sent < length) {

tmp = BIT(5) | BIT(3);

while((SPI_ISR & tmp) != 0);

SPI_TXD = tx_buf[sent++];
cfg = (3 << 16) | (3 << 14) | BIT(0);
writel(&SPI_CFG, cfg);

}

while(received < length) {

while((SPI_ISR & BIT(5)) != 0);

rx_buf[received++] = SPI_RXD;

}

return length;

}

unsigned char *spi_get_jedec_id(unsigned char *rx_buf, size_t tx_size_buf) {

unsigned char tx_buf[tx_size_buf];

clear_buf(tx_buf, tx_size_buf);

tx_buf[0] = 0x9F;
tx_buf[1] = 0x00;
tx_buf[2] = 0x00;
tx_buf[3] = 0x00;

spi_xfer(tx_buf, rx_buf, 8);

return 0;
}


void spi_init(void) {

writel(&SPI_REF_CTRL, BIT(24));

cfg = (3 << 16) | (3 << 14) | BIT(0);

writel(&SPI_CFG, cfg);
writel(&SPI_EN, BIT(0));

unsigned char buf[8];
spi_get_jedec_id(buf, 8);

printf("Jedec id: ");
puthex(buf[0]);
puthex(buf[1]);
puthex(buf[2]);
putc('\n');

}

