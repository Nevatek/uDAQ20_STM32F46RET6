/*
 * DRV_DAC81416.h
 *
 *  Created on: Apr 29, 2025
 *      Author: User
 */

#ifndef DRV_DAC81416_H_
#define DRV_DAC81416_H_

/************************** defines ******************************************/
/************************** typedefs *****************************************/

typedef enum
{
	DAC_FLAG_WRITE	= 0,	// Write Flag
    DAC_FLAG_READ 	= 1  	// Read Flag
} DAC81416_FLAG_TYPE;

typedef enum
{
	DAC_PIN_CS = 0,  		// Active LOW: serial data enable
    DAC_PIN_TOGGLE0, 		// Toggle pin 0: LOW -> Reg A, HIGH -> Reg B
    DAC_PIN_TOGGLE1,  		// Toggle pin 1
    DAC_PIN_TOGGLE2, 		// Toggle pin 2
    DAC_PIN_LDAC,  			// Active LOW: Synchronize DAC outputs
    DAC_PIN_RESET,  		// Active LOW: Trigger power-on-reset
    DAC_PIN_CLR        		// Active LOW: Clear DAC outputs to clear code
} DAC81416_PIN_TYPE;

typedef enum
{
    DAC_REG_NOP               = 0x00,  // NOP Register
    DAC_REG_DEVICEID          = 0x01,  // Device ID Register
    DAC_REG_STATUS            = 0x02,  // Status Register
    DAC_REG_SPICONFIG         = 0x03,  // SPI Configuration Register
    DAC_REG_GENCONFIG         = 0x04,  // General Configuration Register
    DAC_REG_BRDCONFIG         = 0x05,  // Broadcast Configuration Register
    DAC_REG_SYNCCONFIG        = 0x06,  // Sync Configuration Register
    DAC_REG_TOGGCONFIG0       = 0x07,  // DAC[15:8] Toggle Configuration Register
    DAC_REG_TOGGCONFIG1       = 0x08,  // DAC[7:0] Toggle Configuration Register
    DAC_REG_DACPWDWN          = 0x09,  // DAC Power-Down Register
    DAC_REG_DACRANGE0         = 0x0A,  // DAC[15:12] Range Register
    DAC_REG_DACRANGE1         = 0x0B,  // DAC[11:8] Range Register
    DAC_REG_DACRANGE2         = 0x0C,  // DAC[7:4] Range Register
    DAC_REG_DACRANGE3         = 0x0D,  // DAC[3:0] Range Register
    DAC_REG_TRIGGER           = 0x0E,  // Trigger Register
    DAC_REG_BRDCAST           = 0x0F,  // Broadcast Data Register
    DAC_REG_DAC0              = 0x10,  // DAC0 Data Register
    DAC_REG_DAC1              = 0x11,  // DAC1 Data Register
    DAC_REG_DAC2              = 0x12,  // DAC2 Data Register
    DAC_REG_DAC3              = 0x13,  // DAC3 Data Register
    DAC_REG_DAC4              = 0x14,  // DAC4 Data Register
    DAC_REG_DAC5              = 0x15,  // DAC5 Data Register
    DAC_REG_DAC6              = 0x16,  // DAC6 Data Register
    DAC_REG_DAC7              = 0x17,  // DAC7 Data Register
    DAC_REG_DAC8              = 0x18,  // DAC8 Data Register
    DAC_REG_DAC9              = 0x19,  // DAC9 Data Register
    DAC_REG_DAC10             = 0x1A,  // DAC10 Data Register
    DAC_REG_DAC11             = 0x1B,  // DAC11 Data Register
    DAC_REG_DAC12             = 0x1C,  // DAC12 Data Register
    DAC_REG_DAC13             = 0x1D,  // DAC13 Data Register
    DAC_REG_DAC14             = 0x1E,  // DAC14 Data Register
    DAC_REG_DAC15             = 0x1F,  // DAC15 Data Register
    DAC_REG_OFFSET0           = 0x20,  // DAC[14-15;12-13] Differential Offset Register
    DAC_REG_OFFSET1           = 0x21,  // DAC[10-11;8-9] Differential Offset Register
    DAC_REG_OFFSET2           = 0x22,  // DAC[6-7;4-5] Differential Offset Register
    DAC_REG_OFFSET3           = 0x23   // DAC[2-3;0-1] Differential Offset Register
} DAC81416_REG_MAP;


typedef struct
{
	uint16_t VERSIONID 	: 2; /* Version ID : Subject to change.*/
	uint16_t DEVICEID 	: 14; /* Device ID  : DAC81416: 29Ch */
}DAC81416_REG_DEVICEID;

typedef struct
{
	uint16_t TEMP_ALM 	:1;		/* 1 = indicates die temperature is over +140°C.
								  * A thermal alarm event forces the DAC outputs
								  * to go into power_down mode.*/
	uint16_t DAC_BUSY	:1;		/* indicates DAC registers are not ready for updates.*/
	uint16_t CRC_ALM 	:1; 	/* indicates a CRC error. */
	uint16_t 			:13; 	/* This bit is reserved.*/
}DAC81416_REG_STATUS;

typedef struct
{
	uint16_t 				: 1; /* This bit is reserved.*/
	uint16_t FSDO 			: 1; /* Fast SDO bit (half_cycle speedup).
								  * 0 = SDO updates duringS CLK rising edges.
								  * 1 = SDO updates during SCLK falling edges.*/
	uint16_t SDO_EN 		: 1; /* 1 = the SDO pin is operational.*/
	uint16_t STR_EN 		: 1; /* 1 = streaming mode operation is enabled.*/
	uint16_t CRC_EN 		: 1; /* 1 = frame error checking is enabled.*/
	uint16_t DEV_PWDWN 		: 1; /* 1 = sets the device in power_down mode
								  * 0 = sets the device in active mode*/
	uint16_t SOFTTOGGLE_EN 	: 1; /* 1 = enables soft toggle operation.*/
	uint16_t 				: 1; /* This bit is reserved.*/
	uint16_t 				: 1; /* This bit is reserved.*/
	uint16_t CRCALM_EN 		: 1; /* 1 a CRC error triggers the ALMOUT pin.*/
	uint16_t DACBUSY_EN 	: 1; /* 1 = ALMOUT pin is set between DAC output updates.
								  * this alarm resets automatically.*/
	uint16_t TEMPALM_EN 	: 1; /* 1 a thermal alarm triggers the ALMOUT pin.*/
	uint16_t 				: 4; /* This bit is reserved.*/
}DAC81416_REG_SPICONFIG;

typedef struct
{
	uint16_t DAC_0_1_DIFF_EN      : 1;  /* Enable differential mode for DAC 0/1 */
	uint16_t DAC_2_3_DIFF_EN      : 1;  /* Enable differential mode for DAC 2/3 */
	uint16_t DAC_4_5_DIFF_EN      : 1;  /* Enable differential mode for DAC 4/5 */
	uint16_t DAC_6_7_DIFF_EN      : 1;  /* Enable differential mode for DAC 6/7 */
	uint16_t DAC_8_9_DIFF_EN      : 1;  /* Enable differential mode for DAC 8/9 */
	uint16_t DAC_10_11_DIFF_EN    : 1;  /* Enable differential mode for DAC 10/11 */
	uint16_t DAC_12_13_DIFF_EN    : 1;  /* Enable differential mode for DAC 12/13 */
	uint16_t DAC_14_15_DIFF_EN    : 1;  /* Enable differential mode for DAC 14/15 */
	uint16_t                      : 6;  /* Reserved */
	uint16_t REF_PWDWN            : 1;  /* Power down internal reference */
	uint16_t                      : 1;  /* Reserved */
} DAC81416_REG_DIFFCONFIG;

/* BRDCONFIG Register – Enables broadcast update for each DAC channel (0 to 15) */
typedef struct
{
	uint16_t DAC0_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC0 */
	uint16_t DAC1_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC1 */
	uint16_t DAC2_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC2 */
	uint16_t DAC3_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC3 */
	uint16_t DAC4_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC4 */
	uint16_t DAC5_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC5 */
	uint16_t DAC6_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC6 */
	uint16_t DAC7_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC7 */
	uint16_t DAC8_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC8 */
	uint16_t DAC9_BRDCAST_EN  : 1;  /*  Enable broadcast for DAC9 */
	uint16_t DAC10_BRDCAST_EN : 1;  /* Enable broadcast for DAC10 */
	uint16_t DAC11_BRDCAST_EN : 1;  /* Enable broadcast for DAC11 */
	uint16_t DAC12_BRDCAST_EN : 1;  /* Enable broadcast for DAC12 */
	uint16_t DAC13_BRDCAST_EN : 1;  /* Enable broadcast for DAC13 */
	uint16_t DAC14_BRDCAST_EN : 1;  /* Enable broadcast for DAC14 */
	uint16_t DAC15_BRDCAST_EN : 1;  /* Enable broadcast for DAC15 */
} DAC81416_REG_BRDCONFIG;

/* SYNCCONFIG Register – Enables synchronous update mode for each DAC channel (0 to 15) */
typedef struct
{
	uint16_t DAC0_SYNC_EN  : 1;  /*  Enable sync mode for DAC0 */
	uint16_t DAC1_SYNC_EN  : 1;  /*  Enable sync mode for DAC1 */
	uint16_t DAC2_SYNC_EN  : 1;  /*  Enable sync mode for DAC2 */
	uint16_t DAC3_SYNC_EN  : 1;  /*  Enable sync mode for DAC3 */
	uint16_t DAC4_SYNC_EN  : 1;  /*  Enable sync mode for DAC4 */
	uint16_t DAC5_SYNC_EN  : 1;  /*  Enable sync mode for DAC5 */
	uint16_t DAC6_SYNC_EN  : 1;  /*  Enable sync mode for DAC6 */
	uint16_t DAC7_SYNC_EN  : 1;  /*  Enable sync mode for DAC7 */
	uint16_t DAC8_SYNC_EN  : 1;  /*  Enable sync mode for DAC8 */
	uint16_t DAC9_SYNC_EN  : 1;  /*  Enable sync mode for DAC9 */
	uint16_t DAC10_SYNC_EN : 1;  /* Enable sync mode for DAC10 */
	uint16_t DAC11_SYNC_EN : 1;  /* Enable sync mode for DAC11 */
	uint16_t DAC12_SYNC_EN : 1;  /* Enable sync mode for DAC12 */
	uint16_t DAC13_SYNC_EN : 1;  /* Enable sync mode for DAC13 */
	uint16_t DAC14_SYNC_EN : 1;  /* Enable sync mode for DAC14 */
	uint16_t DAC15_SYNC_EN : 1;  /* Enable sync mode for DAC15 */
} DAC81416_REG_SYNCCONFIG;

/* TOGGCONFIG0 Register – Configures toggle mode for DAC channels 8 to 15.
 *
 * Toggle Mode Encoding:
 *   00 = Toggle mode disabled
 *   01 = Toggle mode enabled: TOGGLE0
 *   10 = Toggle mode enabled: TOGGLE1
 *   11 = Toggle mode enabled: TOGGLE2
 */
typedef struct
{
	uint16_t DAC8_AB_TOGG_EN  : 2;  /* Toggle mode config for DAC8  */
	uint16_t DAC9_AB_TOGG_EN  : 2;  /* Toggle mode config for DAC9  */
	uint16_t DAC10_AB_TOGG_EN : 2;  /* Toggle mode config for DAC10 */
	uint16_t DAC11_AB_TOGG_EN : 2;  /* Toggle mode config for DAC11 */
	uint16_t DAC12_AB_TOGG_EN : 2;  /* Toggle mode config for DAC12 */
	uint16_t DAC13_AB_TOGG_EN : 2;  /* Toggle mode config for DAC13 */
	uint16_t DAC14_AB_TOGG_EN : 2;  /* Toggle mode config for DAC14 */
	uint16_t DAC15_AB_TOGG_EN : 2;  /* Toggle mode config for DAC15 */
} DAC81416_REG_TOGGCONFIG0;

/* TOGGCONFIG1 Register – Configures toggle mode for DAC channels 0 to 7.
 *
 * Toggle Mode Encoding:
 *   00 = Toggle mode disabled
 *   01 = Toggle mode enabled: TOGGLE0
 *   10 = Toggle mode enabled: TOGGLE1
 *   11 = Toggle mode enabled: TOGGLE2
 */
typedef struct
{
	uint16_t DAC0_AB_TOGG_EN : 2;  /* Toggle mode config for DAC0 */
	uint16_t DAC1_AB_TOGG_EN : 2;  /* Toggle mode config for DAC1 */
	uint16_t DAC2_AB_TOGG_EN : 2;  /* Toggle mode config for DAC2 */
	uint16_t DAC3_AB_TOGG_EN : 2;  /* Toggle mode config for DAC3 */
	uint16_t DAC4_AB_TOGG_EN : 2;  /* Toggle mode config for DAC4 */
	uint16_t DAC5_AB_TOGG_EN : 2;  /* Toggle mode config for DAC5 */
	uint16_t DAC6_AB_TOGG_EN : 2;  /* Toggle mode config for DAC6 */
	uint16_t DAC7_AB_TOGG_EN : 2;  /* Toggle mode config for DAC7 */
} DAC81416_REG_TOGGCONFIG1;

/* DACPWDWN Register – Controls the power-down mode for DAC channels 0 to 15.
 * When set to 1, the corresponding DAC is in power-down mode, and its
 * output is connected to GND through a 10-kΩ internal resistor.
 */
typedef struct
{
	uint16_t DAC0_PWDWN  : 1;  /*  Power-down mode for DAC0  */
	uint16_t DAC1_PWDWN  : 1;  /*  Power-down mode for DAC1  */
	uint16_t DAC2_PWDWN  : 1;  /*  Power-down mode for DAC2  */
	uint16_t DAC3_PWDWN  : 1;  /* Power-down mode for DAC3 */
	uint16_t DAC4_PWDWN  : 1;  /* Power-down mode for DAC4 */
	uint16_t DAC5_PWDWN  : 1;  /* Power-down mode for DAC5 */
	uint16_t DAC6_PWDWN  : 1;  /* Power-down mode for DAC6 */
	uint16_t DAC7_PWDWN  : 1;  /* Power-down mode for DAC7 */
	uint16_t DAC8_PWDWN  : 1;  /* Power-down mode for DAC8  */
	uint16_t DAC9_PWDWN  : 1;  /* Power-down mode for DAC9  */
	uint16_t DAC10_PWDWN : 1;  /* Power-down mode for DAC10 */
	uint16_t DAC11_PWDWN : 1;  /* Power-down mode for DAC11 */
	uint16_t DAC12_PWDWN : 1;  /* Power-down mode for DAC12 */
	uint16_t DAC13_PWDWN : 1;  /* Power-down mode for DAC13 */
	uint16_t DAC14_PWDWN : 1;  /* Power-down mode for DAC14 */
	uint16_t DAC15_PWDWN : 1;  /* Power-down mode for DAC15 */
} DAC81416_REG_DACPWDWN;

/* DACRANGEn Register – Configures the output range for DAC channels a, b, c, and d.
 * The two outputs of a differential DAC pair must be configured to the same output
 *  range before enabling differential mode.
 * Valid output ranges:
 *   0000 = 0 to 5 V
 *   0001 = 0 to 10 V
 *   0010 = 0 to 20 V
 *   0100 = 0 to 40 V
 *   1001 = -5 V to +5 V
 *   1010 = -10 V to +10 V
 *   1100 = -20 V to +20 V
 *   1110 = -2.5 V to +2.5 V
 * All other values are invalid.
 */
typedef struct
{
	uint16_t DACd_RANGE : 4;  /* Output range for DACd */
	uint16_t DACc_RANGE : 4;  /* Output range for DACc */
	uint16_t DACb_RANGE : 4;  /* Output range for DACb */
	uint16_t DACa_RANGE : 4;  /* Output range for DACa */
} DAC81416_REG_DACRANGE;

/* TRIGGER Register – Controls various trigger actions like alarm reset,
   soft toggle, and loading DACs synchronously. */
typedef struct
{
	uint16_t SOFT_RESET: 4;   /* 1010h = Soft reset to default state. */
	uint16_t LDAC      : 1;   /* 1h = Load DACs synchronously as per SYNCCONFIG. */
	uint16_t AB_TOG0   : 1;   /* 1h = Toggle between Register A and B
                                for DACs in toggle mode 0. */
	uint16_t AB_TOG1   : 1;   /* 1h = Toggle between Register A and B
                                for DACs in toggle mode 1. */
	uint16_t AB_TOG2   : 1;   /* 1h = Toggle between Register A and B
                                for DACs in toggle mode 2. */
	uint16_t ALM_RESET : 1;   /* 1h = Clear alarm event. */
	uint16_t RESERVED  : 6;   /* Reserved bits. */
} DAC81416_REG_TRIGGER;

/* BRDCAST Register – Forces DAC channels configured for broadcast to
   update their data registers with the BRDCAST-DATA value. The data
   is MSB aligned in straight binary format. */
typedef struct
{
	uint16_t BRDCAST_DATA : 16; /* Data to update DAC channels configured for broadcast. */
} DAC81416_REG_BRDCAST;


/* DACn Register – Stores the data to be loaded into the DAC in MSB
   aligned straight binary format. In differential mode, data is
   loaded into the lowest-valued DAC in the pair, and writes to the
   higher DAC are ignored. */
typedef struct
{
	uint16_t DACn_DATA : 16; /* Data to be loaded to DACn (16-bit, 14-bit, or 12-bit). */
} DAC81416_REG_DACn;

/************************** function prototypes *****************************************/
void DAC81416_Init();
uint8_t DAC81416_ReadRegister(DAC81416_REG_MAP m_reg);
uint8_t DAC81416_SetStatus(DAC81416_FLAG_TYPE type, uint8_t status);
uint8_t DAC81416_GetStatus(DAC81416_FLAG_TYPE type);
uint8_t DAC81416_GetValue(uint16_t *pU16TxData);
uint8_t DAC81416_WriteRegister(DAC81416_REG_MAP m_reg, uint16_t pU16TxData);

void Callback_DAC81416TxComplete(void);
void Callback_DAC81416TxRxComplete(void);
#endif /* DRV_DAC81416_H_ */
