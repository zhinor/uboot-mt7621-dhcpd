/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * SPI flash internal definitions
 *
 * Copyright (C) 2008 Atmel Corporation
 * Copyright (C) 2013 Jagannadha Sutradharudu Teki, Xilinx Inc.
 */

#ifndef _SF_INTERNAL_H_
#define _SF_INTERNAL_H_

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/bitops.h>

/* SFDP (Serial Flash Discoverable Parameters) definitions */
#define SFDP_SIGNATURE		0x50444653U	/* "SFDP" */
#define SFDP_JESD216_MAJOR	1
#define SFDP_JESD216_MINOR	0
#define SFDP_JESD216A_MINOR	5
#define SFDP_JESD216B_MINOR	6

#define SFDP_BFPT_ID		0xff00	/* Basic Flash Parameter Table */
#define SFDP_SECTOR_MAP_ID	0xff81	/* Sector Map Table */
#define SFDP_SST_ID			0x01bf	/* Manufacturer specific Table */
#define SFDP_PROFILE1_ID	0xff05	/* xSPI Profile 1.0 Table */
#define SFDP_SCCR_MAP_ID	0xff87	/* Status, Control and Configuration Register Map */

struct sfdp_parameter_header {
	u8		id_lsb;
	u8		minor;
	u8		major;
	u8		length; /* in double words */
	u8		parameter_table_pointer[3]; /* byte address */
	u8		id_msb;
};

#define SFDP_PARAM_HEADER_ID(p)	(((p)->id_msb << 8) | (p)->id_lsb)
#define SFDP_PARAM_HEADER_PTP(p) \
	(((p)->parameter_table_pointer[2] << 16) | \
	 ((p)->parameter_table_pointer[1] <<  8) | \
	 ((p)->parameter_table_pointer[0] <<  0))

struct sfdp_header {
	u32		signature; /* 0x50444653U <=> "SFDP" */
	u8		minor;
	u8		major;
	u8		nph; /* 0-base number of parameter headers */
	u8		unused;

	/* Basic Flash Parameter Table. */
	struct sfdp_parameter_header	bfpt_header;
};

/* Basic Flash Parameter Table */
#define BFPT_DWORD(i)		((i) - 1)
#define BFPT_DWORD_MAX		20
#define BFPT_DWORD_MAX_JESD216	9
#define BFPT_DWORD_MAX_JESD216B	16

/* 1st DWORD. */
#define BFPT_DWORD1_FAST_READ_1_1_2	BIT(16)
#define BFPT_DWORD1_ADDRESS_BYTES_MASK	GENMASK(18, 17)
#define BFPT_DWORD1_ADDRESS_BYTES_3_ONLY	(0x0UL << 17)
#define BFPT_DWORD1_ADDRESS_BYTES_3_OR_4	(0x1UL << 17)
#define BFPT_DWORD1_ADDRESS_BYTES_4_ONLY	(0x2UL << 17)
#define BFPT_DWORD1_DTR				BIT(19)
#define BFPT_DWORD1_FAST_READ_1_2_2	BIT(20)
#define BFPT_DWORD1_FAST_READ_1_4_4	BIT(21)
#define BFPT_DWORD1_FAST_READ_1_1_4	BIT(22)

/* 5th DWORD. */
#define BFPT_DWORD5_FAST_READ_2_2_2	BIT(0)
#define BFPT_DWORD5_FAST_READ_4_4_4	BIT(4)

/* 11th DWORD. */
#define BFPT_DWORD11_PAGE_SIZE_SHIFT	4
#define BFPT_DWORD11_PAGE_SIZE_MASK	GENMASK(7, 4)

/* 15th DWORD - Quad Enable Requirements */
#define BFPT_DWORD15_QER_MASK		GENMASK(22, 20)
#define BFPT_DWORD15_QER_NONE		(0x0UL << 20) /* Micron */
#define BFPT_DWORD15_QER_SR2_BIT1_BUGGY	(0x1UL << 20)
#define BFPT_DWORD15_QER_SR1_BIT6	(0x2UL << 20) /* Macronix */
#define BFPT_DWORD15_QER_SR2_BIT7	(0x3UL << 20)
#define BFPT_DWORD15_QER_SR2_BIT1_NO_RD	(0x4UL << 20)
#define BFPT_DWORD15_QER_SR2_BIT1	(0x5UL << 20) /* Spansion */

#define BFPT_DWORD16_SOFT_RST		BIT(12)
#define BFPT_DWORD16_EX4B_PWRCYC	BIT(21)

#define BFPT_DWORD18_CMD_EXT_MASK	GENMASK(30, 29)
#define BFPT_DWORD18_CMD_EXT_REP	(0x0UL << 29) /* Repeat */
#define BFPT_DWORD18_CMD_EXT_INV	(0x1UL << 29) /* Invert */
#define BFPT_DWORD18_CMD_EXT_RES	(0x2UL << 29) /* Reserved */
#define BFPT_DWORD18_CMD_EXT_16B	(0x3UL << 29) /* 16-bit opcode */

struct sfdp_bfpt {
	u32	dwords[BFPT_DWORD_MAX];
};

/* Dual SPI flash memories - see SPI_COMM_DUAL_... */
enum spi_dual_flash {
	SF_SINGLE_FLASH	= 0,
	SF_DUAL_STACKED_FLASH	= BIT(0),
	SF_DUAL_PARALLEL_FLASH	= BIT(1),
};

enum spi_nor_option_flags {
	SNOR_F_SST_WR		= BIT(0),
	SNOR_F_USE_FSR		= BIT(1),
	SNOR_F_NO_ERASE		= BIT(2),
	SNOR_F_USE_UPAGE	= BIT(3),
	SNOR_F_USE_CLSR		= BIT(4),
	SNOR_F_4B_ADDR		= BIT(5),
};

#define SPI_FLASH_3B_ADDR_LEN		3
#define SPI_FLASH_CMD_LEN		(1 + SPI_FLASH_3B_ADDR_LEN)
#define SPI_FLASH_16MB_BOUN		0x1000000

/* CFI Manufacture ID's */
#define SPI_FLASH_CFI_MFR_SPANSION	0x01
#define SPI_FLASH_CFI_MFR_STMICRO	0x20
#define SPI_FLASH_CFI_MFR_MACRONIX	0xc2
#define SPI_FLASH_CFI_MFR_SST		0xbf
#define SPI_FLASH_CFI_MFR_WINBOND	0xef
#define SPI_FLASH_CFI_MFR_ATMEL		0x1f

/* Erase commands */
#define CMD_ERASE_4K			0x20
#define CMD_ERASE_CHIP			0xc7
#define CMD_ERASE_64K			0xd8
#define CMD_ERASE_4K_4B			0x21
#define CMD_ERASE_64K_4B		0xdc

/* Write commands */
#define CMD_WRITE_STATUS		0x01
#define CMD_PAGE_PROGRAM		0x02
#define CMD_WRITE_DISABLE		0x04
#define CMD_WRITE_ENABLE		0x06
#define CMD_QUAD_PAGE_PROGRAM		0x32
#define CMD_PAGE_PROGRAM_4B		0x12
#define CMD_QUAD_PAGE_PROGRAM_4B	0x34

/* Read commands */
#define CMD_READ_ARRAY_SLOW		0x03
#define CMD_READ_ARRAY_FAST		0x0b
#define CMD_READ_DUAL_OUTPUT_FAST	0x3b
#define CMD_READ_DUAL_IO_FAST		0xbb
#define CMD_READ_QUAD_OUTPUT_FAST	0x6b
#define CMD_READ_QUAD_IO_FAST		0xeb
#define CMD_READ_ARRAY_SLOW_4B		0x13
#define CMD_READ_ARRAY_FAST_4B		0x0c
#define CMD_READ_DUAL_OUTPUT_FAST_4B	0x3c
#define CMD_READ_QUAD_OUTPUT_FAST_4B	0x6c
#define CMD_READ_ID			0x9f
#define CMD_READ_STATUS			0x05
#define CMD_READ_STATUS1		0x35
#define CMD_READ_CONFIG			0x35
#define CMD_FLAG_STATUS			0x70
#define CMD_CLEAR_STATUS		0x30
#define CMD_READ_SFDP			0x5a	/* SFDP table read command */

/* Bank addr access commands */
#ifdef CONFIG_SPI_FLASH_BAR
# define CMD_BANKADDR_BRWR		0x17
# define CMD_BANKADDR_BRRD		0x16
# define CMD_EXTNADDR_WREAR		0xC5
# define CMD_EXTNADDR_RDEAR		0xC8
#endif

/* Common status */
#define STATUS_WIP			BIT(0)
#define STATUS_QEB_WINSPAN		BIT(1)
#define STATUS_QEB_MXIC			BIT(6)
#define STATUS_PEC			BIT(7)
#define STATUS_E_ERR			BIT(5)
#define STATUS_P_ERR			BIT(6)
#define SR_BP0				BIT(2)  /* Block protect 0 */
#define SR_BP1				BIT(3)  /* Block protect 1 */
#define SR_BP2				BIT(4)  /* Block protect 2 */

/* Flash timeout values */
#define SPI_FLASH_PROG_TIMEOUT		(2 * CONFIG_SYS_HZ)
#define SPI_FLASH_PAGE_ERASE_TIMEOUT	(5 * CONFIG_SYS_HZ)
#define SPI_FLASH_SECTOR_ERASE_TIMEOUT	(10 * CONFIG_SYS_HZ)

/* SST specific */
#ifdef CONFIG_SPI_FLASH_SST
#define SST26_CMD_READ_BPR		0x72
#define SST26_CMD_WRITE_BPR		0x42

#define SST26_BPR_8K_NUM		4
#define SST26_MAX_BPR_REG_LEN		(18 + 1)
#define SST26_BOUND_REG_SIZE		((32 + SST26_BPR_8K_NUM * 8) * SZ_1K)

enum lock_ctl {
	SST26_CTL_LOCK,
	SST26_CTL_UNLOCK,
	SST26_CTL_CHECK
};

# define CMD_SST_BP		0x02    /* Byte Program */
# define CMD_SST_AAI_WP		0xAD	/* Auto Address Incr Word Program */

int sst_write_wp(struct spi_flash *flash, u32 offset, size_t len,
		const void *buf);
int sst_write_bp(struct spi_flash *flash, u32 offset, size_t len,
		const void *buf);
#endif

#define JEDEC_MFR(info)		((info)->id[0])
#define JEDEC_ID(info)		(((info)->id[1]) << 8 | ((info)->id[2]))
#define JEDEC_EXT(info)		(((info)->id[3]) << 8 | ((info)->id[4]))
#define SPI_FLASH_MAX_ID_LEN	6

struct spi_flash_info {
	/* Device name ([MANUFLETTER][DEVTYPE][DENSITY][EXTRAINFO]) */
	const char	*name;

	/*
	 * This array stores the ID bytes.
	 * The first three bytes are the JEDIC ID.
	 * JEDEC ID zero means "no ID" (mostly older chips).
	 */
	u8		id[SPI_FLASH_MAX_ID_LEN];
	u8		id_len;

	/*
	 * The size listed here is what works with SPINOR_OP_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	u32		sector_size;
	u32		n_sectors;

	u16		page_size;

	u16		flags;
#define SECT_4K				BIT(0)	/* CMD_ERASE_4K works uniformly */
#define SECT_4K_PMC		SECT_4K	/* PMC 4K-erase variant (same opcode here) */
#define USE_FSR				BIT(1)	/* use flag status register */
#define E_FSR			USE_FSR
#define SST_WRITE			BIT(2)	/* use SST byte/word programming */
#define SST_WR			SST_WRITE
#define WR_QPP				BIT(3)	/* use Quad Page Program */
#define SPI_NOR_QUAD_READ	BIT(4)	/* chip supports Quad Read */
#define RD_QUAD			SPI_NOR_QUAD_READ
#define SPI_NOR_DUAL_READ	BIT(5)	/* chip supports Dual Read */
#define RD_DUAL			SPI_NOR_DUAL_READ
#define RD_QUADIO			BIT(6)	/* use Quad IO Read */
#define RD_DUALIO			BIT(7)	/* use Dual IO Read */
#define RD_FULL			(RD_QUAD | RD_DUAL | RD_QUADIO | RD_DUALIO)
#define SPI_NOR_OCTAL_READ	BIT(8)	/* chip supports Octal Read */
#define SPI_NOR_OCTAL_DTR_READ	SPI_NOR_OCTAL_READ
#define SPI_NOR_HAS_LOCK	BIT(9)	/* chip supports lock/unlock via status reg */
#define SPI_NOR_HAS_TB		SPI_NOR_HAS_LOCK
#define SPI_NOR_HAS_SST26LOCK	SPI_NOR_HAS_LOCK
#define SPI_NOR_4B_OPCODES	BIT(10)	/* use dedicated 4-byte address opcodes */
#define SPI_NOR_NO_ERASE	BIT(11)	/* chip has no erase command */
#define SPI_NOR_NO_FR		BIT(12)	/* chip cannot do fast read */
#define SPI_NOR_SKIP_SFDP	BIT(13)	/* skip SFDP table parsing */
#define USE_CLSR			BIT(14)	/* use CLSR command */
#define NO_CHIP_ERASE		BIT(15)	/* chip has no chip-wide erase opcode */
};

extern const struct spi_flash_info spi_flash_ids[];

/* Send a single-byte command to the device and read the response */
int spi_flash_cmd(struct spi_slave *spi, u8 cmd, void *response, size_t len);

/*
 * Send a multi-byte command to the device and read the response. Used
 * for flash array reads, etc.
 */
int spi_flash_cmd_read(struct spi_slave *spi, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len);

/*
 * Send a multi-byte command to the device followed by (optional)
 * data. Used for programming the flash array, etc.
 */
int spi_flash_cmd_write(struct spi_slave *spi, const u8 *cmd, size_t cmd_len,
		const void *data, size_t data_len);


/* Flash erase(sectors) operation, support all possible erase commands */
int spi_flash_cmd_erase_ops(struct spi_flash *flash, u32 offset, size_t len);

/* Lock stmicro spi flash region */
int stm_lock(struct spi_flash *flash, u32 ofs, size_t len);

/* Unlock stmicro spi flash region */
int stm_unlock(struct spi_flash *flash, u32 ofs, size_t len);

/* Check if a stmicro spi flash region is completely locked */
int stm_is_locked(struct spi_flash *flash, u32 ofs, size_t len);

/* Enable writing on the SPI flash */
static inline int spi_flash_cmd_write_enable(struct spi_flash *flash)
{
	return spi_flash_cmd(flash->spi, CMD_WRITE_ENABLE, NULL, 0);
}

/* Disable writing on the SPI flash */
static inline int spi_flash_cmd_write_disable(struct spi_flash *flash)
{
	return spi_flash_cmd(flash->spi, CMD_WRITE_DISABLE, NULL, 0);
}

/*
 * Used for spi_flash write operation
 * - SPI claim
 * - spi_flash_cmd_write_enable
 * - spi_flash_cmd_write
 * - spi_flash_wait_till_ready
 * - SPI release
 */
int spi_flash_write_common(struct spi_flash *flash, const u8 *cmd,
		size_t cmd_len, const void *buf, size_t buf_len);

/*
 * Flash write operation, support all possible write commands.
 * Write the requested data out breaking it up into multiple write
 * commands as needed per the write size.
 */
int spi_flash_cmd_write_ops(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf);

/*
 * Same as spi_flash_cmd_read() except it also claims/releases the SPI
 * bus. Used as common part of the ->read() operation.
 */
int spi_flash_read_common(struct spi_flash *flash, const u8 *cmd,
		size_t cmd_len, void *data, size_t data_len);

/* Flash read operation, support all possible read commands */
int spi_flash_cmd_read_ops(struct spi_flash *flash, u32 offset,
		size_t len, void *data);

#ifdef CONFIG_SPI_FLASH_MTD
int spi_flash_mtd_register(struct spi_flash *flash);
void spi_flash_mtd_unregister(void);
#endif

/**
 * spi_flash_scan - scan the SPI FLASH
 * @flash:	the spi flash structure
 *
 * The drivers can use this fuction to scan the SPI FLASH.
 * In the scanning, it will try to get all the necessary information to
 * fill the spi_flash{}.
 *
 * Return: 0 for success, others for failure.
 */
int spi_flash_scan(struct spi_flash *flash);

#endif /* _SF_INTERNAL_H_ */
