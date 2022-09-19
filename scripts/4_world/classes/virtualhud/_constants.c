enum eBadgeLevel
{
	NONE,
	FIRST,
	SECOND,
	THIRD,
}

enum eDisplayElements
{
	DELM_BADGE_STUFFED,
	DELM_BADGE_WET,
	DELM_BADGE_SICK,
	DELM_BADGE_POISONED,
	DELM_BADGE_FRACTURE,
	DELM_BADGE_BLEEDING,
	DELM_BADGE_PILLS,
	DELM_BADGE_HEARTBEAT,
	//------------------
	DELM_TDCY_HEALTH,
	DELM_TDCY_BLOOD,
	DELM_TDCY_TEMPERATURE,
	DELM_TDCY_ENERGY,
	DELM_TDCY_WATER,
	DELM_TDCY_BACTERIA,
	//------------------
	DELM_STANCE,
	COUNT
}

//constants related to ui
//key constants
const int	NTFKEY_THIRSTY	= 1;
const int	NTFKEY_HUNGRY	= 2;
const int	NTFKEY_WARMTH	= 3;
const int	NTFKEY_WETNESS	= 4;
const int	NTFKEY_FRACTURE	= 5;
const int	NTFKEY_HEALTHY	= 6;
const int	NTFKEY_FEVERISH	= 7;
const int	NTFKEY_SICK		= 8;
const int	NTFKEY_STUFFED	= 9;
const int	NTFKEY_BLEEDISH	= 10;
const int	NTFKEY_BLOOD	= 11;
const int	NTFKEY_LIVES	= 12;
const int	NTFKEY_BACTERIA	= 13;
const int	NTFKEY_POISONED	= 14;
const int	NTFKEY_PILLS	= 15;
const int	NTFKEY_HEARTBEAT = 16;

//tendency 
const int	TENDENCY_STABLE 	= 0;
const int	TENDENCY_INC_LOW	= 1;
const int	TENDENCY_INC_MED	= 2;
const int	TENDENCY_INC_HIGH	= 3;
const int	TENDENCY_DEC_LOW 	= 4;
const int	TENDENCY_DEC_MED 	= 5;
const int	TENDENCY_DEC_HIGH	= 6;

