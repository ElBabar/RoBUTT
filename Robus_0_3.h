#ifndef Robus.h
#define Robus.h


bool is_xbee_available = 0;
unsigned int xbee_crash_count = 0;

struct type_ID {
	char system;
	char member;
}

struct type_frame{
	type_ID ID;
	char[8] data;
}


class Robus{
	public :
		void xbee_checkup(void);
		type_frame xbee_listen(void); 	//returns collected frame if present; else 0
		int xbee_write();				//returns 1 if successful; else 0 (xbee busy)
		type_frame can_listen(void);	//returns collected frame if present; else 0
		void can_write(type_frame);		//always successful
		char serial_listen(void);		//returns char received on Serial
		void serial_write(char);		//always successful
		int init(int xbee_serial_tx, int xbee_serial_rx, int can_spi_mosi, int can_spi_miso, int can_spi_cs);	//returns 1 if successful; else 0
		int get_work_mode(void);		//returns working mode - 0:XBee to serial 1:XBee to CAN 2:Programmer

	private :
		void setup_xbee_serial_port(int xbee_serial_tx, int xbee_serial_rx);
		void setup_can_spi_port(int can_spi_mosi, int can_spi_miso, int can_spi_cs);
		void init_ID(void);
		void init_channel(void);
		void init_work_mode(void);

		void at_open(void);
		void at_close(void);
		void at_rst(void);
		//Programmer functions :
		void at_set_channel(int channel_mode);
		int at_get_channel(void);
		void at_set_conf_time(int conf_time);
		int at_get_conf_time(void);
		void at_set_api_mode(int api_mode);
		int at_get_api_mode(void);
		void at_set_retries_broadcast(int retries);
		int at_get_retries_broadcast(void);
		void at_set_baudrate(int baudrate);
}