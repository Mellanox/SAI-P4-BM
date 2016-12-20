// defines

// genreal
#define TRUE 				1
#define FALSE 				0
#define MAC_LEARN_RECEIVER 	512

// header/metadata fields width
#define LAG_WDT				16
#define L2_BRIDGE_NUM_WDT 	3 // TODO
#define L2_BRIDGE_PORT_WDT   8 // TODO
#define VID_WDT				12
#define PACKET_TYPE_WDT		2 // TODO

// table size
#define FDB_TABLE_SIZE 512 // TODO

//phy
#define PHY_PORT_NUM 		64  // TODO
#define PHY_PORT_NUM_WDT 	6 	// TODO

// L2 
// L2_IF_TYPE
#define L2_ROUTER_TYPE 		0x1
#define L2_1D_BRIDGE 		0x2
#define L2_1Q_BRIDGE 		0x3

#define UNICAST 			0 // TODO
// STP STATE
#define STP_FORWARDING		1 // TODO
#define STP_DISCARDING      0 // TODO
#define TAG 				1 // TODO

// OUT_IF TYPE
#define OUT_IF_IS_PORT      0
#define OUT_IF_IS_LAG 		1 // TODO
#define OUT_IF_IS_ROUTER	2 // TODO

