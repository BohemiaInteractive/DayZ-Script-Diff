//! PPE type priorities, C++ based. DO NOT CHANGE ORDER! Used only when calling 'SetCameraPostProcessEffect'
enum PostProcessPrioritiesCamera
{
	PPP_SSAO,
	PPP_CLOUDS,
	PPP_DOF,
	PPP_ROTBLUR,
	PPP_GODRAYS,
	PPP_RAIN,
	PPP_RADIALBLUR,
	PPP_CHROMABER,
	PPP_WETDISTORT,
	PPP_DYNBLUR,
	PPP_UNDERWATER,
	PPP_DOF_BOKEH,
	PPP_COLORGRADE,
	PPP_GLOW,
	PPP_FILMGRAIN,
	PPP_FILMGRAIN_NV,
	PPP_FXAA,
	PPP_SMAA,
	PPP_GAUSS_FILTER,
	PPP_MEDIAN, //unused?
	//PPP_SSR
};

enum PPERequesterCategory
{
	NONE = 0;
	GAMEPLAY_EFFECTS = 2;
	MENU_EFFECTS = 4;
	MISC_EFFECTS = 8;
	ALL = 14; //GAMEPLAY_EFFECTS|MENU_EFFECTS|MISC_EFFECTS
};

/** 
/brief IDs of custom PPE classes 
/note Currently used for various native exceptions that used to be handled outside of script-side postprocess system.
/note Can be used for custom functionality as well, C++ permitting.
*/
enum PPEExceptions
{
	NONE = -1,
	EXPOSURE = 50,
	DOF,
	EYEACCOM,
	NVLIGHTPARAMS,
};

//! PP operator masks specify operation between subsequent layers
enum PPOperators
{
	LOWEST, //only lowest value gets used. Note - if first request, it is compared against default values!
	HIGHEST, //only highest value gets used. Note - if first request, it is compared against default values!
	ADD, //LINEAR addition
	ADD_RELATIVE, //LINEAR relative addition (relative to diff between current and max, where applicable. Otherwise used as absolute addition)
	SUBSTRACT, //LINEAR substraction
	SUBSTRACT_RELATIVE, //LINEAR relative substraction
	SUBSTRACT_REVERSE, //LINEAR sub. target from dst
	SUBSTRACT_REVERSE_RELATIVE, //LINEAR relative sub. target from dst
	MULTIPLICATIVE, //LINEAR multiplication
	SET, //sets the value, does not terminate possible further calculations
	OVERRIDE //does not interact; sets the value, and terminates possible further calculations. Use with care, preferred use is SET with higher priority command
};

class PPEConstants
{
	static const int VAR_TYPE_BOOL = 1;
	static const int VAR_TYPE_INT = 2;
	static const int VAR_TYPE_FLOAT = 4;
	static const int VAR_TYPE_COLOR = 8;
	static const int VAR_TYPE_VECTOR = 16;
	
	static const int VAR_TYPE_BLENDABLE = VAR_TYPE_INT|VAR_TYPE_FLOAT|VAR_TYPE_COLOR;
	static const int VAR_TYPE_TEXTURE = 32;
	static const int VAR_TYPE_RESOURCE = 64;
	
	//static const int VALUE_MAX_COLOR = 255; //if only...
	
	static const int DEPENDENCY_ORDER_BASE = 0;
	static const int DEPENDENCY_ORDER_HIGHEST = 1; //!< number of potential recursive update cycles in case of parameter dependency
}

typedef Param2<string,bool> PPETemplateDefBool;
typedef Param4<string,int,int,int> PPETemplateDefInt;
typedef Param4<string,float,float,float> PPETemplateDefFloat; //name, default, min, max
typedef Param5<string,float,float,float,float> PPETemplateDefColor; //name, defaults - floats. Min/Max is always the same, no need to define it here.
//typedef Param4<string,vector,vector,vector> PPETemplateDefVector;
typedef Param2<string,ref array<float>> PPETemplateDefVector; //needs to be compatible with every type of vector (vector2 to vector4), hence array<float>...
typedef Param2<string,string> PPETemplateDefTexture; //Currently unused, setting these parameters during runtime can prove problematic
typedef Param2<string,string> PPETemplateDefResource; //Currently unused, setting these parameters during runtime can prove problematic
