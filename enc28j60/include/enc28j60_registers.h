#pragma once

// ENC28J60 Control Registers
// Control register definitions are a combination of address,
// bank number, and Ethernet/MAC/PHY indicator bits.
// - Register address        (bits 0-4)
// - Bank number        (bits 5-6)
// - MAC/PHY indicator        (bit 7)
constexpr uint8_t ADDR_MASK = 0x1F;
constexpr uint8_t BANK_MASK = 0x60;
constexpr uint8_t SPRD_MASK = 0x80;
// All-bank registers
constexpr uint8_t EIE = 0x1B;
constexpr uint8_t EIR = 0x1C;
constexpr uint8_t ESTAT = 0x1D;
constexpr uint8_t ECON2 = 0x1E;
constexpr uint8_t ECON1 = 0x1F;
// Bank 0 registers
constexpr uint8_t ERDPT = (0x00 | 0x00);
constexpr uint8_t EWRPT = (0x02 | 0x00);
constexpr uint8_t ETXST = (0x04 | 0x00);
constexpr uint8_t ETXND = (0x06 | 0x00);
constexpr uint8_t ERXST = (0x08 | 0x00);
constexpr uint8_t ERXND = (0x0A | 0x00);
constexpr uint8_t ERXRDPT = (0x0C | 0x00);
constexpr uint8_t EDMAST = (0x10 | 0x00);
constexpr uint8_t EDMAND = (0x12 | 0x00);
constexpr uint8_t EDMACS = (0x16 | 0x00);
// Bank 1 registers
constexpr uint8_t EHT0 = (0x00 | 0x20);
constexpr uint8_t EHT1 = (0x01 | 0x20);
constexpr uint8_t EHT2 = (0x02 | 0x20);
constexpr uint8_t EHT3 = (0x03 | 0x20);
constexpr uint8_t EHT4 = (0x04 | 0x20);
constexpr uint8_t EHT5 = (0x05 | 0x20);
constexpr uint8_t EHT6 = (0x06 | 0x20);
constexpr uint8_t EHT7 = (0x07 | 0x20);
constexpr uint8_t EPMM0 = (0x08 | 0x20);
constexpr uint8_t EPMM1 = (0x09 | 0x20);
constexpr uint8_t EPMM2 = (0x0A | 0x20);
constexpr uint8_t EPMM3 = (0x0B | 0x20);
constexpr uint8_t EPMM4 = (0x0C | 0x20);
constexpr uint8_t EPMM5 = (0x0D | 0x20);
constexpr uint8_t EPMM6 = (0x0E | 0x20);
constexpr uint8_t EPMM7 = (0x0F | 0x20);
constexpr uint8_t EPMCS = (0x10 | 0x20);
constexpr uint8_t EPMO = (0x14 | 0x20);
constexpr uint8_t EWOLIE = (0x16 | 0x20);
constexpr uint8_t EWOLIR = (0x17 | 0x20);
constexpr uint8_t ERXFCON = (0x18 | 0x20);
constexpr uint8_t EPKTCNT = (0x19 | 0x20);
// Bank 2 registers
constexpr uint8_t MACON1 = (0x00 | 0x40 | 0x80);
constexpr uint8_t MACON3 = (0x02 | 0x40 | 0x80);
constexpr uint8_t MACON4 = (0x03 | 0x40 | 0x80);
constexpr uint8_t MABBIPG = (0x04 | 0x40 | 0x80);
constexpr uint8_t MAIPG = (0x06 | 0x40 | 0x80);
constexpr uint8_t MACLCON1 = (0x08 | 0x40 | 0x80);
constexpr uint8_t MACLCON2 = (0x09 | 0x40 | 0x80);
constexpr uint8_t MAMXFL = (0x0A | 0x40 | 0x80);
constexpr uint8_t MAPHSUP = (0x0D | 0x40 | 0x80);
constexpr uint8_t MICON = (0x11 | 0x40 | 0x80);
constexpr uint8_t MICMD = (0x12 | 0x40 | 0x80);
constexpr uint8_t MIREGADR = (0x14 | 0x40 | 0x80);
constexpr uint8_t MIWR = (0x16 | 0x40 | 0x80);
constexpr uint8_t MIRD = (0x18 | 0x40 | 0x80);
constexpr uint8_t MIRDL = (0x18 | 0x40 | 0x80);
constexpr uint8_t MIRDH = (0x19 | 0x40 | 0x80);
// Bank 3 registers
constexpr uint8_t MAADR1 = (0x00 | 0x60 | 0x80);
constexpr uint8_t MAADR0 = (0x01 | 0x60 | 0x80);
constexpr uint8_t MAADR3 = (0x02 | 0x60 | 0x80);
constexpr uint8_t MAADR2 = (0x03 | 0x60 | 0x80);
constexpr uint8_t MAADR5 = (0x04 | 0x60 | 0x80);
constexpr uint8_t MAADR4 = (0x05 | 0x60 | 0x80);
constexpr uint8_t EBSTSD = (0x06 | 0x60);
constexpr uint8_t EBSTCON = (0x07 | 0x60);
constexpr uint8_t EBSTCS = (0x08 | 0x60);
constexpr uint8_t MISTAT = (0x0A | 0x60 | 0x80);
constexpr uint8_t EREVID = (0x12 | 0x60);
constexpr uint8_t ECOCON = (0x15 | 0x60);
constexpr uint8_t EFLOCON = (0x17 | 0x60);
constexpr uint8_t EPAUS = (0x18 | 0x60);

// ENC28J60 ERXFCON Register Bit Definitions
constexpr uint8_t ERXFCON_UCEN = 0x80;
constexpr uint8_t ERXFCON_ANDOR = 0x40;
constexpr uint8_t ERXFCON_CRCEN = 0x20;
constexpr uint8_t ERXFCON_PMEN = 0x10;
constexpr uint8_t ERXFCON_MPEN = 0x08;
constexpr uint8_t ERXFCON_HTEN = 0x04;
constexpr uint8_t ERXFCON_MCEN = 0x02;
constexpr uint8_t ERXFCON_BCEN = 0x01;
// ENC28J60 EIE Register Bit Definitions
constexpr uint8_t EIE_INTIE = 0x80;
constexpr uint8_t EIE_PKTIE = 0x40;
constexpr uint8_t EIE_DMAIE = 0x20;
constexpr uint8_t EIE_LINKIE = 0x10;
constexpr uint8_t EIE_TXIE = 0x08;
constexpr uint8_t EIE_WOLIE = 0x04;
constexpr uint8_t EIE_TXERIE = 0x02;
constexpr uint8_t EIE_RXERIE = 0x01;
// ENC28J60 EIR Register Bit Definitions
constexpr uint8_t EIR_PKTIF = 0x40;
constexpr uint8_t EIR_DMAIF = 0x20;
constexpr uint8_t EIR_LINKIF = 0x10;
constexpr uint8_t EIR_TXIF = 0x08;
constexpr uint8_t EIR_WOLIF = 0x04;
constexpr uint8_t EIR_TXERIF = 0x02;
constexpr uint8_t EIR_RXERIF = 0x01;
// ENC28J60 ESTAT Register Bit Definitions
constexpr uint8_t ESTAT_INT = 0x80;
constexpr uint8_t ESTAT_LATECOL = 0x10;
constexpr uint8_t ESTAT_RXBUSY = 0x04;
constexpr uint8_t ESTAT_TXABRT = 0x02;
constexpr uint8_t ESTAT_CLKRDY = 0x01;
// ENC28J60 ECON2 Register Bit Definitions
constexpr uint8_t ECON2_AUTOINC = 0x80;
constexpr uint8_t ECON2_PKTDEC = 0x40;
constexpr uint8_t ECON2_PWRSV = 0x20;
constexpr uint8_t ECON2_VRPS = 0x08;
// ENC28J60 ECON1 Register Bit Definitions
constexpr uint8_t ECON1_TXRST = 0x80;
constexpr uint8_t ECON1_RXRST = 0x40;
constexpr uint8_t ECON1_DMAST = 0x20;
constexpr uint8_t ECON1_CSUMEN = 0x10;
constexpr uint8_t ECON1_TXRTS = 0x08;
constexpr uint8_t ECON1_RXEN = 0x04;
constexpr uint8_t ECON1_BSEL1 = 0x02;
constexpr uint8_t ECON1_BSEL0 = 0x01;
// ENC28J60 MACON1 Register Bit Definitions
constexpr uint8_t MACON1_LOOPBK = 0x10;
constexpr uint8_t MACON1_TXPAUS = 0x08;
constexpr uint8_t MACON1_RXPAUS = 0x04;
constexpr uint8_t MACON1_PASSALL = 0x02;
constexpr uint8_t MACON1_MARXEN = 0x01;
// ENC28J60 MACON3 Register Bit Definitions
constexpr uint8_t MACON3_PADCFG2 = 0x80;
constexpr uint8_t MACON3_PADCFG1 = 0x40;
constexpr uint8_t MACON3_PADCFG0 = 0x20;
constexpr uint8_t MACON3_TXCRCEN = 0x10;
constexpr uint8_t MACON3_PHDRLEN = 0x08;
constexpr uint8_t MACON3_HFRMLEN = 0x04;
constexpr uint8_t MACON3_FRMLNEN = 0x02;
constexpr uint8_t MACON3_FULDPX = 0x01;
// ENC28J60 MICMD Register Bit Definitions
constexpr uint8_t MICMD_MIISCAN = 0x02;
constexpr uint8_t MICMD_MIIRD = 0x01;
// ENC28J60 MISTAT Register Bit Definitions
constexpr uint8_t MISTAT_NVALID = 0x04;
constexpr uint8_t MISTAT_SCAN = 0x02;
constexpr uint8_t MISTAT_BUSY = 0x01;

// ENC28J60 EBSTCON Register Bit Definitions
constexpr uint8_t EBSTCON_PSV2 = 0x80;
constexpr uint8_t EBSTCON_PSV1 = 0x40;
constexpr uint8_t EBSTCON_PSV0 = 0x20;
constexpr uint8_t EBSTCON_PSEL = 0x10;
constexpr uint8_t EBSTCON_TMSEL1 = 0x08;
constexpr uint8_t EBSTCON_TMSEL0 = 0x04;
constexpr uint8_t EBSTCON_TME = 0x02;
constexpr uint8_t EBSTCON_BISTST = 0x01;

// PHY registers
constexpr uint8_t PHCON1 = 0x00;
constexpr uint8_t PHSTAT1 = 0x01;
constexpr uint8_t PHHID1 = 0x02;
constexpr uint8_t PHHID2 = 0x03;
constexpr uint8_t PHCON2 = 0x10;
constexpr uint8_t PHSTAT2 = 0x11;
constexpr uint8_t PHIE = 0x12;
constexpr uint8_t PHIR = 0x13;
constexpr uint8_t PHLCON = 0x14;

// ENC28J60 PHY PHCON1 Register Bit Definitions
constexpr uint16_t PHCON1_PRST = 0x8000;
constexpr uint16_t PHCON1_PLOOPBK = 0x4000;
constexpr uint16_t PHCON1_PPWRSV = 0x0800;
constexpr uint16_t PHCON1_PDPXMD = 0x0100;
// ENC28J60 PHY PHSTAT1 Register Bit Definitions
constexpr uint16_t PHSTAT1_PFDPX = 0x1000;
constexpr uint16_t PHSTAT1_PHDPX = 0x0800;
constexpr uint16_t PHSTAT1_LLSTAT = 0x0004;
constexpr uint16_t PHSTAT1_JBSTAT = 0x0002;
constexpr uint16_t PHSTAT2_LSTAT = 0x0400;
// ENC28J60 PHY PHCON2 Register Bit Definitions
constexpr uint16_t PHCON2_FRCLINK = 0x4000;
constexpr uint16_t PHCON2_TXDIS = 0x2000;
constexpr uint16_t PHCON2_JABBER = 0x0400;
constexpr uint16_t PHCON2_HDLDIS = 0x0100;

// ENC28J60 Packet Control Byte Bit Definitions
constexpr uint16_t PKTCTRL_PHUGEEN = 0x08;
constexpr uint16_t PKTCTRL_PPADEN = 0x04;
constexpr uint16_t PKTCTRL_PCRCEN = 0x02;
constexpr uint16_t PKTCTRL_POVERRIDE = 0x01;

// SPI operation codes
constexpr uint8_t ENC28J60_READ_CTRL_REG = 0x00;
constexpr uint8_t ENC28J60_READ_BUF_MEM = 0x3A;
constexpr uint8_t ENC28J60_WRITE_CTRL_REG = 0x40;
constexpr uint8_t ENC28J60_WRITE_BUF_MEM = 0x7A;
constexpr uint8_t ENC28J60_BIT_FIELD_SET = 0x80;
constexpr uint8_t ENC28J60_BIT_FIELD_CLR = 0xA0;
constexpr uint8_t ENC28J60_SOFT_RESET = 0xFF;
constexpr uint8_t ENC_ADDR_MASK = 0x1f;
