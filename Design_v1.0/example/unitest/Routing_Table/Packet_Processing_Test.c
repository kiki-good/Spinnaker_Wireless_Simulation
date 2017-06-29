

/* System library */
#include <sark.h>
#include <spin1_api.h>

/* Application library */
#include "Scheduler.h"
#include "Event.h"
#include "Object.h"
#include "timer.h"

/*
 Test basic functions for on broad chip
 communicatitions with SDP message(Spinnaker
 datagram protocal).
*/

/* Currently, we only support 4 chips on a single broad */
#define NUMBER_OF_XCHIPS 2
#define NUMBER_OF_YCHIPS 2

#define NORTH              3
#define SOUTH              2
#define EAST               1
#define WEST               0

#define NECh ((1 << NORTH) | (1 << EAST))
#define NWCh ((1 << NORTH) | (1 << WEST))
#define SECh ((1 << SOUTH) | (1 << EAST))
#define SWCh ((1 << SOUTH) | (1 << WEST))

#define EAST_LINK                  0
#define NORTH_EAST_LINK            1
#define NORTH_LINK                 2
#define WEST_LINK                  3
#define SOUTH_WEST_LINK            4
#define SOUTH_LINK                 5

#define DONT_ROUTE_KEY     0xffff
#define ROUTING_KEY(chip, core)    ((chip << 5) | core)
#define ROUTE_TO_CORE(core)        (1 << (core + 6))
#define ROUTE_TO_LINK(link)        (1 << link)

#define XY_TO_CHIP(x,y) ((x<<8)|y)

#define CORE_PER_CHIP 18
#define NUM_CORES NUMBER_OF_XCHIPS*NUMBER_OF_YCHIPS*CORE_PER_CHIP

/* used for testing; it could be modified */
/* inner communication test */
#define src_inner_sId "0,1,1"
#define dest_inner_sId "0,1,2"

/* inter communication test */
#define src_inter_sId "0,1,2"
#define dest_inter_sId "1,0,3"

/* used for routing table; */
/* singal to multiple routing path: 
    (0,1,1) ==> (0,1,4) OK!(inner Chip)
    (0,1,4) ==> (1,1,1) OK!(inter Chip)
    (0,1,1) ==> (1,1,2) (Not simply work) will creat a test app to do this
    
    (0,1,1) ==> (1,0,3)
    (0,1,1) ==> (1,1,1)
 */
#define src_mc_sID_M "0,1,1"
#define dest_mc_sID_A "0,1,4"
#define dest_mc_sID_B "1,0,3"
#define dest_mc_sID_C "1,1,1"
#define dest_mc_sID_D "1,1,2"

/* (0,1,1) */
#define TEST_ROUTE_CHIPID_M    XY_TO_CHIP(0,1) /* master key */
#define TEST_ROUTE_COREID_M    1  /* will optimize here */

/* (0,1,4) */
#define TEST_ROUTE_CHIPID_A    XY_TO_CHIP(0,1)
#define TEST_ROUTE_COREID_A    4  /* will optimize here */  

/* (1,0,3) */
#define TEST_ROUTE_CHIPID_B    XY_TO_CHIP(1,0)
#define TEST_ROUTE_COREID_B    3  /* will optimize here */

/* (1,1,1) */
#define TEST_ROUTE_CHIPID_C    XY_TO_CHIP(1,1)
#define TEST_ROUTE_COREID_C    1  /* will optimize here */

/* (1,1,2) */
#define TEST_ROUTE_CHIPID_D    XY_TO_CHIP(1,1)
#define TEST_ROUTE_COREID_D    2  /* will optimize here */


#define TEST_ROUTE_M_KEY    ROUTING_KEY(TEST_ROUTE_CHIPID_M,TEST_ROUTE_COREID_M)

#define TEST_ROUTE_A_KEY    ROUTING_KEY(TEST_ROUTE_CHIPID_A,TEST_ROUTE_COREID_A)
#define ROUTE_TO_A			ROUTE_TO_CORE(TEST_ROUTE_COREID_A)

#define TEST_ROUTE_B_KEY	ROUTING_KEY(TEST_ROUTE_CHIPID_B,TEST_ROUTE_COREID_B)
#define ROUTE_TO_B          ROUTE_TO_CORE(TEST_ROUTE_COREID_B)/* to do */

#define TEST_ROUTE_C_KEY	ROUTING_KEY(TEST_ROUTE_CHIPID_C,TEST_ROUTE_COREID_C)
#define ROUTE_TO_C_0			ROUTE_TO_CORE(TEST_ROUTE_COREID_C)|ROUTE_TO_LINK(EAST_LINK)
#define ROUTE_TO_C_1			ROUTE_TO_CORE(TEST_ROUTE_COREID_C)|ROUTE_TO_LINK(WEST_LINK)
#define ROUTE_TO_C_2			ROUTE_TO_CORE(TEST_ROUTE_COREID_C)|ROUTE_TO_LINK(NORTH_LINK)
#define ROUTE_TO_C_3			ROUTE_TO_CORE(TEST_ROUTE_COREID_C)|ROUTE_TO_LINK(SOUTH_LINK)
#define ROUTE_TO_C_4			ROUTE_TO_CORE(TEST_ROUTE_COREID_C)|ROUTE_TO_LINK(NORTH_EAST_LINK)
#define ROUTE_TO_C_5			ROUTE_TO_CORE(TEST_ROUTE_COREID_C)|ROUTE_TO_LINK(SOUTH_WEST_LINK)


typedef struct topology{
	uint x;
	uint y;
	uint core;
	char reserved;
	char* sId;
}topology;

topology topology_map[NUM_CORES]= {
	/* x,y,core,reserved,sID(x,y,core) */
	/* core 0 */
	{0,0,0,1,"0,0,0"},
	{0,1,0,1,"0,1,0"},	
	{1,0,0,1,"1,0,0"},
	{0,1,0,1,"0,1,0"},
	{1,1,0,1,"1,1,0"},

	/* core 1*/
	{0,0,1,0,"0,0,1"},
	{0,1,1,0,"0,1,1"},
	{1,0,1,0,"1,0,1"},
	{0,1,1,0,"0,1,1"},
	{1,1,1,0,"1,1,1"},

	/* core 2 */
	{0,0,2,0,"0,0,2"},
	{0,1,2,0,"0,1,2"},
	{1,0,2,0,"1,0,2"},
	{0,1,2,0,"0,1,2"},
	{1,1,2,0,"1,1,2"},

	/* core 2 */
	{0,0,3,0,"0,0,3"},
	{0,1,3,0,"0,1,3"},
	{1,0,3,0,"1,0,3"},
	{0,1,3,0,"0,1,3"},
	{1,1,3,0,"1,1,3"},

	/* core 4 */
	{0,1,4,0,"0,1,4"}
};

uint core_map[NUMBER_OF_XCHIPS][NUMBER_OF_YCHIPS] =
{
  {2, 2},
  {2, 2}
};

uint chip_map[NUMBER_OF_XCHIPS][NUMBER_OF_YCHIPS] =
{
    {SWCh, NWCh},
    {SECh, NECh}
};

// ---
// Routing table constants and macro definitions
// ---
#define NORTH              3
#define SOUTH              2
#define EAST               1
#define WEST               0

#define NECh ((1 << NORTH) | (1 << EAST))
#define NWCh ((1 << NORTH) | (1 << WEST))
#define SECh ((1 << SOUTH) | (1 << EAST))
#define SWCh ((1 << SOUTH) | (1 << WEST))

#define NORTH_ARRIVED      (1 << NORTH)
#define SOUTH_ARRIVED      (1 << SOUTH)
#define NONE_ARRIVED       0

#define CORE_TO_NORTH(core)        (core + 1)
#define CORE_TO_SOUTH(core)        (core - 1)

#define CHIP_TO_NORTH(chip)     (chip + 1)
#define CHIP_TO_SOUTH(chip)     (chip - 1)
#define IS_NORTHERNMOST_CHIP(x, y) ((chip_map[x][y] & (1 << NORTH)) != 0)
#define IS_SOUTHERNMOST_CHIP(x, y) ((chip_map[x][y] & (1 << SOUTH)) != 0)
#define NORTHERNMOST_CORE(core)    (((core - 1) | 0x3) + 1)
#define SOUTHERNMOST_CORE(core)    (((core - 1) & 0xc) + 1)



#define NORTH_LINK                 2
#define SOUTH_LINK                 5

#define TIMER_TICK_PERIOD  25000


/* CPU OR Core related */
uint coreID;
uint chipID;
uint my_chip, my_x, my_y;
uint init_arrived;

#define IS_NORTHERNMOST_CORE(core) (((core - 1) & 0x3) == 0x3)
#define IS_SOUTHERNMOST_CORE(core) (((core - 1) & 0x3) == 0x0)
#define IS_NORTHERNMOST_CHIP(x, y) ((chip_map[x][y] & (1 << NORTH)) != 0)
#define IS_SOUTHERNMOST_CHIP(x, y) ((chip_map[x][y] & (1 << SOUTH)) != 0)

/* Callback related */
uint my_key;
uint north_key;
uint south_key;
sdp_msg_t *my_msg = NULL;
uint my_route = 0;  // where I send my data

uint route_from_north = FALSE;
uint route_from_south = FALSE;

uint data_to_send = 1;

char isSource = 0,isDest = 0;
uint srcIndex = 0,desIndex = 0;

static char match_sID(char* sStringID,uint* index)
{
	uint i;
	char result = 0;
    
	for(i = 0;i < sizeof(topology_map)/sizeof(topology);i++)
	{
		if(strcmp(topology_map[i].sId,sStringID) == 0)
		{
			result = 1;
			*index = i;
			break;
		}
	}

	return result;
}

char validationChipLoadApp(char* sStringID)
{
	uint srcIndex = 0;
	char rc = 0;
	
	if(match_sID(sStringID,&srcIndex))
	 {		  
	   if((topology_map[srcIndex].x == my_x) && 
		   (topology_map[srcIndex].y == my_y) &&
		   (topology_map[srcIndex].core == coreID))
	   {
#if 0	   
		   io_printf(IO_BUF, "(1) %d, (1) %d\n",topology_map[srcIndex].x,my_x);  
		   io_printf(IO_BUF, "(2) %d, (2) %d\n",topology_map[srcIndex].y,my_y);  
		   io_printf(IO_BUF, "(3) %d, (3) %d\n",topology_map[srcIndex].core,coreID);
		   isDest = 0;	   
		   isSource = 1;
#endif	
		   
		   rc = 1;
#if 0
		   io_printf(IO_BUF, "init passed;(1) srcIndex [%d],srcString [%s]\n",srcIndex,topology_map[srcIndex].sId); 
#endif
		   return rc;
		     
	   }
	   else
	   {
		   //io_printf(IO_BUF, "Error(1); app_load doesn't match with configuration\n");
		   return rc ;
	   }
	 }
	 else
	 {	   
	   //io_printf(IO_BUF, "Error(1); Index not implemented\n");
	   return rc;
	 }

}


/* Callbacks */

// Callback MC_PACKET_RECEIVED
void receive_data (uint key, uint payload) {
  
  if(validationChipLoadApp(dest_mc_sID_A))
  {
  	io_printf (IO_BUF, "Packet From M:MC_PACKET:Received data (0x%x, %u)\n", key, payload);
  }

  if(validationChipLoadApp(dest_mc_sID_C))
  {
    io_printf (IO_BUF, "Packet From A:MC_PACKET:Received data (0x%x, %u)\n", key, payload);
  }
  
  if(validationChipLoadApp(dest_mc_sID_D))
  {  
    io_printf (IO_BUF, "Packet From M :(Innter)MC_PACKET:Received data (0x%x, %u)\n", key, payload);
  }
}

uint counter;

// Callback TIMER_TICK
void update (uint a, uint b) {
  //io_printf (IO_BUF, "tick %d\n", data_to_send);

  // send data to neighbor(s)
  //spin1_send_mc_packet(my_key, data_to_send, WITH_PAYLOAD);

  // report data to host
  
  if(isSource)
  {
  	
	spin1_send_mc_packet(TEST_ROUTE_M_KEY,data_to_send,WITH_PAYLOAD);
	/* increase the value to check whether the receiver really works */
	data_to_send++;
  }

  
  if(validationChipLoadApp(dest_mc_sID_A))
  {
     //io_printf (IO_BUF, "send packet to chip(1,1)\n");
     /*send mc packet to (1,1) */
	 spin1_send_mc_packet(TEST_ROUTE_A_KEY,data_to_send,WITH_PAYLOAD);
  }
}

void routing_table_init () {
  my_key = ROUTING_KEY(chipID, coreID);

#if 1
  if(validationChipLoadApp(src_mc_sID_M))
  {
	  /* data send through(from) M so key is  TEST_ROUTE_M_KEY */
	  spin1_set_mc_table_entry(coreID,     // entry
	                             TEST_ROUTE_M_KEY,            // key
	                             0xffffffff,           // mask
	                             ROUTE_TO_A // route
	                            );

	  io_printf (IO_BUF, "ROUTE_TO_A %x\n",ROUTE_TO_A);  
	  io_printf (IO_BUF, "TEST_ROUTE_M_KEY %x\n",TEST_ROUTE_M_KEY);  
	  io_printf (IO_BUF, "my_key %x\n",my_key);

	  /* The table below is only set for communication between  (0,1,1) ==> (1,1,2) */

	  spin1_set_mc_table_entry(coreID+4,     // entry
	                             TEST_ROUTE_M_KEY,            // key
	                             0xffffffff,           // mask
	                             ROUTE_TO_LINK(EAST_LINK) // route
	                            );

	  //0xffffffff
	  
  }
  
  /* should only be called when loading (0,1,4)*/
  if(validationChipLoadApp(dest_mc_sID_A))
  {
  	  /* data send through(from) A so key is  TEST_ROUTE_A_KEY */
	  spin1_set_mc_table_entry(coreID+1,     // entry
	                             TEST_ROUTE_A_KEY,            // key
	                             0xffffffff,           // mask
	                             ROUTE_TO_LINK(EAST_LINK) // route
	                            );

	  io_printf (IO_BUF, "ROUTE_TO_B Chip %x\n",ROUTE_TO_A);  
	  io_printf (IO_BUF, "TEST_ROUTE_A_KEY %x\n",TEST_ROUTE_M_KEY);  
  }

  /* configure the other side(MUST!) */
  /* should only be called when loading (1,1,1) */
  if(validationChipLoadApp(dest_mc_sID_C))
  {  
      uint west_route_key,west_route;
      /* Packet send from (0,1,4)*/
	  west_route_key = ROUTING_KEY(XY_TO_CHIP(0,1),TEST_ROUTE_COREID_A);

      /* Packet then route to core 1 in (1,1) */
	  west_route = ROUTE_TO_CORE(coreID);
  
  	  /* data send through(from) A so key is  TEST_ROUTE_A_KEY */
	  /* Test route to core 4(from core 1 in Chip(1,1))*/
      spin1_set_mc_table_entry(coreID+2,     // entry
	                             west_route_key,            // key
	                             0xffffffff,           // mask
	                             west_route // route
	                            );

	  io_printf (IO_BUF, "ROUTE_TO_B Chip for B Chip's core %x\n",ROUTE_TO_A);  
	  io_printf (IO_BUF, "(From)TEST_ROUTE_A_KEY %x\n",TEST_ROUTE_A_KEY);  
  }

  
  if(validationChipLoadApp(dest_mc_sID_D))
  {
  	  uint west_route_key,west_route;
      /* Packet send from (0,1,1)*/
	  west_route_key = ROUTING_KEY(XY_TO_CHIP(0,1),TEST_ROUTE_COREID_M);

      /* Packet then route to core 1 in (1,1) */
	  west_route = ROUTE_TO_CORE(coreID);
  
  	  /* data send through(from) A so key is  TEST_ROUTE_A_KEY */
	  /* Test route to core 4(from core 1 in Chip(1,1))*/
      spin1_set_mc_table_entry(coreID+2,     // entry
	                             west_route_key,            // key
	                             0xffffffff,           // mask
	                             west_route // route
	                            );

	  io_printf (IO_BUF, "ROUTE_TO_B Chip for B Chip's core %x\n",ROUTE_TO_A);  
	  io_printf (IO_BUF, "(From)TEST_ROUTE_A_KEY %x\n",TEST_ROUTE_A_KEY);  		
  }
  	
#endif

#if 0
  spin1_set_mc_table_entry(chipID+1,     // entry
                             TEST_ROUTE_M_KEY,            // key
                             0xffffffff,           // mask
                             ROUTE_TO_C_0 // route
                            );
  spin1_set_mc_table_entry(chipID+2,     // entry
                             TEST_ROUTE_M_KEY,            // key
                             0xffffffff,           // mask
                             ROUTE_TO_C_1 // route
                            );
  spin1_set_mc_table_entry(chipID+3,     // entry
                             TEST_ROUTE_M_KEY,            // key
                             0xffffffff,           // mask
                             ROUTE_TO_C_2 // route
                            );
  spin1_set_mc_table_entry(chipID+4,     // entry
                             TEST_ROUTE_M_KEY,            // key
                             0xffffffff,           // mask
                             ROUTE_TO_C_3 // route
                            );
		
  spin1_set_mc_table_entry(chipID+5,     // entry
                             TEST_ROUTE_M_KEY,            // key
                             0xffffffff,           // mask
                             ROUTE_TO_C_4 // route
                            );

  spin1_set_mc_table_entry(chipID+6,     // entry
                             TEST_ROUTE_M_KEY,            // key
                             0xffffffff,           // mask
                             ROUTE_TO_C_5 // route
                            );  
#endif

#if 0  
	uint e = rtr_alloc (7); // Get 5 router entries

	if (e == 0)
    	rt_error (RTE_ABORT);

  	rtr_mc_set (e, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_C_0            	// route
	      );

	rtr_mc_set (e+1, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_C_1            	// route
	      );

	rtr_mc_set (e+2, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_C_2            	// route
	      );

	rtr_mc_set (e+3, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_C_3           	// route
	      );

	rtr_mc_set (e+4, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_C_4           	// route
	      );

	rtr_mc_set (e+5, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_C_5           	// route
	      );

	rtr_mc_set (e+6, 			// entry
	      TEST_ROUTE_M_KEY,             	// key
	      0xffffffff,         	// mask
	      ROUTE_TO_A           	// route
	      );
#endif	
}

void c_main (void)
{
  io_printf(IO_BUF, "Starting communication test.\n");
  
  // get this core's ID
  coreID = spin1_get_core_id();
  chipID = spin1_get_chip_id();
  
  io_printf(IO_BUF, "The current CPU is %s\n",leadAp ? "leadAp":"no-leadAp");
  io_printf(IO_BUF, "The current core ID is [%d]\n",coreID);  
  io_printf(IO_BUF, "The current chip ID is [%d]\n",chipID);

  // get this chip's coordinates for core map
  my_x = chipID >> 8;
  my_y = chipID & 0xff;
  my_chip = (my_x * NUMBER_OF_YCHIPS) + my_y;

 /* core ID 1 seems to be leadAP */
 /* Test Case 1. leadAP to noleadAp in the same Chip(different cores) */
 /* src_inter_sId "0,1,1" ==> dest_inter_sId "0,1,2" */

 /* sanity check and set flag */  

 if(validationChipLoadApp(src_mc_sID_M))
 {
 	isDest = 0;	   
	isSource = 1;
	
	io_printf(IO_BUF, "The current chip/core is Source\n");
 }
 else if((validationChipLoadApp(dest_mc_sID_A)) ||
 	(validationChipLoadApp(dest_mc_sID_B)) ||
 	(validationChipLoadApp(dest_mc_sID_C)) ||
 	(validationChipLoadApp(dest_mc_sID_D)))
 {
 	isDest = 1;	   
	isSource = 0;
	
	io_printf(IO_BUF, "The current chip/core is Dest\n");
 }
 else
 {
	io_printf(IO_BUF, "app_load falied! Reason:unmatched\n");
 	return;
 }

  // set the core map for the simulation
  //spin1_application_core_map(NUMBER_OF_XCHIPS, NUMBER_OF_YCHIPS, core_map);

  spin1_set_timer_tick(TIMER_TICK_PERIOD);
  
  // register callbacks
  //spin1_callback_on(MC_PACKET_RECEIVED, receive_data, 0);
  spin1_callback_on(MCPL_PACKET_RECEIVED, receive_data, 0);
  spin1_callback_on(TIMER_TICK, update, 0);

  // Initialize routing tables
  //if(isSource)
  {
  	routing_table_init();
  }
  // go
  spin1_start(FALSE);

  io_printf(IO_BUF, "App exit...........\n");
}
