
/* TEST INNER SDP MESSAGE with 5ns Timer */


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


#define DONT_ROUTE_KEY     0xffff
#define ROUTING_KEY(chip, core)    ((chip << 5) | core)
#define ROUTE_TO_CORE(core)        (1 << (core + 6))
#define ROUTE_TO_LINK(link)        (1 << link)

#define XY_TO_CHIP(x,y) ((x<<8)|y)

#define TEST_INTER_SDP 0

#define TEST_INNER_SDP 1

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
    (0,1,1) ==> (0,1,2)
    (0,1,1) ==> (1,0,3)
    (0,1,1) ==> (1,1,3)
 */

/* (0,1,1) */
#define TEST_ROUTE_CHIPID_M    XY_TO_CHIP(0,1) /* master key */
#define TEST_ROUTE_COREID_M    1  /* will optimize here */

/* (0,1,2) */
#define TEST_ROUTE_CHIPID_A    XY_TO_CHIP(0,1)
#define TEST_ROUTE_COREID_A    2  /* will optimize here */  

/* (1,0,3) */
#define TEST_ROUTE_CHIPID_B    XY_TO_CHIP(1,0)
#define TEST_ROUTE_COREID_B    3  /* will optimize here */

/* (1,1,3) */
#define TEST_ROUTE_CHIPID_C    XY_TO_CHIP(1,1)
#define TEST_ROUTE_COREID_C    3  /* will optimize here */

#define TEST_ROUTE_M_KEY    ROUTING_KEY(TEST_ROUTE_CHIPID_M,TEST_ROUTE_COREID_M)

#define TEST_ROUTE_A_KEY    ROUTING_KEY(TEST_ROUTE_CHIPID_A,TEST_ROUTE_COREID_A)
#define ROUTE_TO_A			ROUTE_TO_CORE(TEST_ROUTE_COREID_A)

#define TEST_ROUTE_B_KEY	ROUTING_KEY(TEST_ROUTE_CHIPID_B,TEST_ROUTE_COREID_B)
#define TEST_ROUTE_C_KEY	ROUTING_KEY(TEST_ROUTE_CHIPID_C,TEST_ROUTE_COREID_C)

#define TEST_TIMER_GRANULARITY 0

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
	{1,1,3,0,"1,1,3"}	
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

//#define TIMER_TICK_PERIOD  25000
#define TIMER_TICK_PERIOD  1000000 /* 1s */


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

volatile uint time_to_send = 0;
volatile uint time_to_receive = 0;

/* SDP TEST Parameter */
uint TEST_Length = sizeof(uint);//SDP_BUF_SIZE;
uint NUM_REPEAT_SENT = 0;

// Start the 32-bit TC2 timer counting down from zero. When it reaches zero
// it will stop. This takes around 21.5 seconds so this is the maximum time
// that can be measured without restarting the timer. Each timer step is
// one CPU clock tick (5ns).

void start_timer ()
{
  tc[T2_CONTROL] = 0xc3;	// Enable timer, one-shot mode
  tc[T2_LOAD] = 0;		// Start counting down from zero
}


// Read the timer. Note that it is counting DOWN so do the subtract in
// the right order to get correct elapsed times.

uint read_timer ()
{
  return tc[T2_COUNT];
}


uint GetClockMs()
{
	return sv->clock_ms;
}

uint GetTimeMs()
{
	return sv->time_ms;
}

void manualDelay(uint numLoops)
{
	volatile uint i = 0;

#if TEST_TIMER_GRANULARITY	
	uint now_ms;
#endif

	do{		
	  i++;
#if	TEST_TIMER_GRANULARITY
	  now_ms = GetClockMs();
	  io_printf (IO_BUF, "Current ms %d\n", now_ms);  
#endif	  
	}while(i<numLoops);
}

/* Callbacks */
void receive_data (uint key, uint payload) {
  io_printf (IO_BUF, "MC_PACKET:Received data (%u, %u)\n", key, payload);

  data_to_send = payload + 1;
}

/*
message->length = 280; //although SDP packages are 292bytes in size, 280 is the maximum one can specify here... ?! 
SDT_hdr_t
message->flags = 0x87; //use 0x87 if reply packet is expected and 0x07 if no reply packet is expected. Have to double check what this means...
message->tag = 0; //IP-Tag number. Used for spinnaker to extern-packages. 0 if internal
message->dest_port = (uchar) 1 << 7 | 3; //Destination Port: 8bits. The lower 5 bits specify the destination Core (i.e.1-16), the upper 3 bits are an "arbitrary" port number from 1-7 (do NOT use 0!)
										  ////1  <<  7 specifies port number 4 (100 00000) using the upper 3 bits, 3 specifies the desired CPU. Both are combined in a single byte using a binary OR.
message->srce_port = (uchar) 1 << 5 | 1; //Same here, except this is the source
message->dest_addr = SDP_CHIP_1_0; //Destination Chip address. This is a 2byte value: the upper byte specifies the X-coordinate, the lower byte the y-coordinate (i.e. Chip 1,0 would be 0x0100). 
										//Here, SDP_CHIP_1_0 is a typedef to (ushort) 0x0100
message->srce_addr = SDP_CHIP_0_0; //Source Chip address, same as above.  
*/

uint counter;

// Callback TIMER_TICK
void update (uint a, uint b) {
  //io_printf (IO_BUF, "tick %d\n", data_to_send);

  // send data to neighbor(s)
  //spin1_send_mc_packet(my_key, data_to_send, WITH_PAYLOAD);

  // report data to host
  if(isSource)
  {   
  
    uint i=0;
    do{
		if(!my_msg)
		{
			//io_printf (IO_BUF, "send sdp message;counter === %d\n",counter++);
			
		    my_msg = spin1_msg_get();
			if(!my_msg)
			{
				io_printf (IO_BUF, "Error!no free message memory available\n");
				return;
			}
			my_msg->tag = 0;                    // IPTag 1
	  		my_msg->flags = 0x87;
		}
		
	    uint len = sizeof(uint);
	    spin1_memcpy (my_msg->data, (uint *) &data_to_send, len);
	    my_msg->length = sizeof (sdp_hdr_t) + sizeof (cmd_hdr_t) + TEST_Length;
		my_msg->dest_port = (uchar) 1 << 7 | topology_map[desIndex].core;
		my_msg->srce_port = (uchar) 1 << 7 | topology_map[srcIndex].core;	
		my_msg->dest_addr = topology_map[desIndex].x << 8 | topology_map[desIndex].y;
		my_msg->srce_addr = topology_map[srcIndex].x << 8 | topology_map[srcIndex].y;

		if(my_msg->srce_addr != sv->p2p_addr)
		{		
			io_printf (IO_BUF, "Error!unmatched src address\n");
			return;
		}
#if 0		
	    io_printf (IO_BUF, "my_msg.dest_port %x\n",my_msg.dest_port);	
	    io_printf (IO_BUF, "my_msg.srce_port %x\n",my_msg.srce_port);

		io_printf (IO_BUF, "my_msg.dest_addr %x\n",my_msg.dest_addr);	
	    io_printf (IO_BUF, "my_msg.srce_addr %x\n",my_msg.srce_addr);
		
	    io_printf (IO_BUF, "sv->p2p_addr %x\n",sv->p2p_addr);
#endif
	    // and send SDP message!

		//if(i == 0)
		{		
		    /* Log the time when we send the first packet */
			time_to_send = read_timer();
		}
		
	    spin1_send_sdp_msg (my_msg, 100); // 100ms timeout

		spin1_msg_free(my_msg);
		
		my_msg = NULL;

		i++;
    }while(i<NUM_REPEAT_SENT);
	
    //io_printf (IO_BUF, "send sdp message;counter %d\n",counter++);
  }
}


// Callback SDP_PACKET_RX
void host_data (uint mailbox, uint port) {

  sdp_msg_t *msg = (sdp_msg_t *) mailbox;
  uint *pData = (uint *) msg->data;

  /* If sender gets ack_message back from receiver, then just receive it without sending it back again */
  if(isSource)
  { 
    static uint NUM_Packets_Received = 0;

	NUM_Packets_Received++;

	/* Log the time when we receive the last packet */

    //if(NUM_Packets_Received%NUM_REPEAT_SENT == 0)
    {
		time_to_receive = read_timer();
		
		//io_printf (IO_BUF, "Time Counter(Start) %d ns\n",time_to_send);		
		//io_printf (IO_BUF, "Time Counter(End) %d ns\n",time_to_receive);

		io_printf (IO_BUF, "Time Spent %d ns\n",(time_to_send - time_to_receive)*5);
#if 0
		io_printf (IO_BUF, "Time To Send %d at ms\n",time_to_send);	
		io_printf (IO_BUF, "Time To Receive %d at ms\n",time_to_receive);
#endif
    }

#if 0
    io_printf (IO_BUF, "Received Ack packet from Receiver port number %d,mailbox %x counter %d\n",
	  			port,mailbox,counter++);
	
	io_printf (IO_BUF, "Received Ack:cmd: %d\n", msg->cmd_rc);  
	io_printf (IO_BUF, "Receiver Ack:data: %d\n", *pData);
#endif	
	spin1_msg_free (msg);
  }
  
  /* If receiver gets data,then receiver sends Data Back to the sender */
  if(isDest)
  {
#if 0  
    io_printf (IO_BUF, "Received packet from Source port number %d,mailbox %x counter %d\n",
	  			port,mailbox,counter++);
	
	io_printf (IO_BUF, "Received from Source:cmd: %d\n", msg->cmd_rc);  
	io_printf (IO_BUF, "Receiver from Source:data: %d\n", *pData);
	
	io_printf (IO_BUF, "Send Ack to Source\n");
#endif
	/* Must Release Here */
	spin1_msg_free (msg);
    
    uint i=0;
	do{	
		sdp_msg_t *Ack_Message;
		uint Ack_data_to_send = 5;		
	    uint len = sizeof(*pData);
		Ack_Message = spin1_msg_get();
	    spin1_memcpy (Ack_Message->data, (uint *) pData, len);
	    Ack_Message->length = sizeof (sdp_hdr_t) + sizeof (cmd_hdr_t) + len;
		Ack_Message->tag = 0;                    // IPTag 1
	  	Ack_Message->flags = 0x87;
		Ack_Message->dest_port = (uchar) 1 << 7 | topology_map[srcIndex].core;
		Ack_Message->srce_port = (uchar) 1 << 7 | topology_map[desIndex].core;	
		Ack_Message->dest_addr = topology_map[srcIndex].x << 8 | topology_map[srcIndex].y;
		Ack_Message->srce_addr = topology_map[desIndex].x << 8 | topology_map[desIndex].y;
	    spin1_send_sdp_msg (Ack_Message, 100); // 100ms timeout
		spin1_msg_free(Ack_Message);

		i++;
	}while(i<NUM_REPEAT_SENT);
  }
}

void sdp_init () {
  // fill in SDP destination fields
#if 0  
  my_msg.tag = 1;                    // IPTag 1
  my_msg.dest_port = PORT_ETH;       // Ethernet
  my_msg.dest_addr = sv->dbg_addr;   // Root chip

  // fill in SDP source & flag fields
  my_msg.flags = 0x07;
  my_msg.srce_port = coreID;
  my_msg.srce_addr = sv->p2p_addr;
#else
  my_msg = spin1_msg_get();
  my_msg->tag = 0;                    // IPTag 1
  my_msg->flags = 0x87;
#endif 
}


char match_sID(char* sStringID,uint* index)
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

#if TEST_INTER_SDP
  /* core ID 1 seems to be leadAP */
  /* Test Case 2. noleadAP to noleadAp in the diffferent Chips and different cores */
  /* src_inter_sId "0,1,2" ==> dest_inter_sId "1,0,3" */
  /* sanity check and set flag */  

  /* sanity check and set flag */  
  if(match_sID(src_inter_sId,&srcIndex))
  {        
	if((topology_map[srcIndex].x == my_x) && 
		(topology_map[srcIndex].y == my_y) &&
		(topology_map[srcIndex].core == coreID))
  	{
	  	//io_printf(IO_BUF, "Error(1); app_load doesn't match with configuration\n");
		io_printf(IO_BUF, "1-(1) %d, (1) %d\n",topology_map[srcIndex].x,my_x);	
		io_printf(IO_BUF, "1-(2) %d, (2) %d\n",topology_map[srcIndex].y,my_y);	
		io_printf(IO_BUF, "1-(3) %d, (3) %d\n",topology_map[srcIndex].core,coreID);

		isDest = 0;		
		isSource = 1;
		
		io_printf(IO_BUF, "init passed;(1) srcIndex [%d],srcString [%s]\n",srcIndex,topology_map[srcIndex].sId);	
  	}
#if 0	
	else
	{
	    io_printf(IO_BUF, "Error(1); app_load doesn't match with configuration\n");
	    return;
	}
#endif	
  }
  else
  {  	
	io_printf(IO_BUF, "Error(1); Index not implemented\n");
	return;
  }

  if(match_sID(dest_inter_sId,&desIndex))
  {
	if((topology_map[desIndex].x == my_x) && 
		(topology_map[desIndex].y == my_y) &&
		(topology_map[desIndex].core == coreID))
  	{
	  	//io_printf(IO_BUF, "Error(2); app_load doesn't match with configuration\n");
		io_printf(IO_BUF, "2-(1) %d, (1) %d\n",topology_map[desIndex].x,my_x);	
		io_printf(IO_BUF, "2-(2) %d, (2) %d\n",topology_map[desIndex].y,my_y);	
		io_printf(IO_BUF, "2-(3) %d, (3) %d\n",topology_map[desIndex].core,coreID);
		
		isDest = 1;		
		isSource = 0;
		
		io_printf(IO_BUF, "init passed(2); desIndex [%d],destString [%s]\n",desIndex,topology_map[desIndex].sId);
  	}
#if 0	
	else
	{
	    io_printf(IO_BUF, "Error(2); app_load doesn't match with configuration\n");
	    return;
	}
#endif	
  }
  else
  {  	
	io_printf(IO_BUF, "Error(2); Index not implemented\n");
	return;
  }
#else if TEST_INNER_SDP

  /* core ID 1 seems to be leadAP */
  /* Test Case 1. leadAP to noleadAp in the same Chip(different cores) */
  /* src_inter_sId "0,1,1" ==> dest_inter_sId "0,1,2" */

  if(match_sID(src_inner_sId,&srcIndex))
  {		  
    if((topology_map[srcIndex].x == my_x) && 
	   (topology_map[srcIndex].y == my_y) &&
	   (topology_map[srcIndex].core == coreID))
    {
	   //io_printf(IO_BUF, "Error(1); app_load doesn't match with configuration\n");
	   io_printf(IO_BUF, "1-(1) %d, (1) %d\n",topology_map[srcIndex].x,my_x);  
	   io_printf(IO_BUF, "1-(2) %d, (2) %d\n",topology_map[srcIndex].y,my_y);  
	   io_printf(IO_BUF, "1-(3) %d, (3) %d\n",topology_map[srcIndex].core,coreID);

	   isDest = 0;	   
	   isSource = 1;
	   
	   io_printf(IO_BUF, "init passed;(1) srcIndex [%d],srcString [%s]\n",srcIndex,topology_map[srcIndex].sId);    
    }
#if 0	
	else
	{		
	   io_printf(IO_BUF, "Error(1); app_load doesn't match with configuration\n");
	   return;
	}
#endif	
  }
  else
  {	   
    io_printf(IO_BUF, "Error(1); Index not implemented\n");
    return;
  }

  if(match_sID(dest_inner_sId,&desIndex))
  {
    if((topology_map[desIndex].x == my_x) && 
	   (topology_map[desIndex].y == my_y) &&
	   (topology_map[desIndex].core == coreID))
    {
	   //io_printf(IO_BUF, "Error(2); app_load doesn't match with configuration\n");
	   io_printf(IO_BUF, "2-(1) %d, (1) %d\n",topology_map[desIndex].x,my_x);  
	   io_printf(IO_BUF, "2-(2) %d, (2) %d\n",topology_map[desIndex].y,my_y);  
	   io_printf(IO_BUF, "2-(3) %d, (3) %d\n",topology_map[desIndex].core,coreID);
	   
	   isDest = 1;	   
	   isSource = 0;
	   
	   io_printf(IO_BUF, "init passed(2); desIndex [%d],destString [%s]\n",desIndex,topology_map[desIndex].sId);
    }
#if 0	
	else
	{
	   io_printf(IO_BUF, "Error(2); app_load doesn't match with configuration\n");
	   return;
	}
#endif	
  }
  else
  {	   
	io_printf(IO_BUF, "Error(2); Index not implemented\n");
    return;
  }
#endif

  // set the core map for the simulation
  //spin1_application_core_map(NUMBER_OF_XCHIPS, NUMBER_OF_YCHIPS, core_map);

  spin1_set_timer_tick(TIMER_TICK_PERIOD);
  
  // register callbacks
  spin1_callback_on(MC_PACKET_RECEIVED, receive_data, 0);
  spin1_callback_on(TIMER_TICK, update, 0);

  spin1_callback_on(SDP_PACKET_RX, host_data, 0);

/* Test TImer */
#if 0
  uint before_time_clock_ms,after_time_clock_ms;  
  uint before_time_ms,after_time_ms;
  uint before_simulation_tick,after_simulation_tick;
  
  before_time_clock_ms = sv->clock_ms;
  before_time_ms = sv->time_ms;
  before_simulation_tick = spin1_get_simulation_time();

  //manualDelay(1000000);
  spin1_delay_us(3000000);
	
  after_time_clock_ms = sv->clock_ms;  
  after_time_ms = sv->time_ms;  
  after_simulation_tick = spin1_get_simulation_time();
  
  io_printf(IO_BUF, "1:Basic Time Test: Delta %d\n",after_time_clock_ms - before_time_clock_ms);  
  io_printf(IO_BUF, "2:Basic Time Test: Delta %d\n",after_time_clock_ms - before_time_clock_ms);
  io_printf(IO_BUF, "3:Basic Time Test: Delta %d\n",after_simulation_tick - before_simulation_tick);
#endif

#if TEST_TIMER_GRANULARITY
	manualDelay(1000000);
#endif

// Initialize routing tables
  if(isSource)
  {
  	// Initialize SDP message buffer
  	sdp_init();	
  	io_printf(IO_BUF, "Starting sdp.\n");
  }
  // go
  
  start_timer();
  spin1_start(FALSE);

  io_printf(IO_BUF, "App exit...........\n");
}



