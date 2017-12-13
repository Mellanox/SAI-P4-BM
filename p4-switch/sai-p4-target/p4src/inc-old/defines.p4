// defines

// genreal
#define TRUE 				1
#define FALSE 				0
#define MAC_LEARN_RECEIVER 	512

//  Mirroring and traps
#define COPY_TO_CPU_MIRROR_ID 250
#define PORT_BRIDGE_ROUTER  251

// header/metadata fields width
#define LAG_WDT				16
#define L2_BRIDGE_NUM_WDT 	3 // TODO
#define L2_BRIDGE_PORT_WDT   8 // TODO
#define VID_WDT				12
#define PACKET_TYPE_WDT		2 // TODO

////////////////////////////
// table size
////////////////////////////
// L2
#define FDB_TABLE_SIZE 512 // TODO
// L3
#define L3_EGRESS_IF_TABLE_SIZE 10 // TODO
#define ROUTER_IF_TABLE_SIZE 12 //TODO
#define ROUTER_LPM_TABLE_SIZE 1024 //TODO
#define NEIGH_TABLE_SIZE 1024 // TODO
#define NHOP_TABLE_SIZE 128 //TODO
#define NHOP_GRP_TABLE_SIZE 16 //TODO
////////////////////////////

//phy
#define PHY_PORT_NUM 		64  // TODO
#define PHY_PORT_NUM_WDT 	8 	// TODO


//port 
#define PORT_MODE_SUBPORT   0x1
#define PORT_MODE_PORT 		0x0
 
//bridge ports 
//#define 1Q_BRIDGE_PORT_ROUTER	100 

// L2 
// L2_IF_TYPE
#define L2_ROUTER_TYPE 		0x1
#define L2_1D_BRIDGE 		0x2
#define L2_1Q_BRIDGE 		0x3


// STP STATE
#define STP_FORWARDING		1 // TODO
#define STP_DISCARDING      0 // TODO
#define TAG 	


#define MAC_BASE_MC_LOOKUP  0 
#define SG_IP_BASE_MC_LOOKUP  2 
#define G_IP_BASE_MC_LOOKUP  1 //need to add  


// OUT_IF TYPE
#define OUT_IF_IS_PORT      0
#define OUT_IF_IS_LAG 		1 // TODO
#define OUT_IF_IS_ROUTER	2 // TODO


// CPU_HDR TYPE
#define PORT    0
#define LAG 	1
#define VLAN 	2


// L3
// ERIF_TYPE
#define VLAN_IF 0

// PACKET_ACTION
#define ACTION_FORWARD 0x1
#define ACTION_TRAP 0x2
#define ACTION_COPY 0x3
#define ACTION_DROP 0x4