class ItemVariableFlags
{
	static const int NONE 					= 0;
	static const int FLOAT			 		= 0x0001;
	static const int STRING		 			= 0x0002;
	static const int BOOL 					= 0x0004;
};

class CachedObjectsParams
{	
	static ref Param1<int> 		PARAM1_INT;//CachedObjectsParams.PARAM1_INT
	static ref Param1<bool> 	PARAM1_BOOL;//CachedObjectsParams.PARAM1_BOOL
	static ref Param1<float> 	PARAM1_FLOAT;//CachedObjectsParams.PARAM1_FLOAT
	static ref Param1<string> 	PARAM1_STRING;//CachedObjectsParams.PARAM1_STRING
	
	static ref Param2<int,int> 		PARAM2_INT_INT;//CachedObjectsParams.PARAM2_INT_INT
	static ref Param2<int,float> 	PARAM2_INT_FLOAT;//CachedObjectsParams.PARAM2_INT_FLOAT
	static ref Param2<int,string> 	PARAM2_INT_STRING;//CachedObjectsParams.PARAM2_INT_STRING
	static ref Param2<string,float> PARAM2_STRING_FLOAT;//CachedObjectsParams.PARAM2_STRING_FLOAT
	static ref Param2<string,string> PARAM2_STRING_STRING;//CachedObjectsParams.PARAM2_STRING_STRING
	static ref Param2<float,float> 	PARAM2_FLOAT_FLOAT;//CachedObjectsParams.PARAM2_STRING_STRING
	
	static void Init()
	{
		PARAM1_INT = new Param1<int>(0);
		PARAM1_BOOL = new Param1<bool>(false);
		PARAM1_FLOAT = new Param1<float>(0);
		PARAM1_STRING = new Param1<string>("");
		
		PARAM2_INT_INT = new Param2<int,int>(0,0);
		PARAM2_INT_FLOAT = new Param2<int,float>(0,0);
		PARAM2_INT_STRING = new Param2<int,string>(0,"");
		PARAM2_STRING_FLOAT = new Param2<string,float>("",0);
		PARAM2_STRING_STRING = new Param2<string,string>("","");
		PARAM2_FLOAT_FLOAT = new Param2<float,float>(0,0);
	}
};


class CachedObjectsArrays//don't forget to .Clear() your cache object before using it
{	
	static void Init()
	{
		ARRAY_STRING 	= new TStringArray;
		ARRAY_FLOAT 	= new TFloatArray;
		ARRAY_INT 		= new TIntArray;
	}
	
	static ref TStringArray ARRAY_STRING;//CachedObjectsArrays.ARRAY_STRING
	static ref TFloatArray ARRAY_FLOAT;//CachedObjectsArrays.ARRAY_FLOAT
	static ref TIntArray ARRAY_INT;//CachedObjectsArrays.ARRAY_INT
};