class InventoryCombinationFlags
{
	static const int NONE 					= 0;
	static const int ADD_AS_ATTACHMENT 		= 1;
	static const int ADD_AS_CARGO 			= 2;
	static const int SWAP 					= 4;
	static const int FSWAP					= 8;
	static const int CRAFT					= 16;
	static const int ACTIONS				= 32;
	static const int SWAP_MAGAZINE			= 64;
	static const int TAKE_TO_HANDS			= 128;
	static const int RECIPE_HANDS			= 256;
	static const int COMBINE_QUANTITY2		= 512;
	static const int RECIPE_ANYWHERE		= 1024;
	static const int PERFORM_ACTION			= 2048;
	static const int LOAD_CHAMBER			= 4096;
	static const int DETACH_MAGAZINE		= 8192;
	static const int ATTACH_MAGAZINE		= 16384;
	static const int SET_ACTION				= 32768;
	//static const int SWAP_MAGAZINE			= 65536;	
}

class InventoryManipulationFlags
{
	static const int NONE 					= 0;
	static const int ADD_AS_ATTACHMENT 		= 1;
	static const int ADD_AS_CARGO 			= 2;
	static const int SWAP					= 4;
	static const int FSWAP					= 8;
	static const int TAKE_TO_HANDS			= 16;
	static const int DROP					= 32;
	static const int PERFORM_ACTION			= 64;
	static const int SET_ACTION				= 128;
	static const int COMBINE_QUANTITY		= 512;
	
	
	//static const int LOAD_CHAMBER			= 4096;
	//static const int DETACH_MAGAZINE		= 8192;
	//static const int ATTACH_MAGAZINE		= 16384;
	
	/*static const int RECIPE_HANDS			= 256;
	static const int COMBINE_QUANTITY2		= 512;
	static const int RECIPE_ANYWHERE		= 1024;
	static const int PERFORM_ACTION			= 2048;
	static const int LOAD_CHAMBER			= 4096;
	static const int DETACH_MAGAZINE		= 8192;
	static const int ATTACH_MAGAZINE		= 16384;
	static const int SET_ACTION				= 32768;*/
	//static const int SWAP_MAGAZINE			= 65536;	
}
