//Robus 0.2


/*how it should work :
 - set xbee and can ports
 - set own ID and channel with switch data
 - define working mode depending on switch :
 	Serial to XBee
 	CAN to XBee
 	Programmer
 - loop functions are called continuously
 - if data received, send to the right bus
 - check XBee health continuously
*/


//variables :
is_xbee_available;
xbee_crash_count;


//structures :
struct frame	//ID + data
struct ID 		//myID


//setup functions :
setup_xbee_serial_port;
setup_can_SPI_port;
init_ID;
init_channel;
init_workMode;
at_open;
at_close;
at_rst;
at_set_channel;
at_get_channel;
at_set_conf_time;
at_get_conf_time;
at_set_api_mode;
at_get_api_mode;
at_set_retries_broadcast;
at_get_retries_broadcast;
at_set_baudrate;


//loop functions :
xbee_checkup;
xbee_listen;
xbee_write;
can_listen;
can_write;
serial_listen;
serial_write;