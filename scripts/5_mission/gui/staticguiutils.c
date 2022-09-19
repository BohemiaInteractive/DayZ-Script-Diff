class StaticGUIUtils
{
	static const int IMAGESETGROUP_INVENTORY = 0;
	
	
	//! Checks for improperly formated, legacy image names and corrects them to default format.
	static string VerifyIconImageString(int imageset_group = IMAGESETGROUP_INVENTORY, string icon_name = "")
	{
		if (icon_name == "")
		{
			return "set:dayz_inventory image:missing";
		}
		
		if ( !icon_name.Contains("image:") )
		{
			switch (imageset_group)
			{
				case IMAGESETGROUP_INVENTORY:
					return "set:dayz_inventory image:" + icon_name;
			}
			
		}
		return icon_name;
	}
}