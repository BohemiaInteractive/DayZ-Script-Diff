class QuantityConversions
{
	static string GetItemQuantityText( EntityAI item, bool showMax = false )
	{
		string quantity_text = "";
		if ( item.IsInherited( InventoryItem) )
		{
			ItemBase item_base;
			Class.CastTo(item_base, item);
			float quantity = item_base.GetQuantity();
			int ammo;
			if ( item.IsMagazine() )
			{
				Magazine magazine_item;
				Class.CastTo(magazine_item, item);
				ammo = magazine_item.GetAmmoCount();

				return ammo.ToString();
			}
			else if ( item.IsInherited( ItemBook) )
			{
				return "";
			}
			int stack_max = item.GetQuantityMax();
			
			//int max = item.ConfigGetInt("varQuantityMax");
			//string unit = item.ConfigGetString("stackedUnit");

			if (stack_max > 0)
			{
				if (stack_max == 1)
				{
					if (quantity > 1)
					{
						if (showMax)
							quantity_text = string.Format("%1/%2", quantity.ToString(), stack_max.ToString() );
						//quantity_text = string.Format("%i/%i", quantity, stack_max );
						else
							quantity_text = quantity.ToString();
					}
					else
					{
						quantity_text = "";
					}
				}
				else
				{
					if (showMax)
						quantity_text = string.Format("%1/%2", quantity.ToString(), stack_max.ToString() );
						//quantity_text = string.Format("%i/%i", quantity, stack_max );
					else
						quantity_text = quantity.ToString();
					// if (unit == "ml")
					// {
					// 	float liters = round(quantity / 1000.0);
					// 	if ( quantity < 2000 )
					// 	{
					// 		liters =  ( round( (quantity / 100.0) ) ) / 10;
					// 	}
					// 	quantity_text = ftoa(liters) + "l";
					// }
					// else
					// {
					// 	quantity_text = itoa(quantity) + unit;
					// }
				}
			}
		}
		return quantity_text;
	}	

	static float GetItemQuantity(InventoryItem item)
	{
		float quantity = 0;
		if ( item.IsInherited(InventoryItem))
		{
			ItemBase item_base;
			Class.CastTo(item_base, item);
			if (item_base)
			{
				if (item.IsMagazine())
				{
					Magazine magazine_item;
					Class.CastTo(magazine_item, item);
					quantity = magazine_item.GetAmmoCount();
				}
				else
				{
					quantity = item_base.GetQuantity();
				}
			}
		}
		return quantity;
	}	
	
	static float GetItemQuantityMax(InventoryItem item)
	{
		float quantity = 0;
		if (item.IsInherited(InventoryItem))
		{
			ItemBase item_base;
			Class.CastTo(item_base, item);
			if (item_base)
			{
				if (item.IsMagazine())
				{
					Magazine magazine_item;
					Class.CastTo(magazine_item, item);
					quantity = magazine_item.GetAmmoMax();
				}
				else
				{
					quantity = item_base.GetQuantityMax();
				}
			}
		}
		return quantity;
	}
	
	static void GetItemQuantity( InventoryItem item, out float q_cur, out float q_min, out float q_max )
	{
		if ( item.IsInherited( InventoryItem ) )
		{
			ItemBase item_base;
			Class.CastTo(item_base, item);
			if ( item.IsMagazine() )
			{
				Magazine magazine_item;
				Class.CastTo(magazine_item, item);
				q_cur = magazine_item.GetAmmoCount();
				q_min = 0;
				q_max = magazine_item.GetAmmoMax();
			}
			else
			{
				q_cur = item_base.GetQuantity();
				q_min = item_base.GetQuantityMin();
				q_max = item_base.GetQuantityMax();
			}
		}
	}

	static int HasItemQuantity( notnull EntityAI item )
	{
		ItemBase ib;
		if ( Class.CastTo(ib, item) )
		{
			if ( item.IsMagazine() )
				return QUANTITY_COUNT;
			
			if ( !ib.m_CanShowQuantity )
				return QUANTITY_HIDDEN;
			
			int max = item.GetQuantityMax();
			if ( max > 0 )
			{
				if ( ib.m_HasQuantityBar )
				{
					return QUANTITY_PROGRESS;
				}
				else
				{
					if (max == 1)
					{
						return QUANTITY_HIDDEN;
					}
					else
					{
						return QUANTITY_COUNT;
					}
				}
			}
		}
		return QUANTITY_HIDDEN;
	}	
}