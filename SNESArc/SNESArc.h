/*Arc System Works*/
#define ramSize 65536

/*Very early version (Battle Commander)*/
#define VER_BC			0
/*Early version (Cyber Spin - aka Shinseiki GPX Cyber Formula in Japan.)*/
#define VER_SHINGPX		1
/*Later version (Sailor Moon)*/
#define VER_NORMAL		2
/*Modified later version by Sun-L (Bonkers)*/
#define VER_SUNL		3

/*Based on names from VGMTrans fork*/
enum EventTypes
{
	EVENT_UNKNOWN0 = 1,
	EVENT_NOP,
	EVENT_UNKNOWN1,
	EVENT_UNKNOWN2,
	EVENT_REST,
	EVENT_TEMPO,
	EVENT_PITCH_LFO,
	EVENT_TUNING,
	EVENT_ADSR,
	EVENT_JUMP,
	EVENT_TIE,
	EVENT_SUBROUTINE_END,
	EVENT_SUBROUTINE,
	EVENT_PROG_CHANGE,
	EVENT_END,
	EVENT_VOLUME,
	EVENT_REPEAT,
	EVENT_TRANSPOSE,
	EVENT_ECHO_ON,
	EVENT_ECHO_PARAM,
	EVENT_ECHO_OFF,
	EVENT_NOISE_ADSR,
	EVENT_NOISE_OFF,
	EVENT_PROGADD,
	EVENT_VIBRATO_OFF,
	EVENT_MASTER_VOLUME,
	EVENT_PITCH_SLIDE,
	EVENT_VOLENV,
	EVENT_REST_NODUR,
};

unsigned int STATUS_NOTE_MIN;
unsigned int STATUS_NOTE_MAX;
unsigned int STATUS_PERCUSSION_NOTE_MIN;
unsigned int STATUS_PERCUSSION_NOTE_MAX;
unsigned int STATUS_NOTE_NODUR_MIN;
unsigned int STATUS_NOTE_NODUR_MAX;

unsigned int EventMap[256];
