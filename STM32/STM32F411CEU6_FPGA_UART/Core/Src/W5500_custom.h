#ifndef W5500_h
#define W5500_h

#define W5500_SELECT_COMMON_REGISTER 	( 0 << 3) 	// 00000 Selects Common Register.

#define W5500_SELECT_SOCKET0_REGISTER	( 1 << 3) 	// 00001 Selects Socket 0 Register
#define W5500_SELECT_TX0_BUFFER 		( 2 << 3) 	// 00010 Selects Socket 0 TX Buffer
#define W5500_SELECT_RX0_BUFFER 		( 3 << 3) 	// 00011 Selects Socket 0 RX Buffer
// 00100 Reserved
#define W5500_SELECT_SOCKET1_REGISTER	( 5 << 3)
#define W5500_SELECT_TX1_BUFFER 		( 6 << 3)
#define W5500_SELECT_RX1_BUFFER 		( 7 << 3)
// 01000 Reserved
#define W5500_SELECT_SOCKET2_REGISTER	( 9 << 3)
#define W5500_SELECT_TX2_BUFFER 		(10 << 3)
#define W5500_SELECT_RX2_BUFFER 		(11 << 3)
// 01100 Reserved
#define W5500_SELECT_SOCKET3_REGISTER	(13 << 3)
#define W5500_SELECT_TX3_BUFFER 		(14 << 3)
#define W5500_SELECT_RX3_BUFFER 		(15 << 3)
// 10000 Reserved
#define W5500_SELECT_SOCKET4_REGISTER	(17 << 3)
#define W5500_SELECT_TX4_BUFFER 		(18 << 3)
#define W5500_SELECT_RX4_BUFFER 		(19 << 3)
// 10100 Reserved
#define W5500_SELECT_SOCKET5_REGISTER	(21 << 3)
#define W5500_SELECT_TX5_BUFFER 		(22 << 3)
#define W5500_SELECT_RX5_BUFFER 		(23 << 3)
// 11000 Reserved
#define W5500_SELECT_SOCKET6_REGISTER	(25 << 3)
#define W5500_SELECT_TX6_BUFFER 		(26 << 3)
#define W5500_SELECT_RX6_BUFFER 		(27 << 3)
//11100 Reserved
#define W5500_SELECT_SOCKET7_REGISTER	(29 << 3)
#define W5500_SELECT_TX7_BUFFER 		(30 << 3)
#define W5500_SELECT_RX7_BUFFER 		(31 << 3)

#define W5500_WRITE						4

#define W5500_LEN_VAR					0
#define W5500_LEN_1						1
#define W5500_LEN_2						2
#define W5500_LEN_4						4


#define W5500_CR_MR      				0x0000  // Mode
#define W5500_CR_GAR     				0x0001  // Gateway Address
#define W5500_CR_SUBR    				0x0005  // Subnet Mask Address
#define W5500_CR_SHAR    				0x0009  // Source Hardware Address
#define W5500_CR_SIPR    				0x000F  // Source IP Address
#define W5500_CR_VERSIONR				0x0039	// Chip version
#define W5500_CR_PHYCFGR				0x002E

#define W5500_OFF_ADDR(VAL) ((VAL >> 8) & 0xFF), ((VAL >> 0) & 0xFF)

// -------------------------------------------------------------------

#define W5500_SCS_HIGH \
		HAL_GPIO_WritePin(W5500_SCS_GPIO_Port, W5500_SCS_Pin, GPIO_PIN_SET)

#define W5500_SCS_LOW \
		HAL_GPIO_WritePin(W5500_SCS_GPIO_Port, W5500_SCS_Pin, GPIO_PIN_RESET)

#define W5500_RST_HIGH \
		HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET)

#define W5500_RST_LOW \
		HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET)


#endif
