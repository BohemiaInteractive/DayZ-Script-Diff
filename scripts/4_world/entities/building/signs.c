class Land_Bilboard_Base: House
{
	void Land_Bilboard_Base()
	{
		#ifndef SERVER
		if (g_Game.GetBillboardHandler())
		{
			string texture = g_Game.GetBillboardHandler().GetTextureByType(GetType());
			if (texture)
			{
				int index = GetHiddenSelectionIndex("advert");
				if (index != -1)
					SetObjectTexture(index, texture);	
			}
		}
		#endif
	}
}