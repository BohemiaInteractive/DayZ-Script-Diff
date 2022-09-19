class BleedingIndicationConstants
{
//--------------------------------------------------------------
//indicator-level constants
	static const int 	INDICATOR_SEVERITY_LOW = 0;
	static const int 	INDICATOR_SEVERITY_MEDIUM = 1;
	static const int 	INDICATOR_SEVERITY_HIGH = 2;
	static const int 	SEQUENCE_STEPS = 10; //! Used to pre-gen poisson probabilities, I REALLY wouldn't go over 10 here. Values above 12 cause overflow!
	static const float 	SEQUENCE_DURATION_DEFAULT = 1.0;
	
//--------------------------------------------------------------
//shared drop-level constants
	static const float 	DROP_PROGRESS_THRESHOLD = 0.2; //[0..1]
	static const float 	DROP_COLOR_DESATURATIONEND = 0.75; //[0..1]
	static const int 	DROP_COLOR_RED = 0x9f;
	static const int 	DROP_COLOR_GREEN = 0x3f;
	static const int 	DROP_COLOR_BLUE = 0x3f;
	static const int 	DROP_COLOR_ALPHA_START = 0xff;
	static const int 	DROP_COLOR_ALPHA_END = 0x00;
	
//--------------------------------------------------------------
//sorted severity constants
	//Low
	static const float 	SEQUENCE_DURATION_LOW = 1.0; //seconds
	static const float 	SEQUENCE_DROP_AVERAGE_LOW = 1.5; //used to pre-gen probabilities on game start. Values above 5 can break the poisson! DO NOT touch unless you know what you're doing.
	static const float 	DROP_DURATION_LOW = 1.5; //seconds
	static const float 	SEQUENCE_DROP_DELAY_MIN_LOW = DROP_DURATION_LOW / 5; //min delay to spawn a drop
	static const float 	SEQUENCE_DROP_DELAY_MAX_LOW = 2.5 * DROP_DURATION_LOW; //max delay between drops
	static const float 	DROP_SIZE_START_LOW = 0.62;
	static const float 	DROP_SIZE_END_LOW = 0.70;
	static const float 	DROP_SIZE_VARIATION_MIN_LOW = 1.0; //randomizes every drop's coef min
	static const float 	DROP_SIZE_VARIATION_MAX_LOW = 1.0; //randomizes every drop's coef max
	static const int 	DROP_SCATTER_LOW = 25; //pixels
	static const float 	DROP_SLIDE_DISTANCE_LOW = 4;
	
	//Medium
	static const float 	SEQUENCE_DURATION_MEDIUM = 0.6; //seconds
	static const float 	SEQUENCE_DROP_AVERAGE_MEDIUM = 2.0; //used to pre-gen probabilities on game start. Values above 5 can break the poisson! DO NOT touch unless you know what you're doing.
	static const float 	DROP_DURATION_MEDIUM = 1.5; //seconds
	static const float 	SEQUENCE_DROP_DELAY_MIN_MEDIUM = DROP_DURATION_MEDIUM / 5; //min delay to spawn a drop
	static const float 	SEQUENCE_DROP_DELAY_MAX_MEDIUM = 2 * DROP_DURATION_MEDIUM; //max delay between drops
	static const float 	DROP_SIZE_START_MEDIUM = 0.76;
	static const float 	DROP_SIZE_END_MEDIUM = 0.83;
	static const float 	DROP_SIZE_VARIATION_MIN_MEDIUM = 1.0; //randomizes every drop's coef min
	static const float 	DROP_SIZE_VARIATION_MAX_MEDIUM = 1.0; //randomizes every drop's coef max
	static const int 	DROP_SCATTER_MEDIUM = 30; //pixels
	static const float 	DROP_SLIDE_DISTANCE_MEDIUM = 6;
	
	//High
	static const float 	SEQUENCE_DURATION_HIGH = 0.4; //seconds
	static const float 	SEQUENCE_DROP_AVERAGE_HIGH = 2.5; //used to pre-gen probabilities on game start. Values above 5 can break the poisson! DO NOT touch unless you know what you're doing.
	static const float 	DROP_DURATION_HIGH = 1.5; //seconds
	static const float 	SEQUENCE_DROP_DELAY_MIN_HIGH = DROP_DURATION_HIGH / 5; //min delay to spawn a drop
	static const float 	SEQUENCE_DROP_DELAY_MAX_HIGH = 1.5 * DROP_DURATION_HIGH; //max delay between drops
	static const float 	DROP_SIZE_START_HIGH = 0.92;
	static const float 	DROP_SIZE_END_HIGH = 1.00;
	static const float 	DROP_SIZE_VARIATION_MIN_HIGH = 1.0; //randomizes every drop's coef min
	static const float 	DROP_SIZE_VARIATION_MAX_HIGH = 1.2; //randomizes every drop's coef max
	static const int 	DROP_SCATTER_HIGH = 35; //pixels
	static const float 	DROP_SLIDE_DISTANCE_HIGH = 8;
}
