class CarWheelType : ItemBaseType
{
	string m_RuinedType;
	string m_NormalType;
	
	void CarWheelType()
	{
		SetReplacementTypes();
	}
	
	//! Note: As each scripted base class creates a CarWheelType instance of itself we want to skip these when we set the wheel replacement type names
	TStringArray GetExcludedReplacementTypes()
	{
		return {"CarWheel", "CarWheel_Ruined", "MotorbikeWheel", "MotorbikeWheel_Ruined"};
	}
	
	protected void SetReplacementTypes()
	{
		string name = GetName();
		if (GetExcludedReplacementTypes().Find(name) == -1)
		{
			if (name.Contains("_Ruined"))
			{
				m_RuinedType = name;
				name.Replace("_Ruined", "");
				m_NormalType = name;
			}
			else
			{
				m_NormalType = name;
				m_RuinedType = name + "_Ruined";
			}
			
			if (!g_Game.ConfigIsExisting(CFG_VEHICLESPATH + " " + m_RuinedType))
			{
				ErrorEx("No config class entry for wheel entity '" + m_RuinedType + "'.", ErrorExSeverity.ERROR);
				m_RuinedType = "";
			}
			
			if (!g_Game.ConfigIsExisting(CFG_VEHICLESPATH + " " + m_NormalType))
			{
				ErrorEx("No config class entry for wheel entity '" + m_NormalType + "'.", ErrorExSeverity.ERROR);
				m_NormalType = "";
			}
		}
	}
};

class ClothingType : ItemBaseType
{
};

class ItemWatchType : ItemBaseType
{
};

class ItemRadioType : ItemBaseType
{
};

class ItemTransmitterType : ClothingType
{
};

class ItemMegaphoneType : ItemBaseType
{
};

class ItemCompassType : ItemBaseType
{
};

class ItemGPSType : ItemBaseType
{
};

class ItemMapType : ItemBaseType
{
};

class ItemBookType : ItemBaseType
{
};

class ItemOpticsType : ItemBaseType
{
	/*
		Optics2D or in other words the model provided by `modelOptics` when `useModelOptics` is
		shared for all instances of a `ItemOpticsType`.
		Call the following methods during `ItemOptics.OnDrawOptics2D` to manipulate currently viewed optics.
	*/
	/*!
		\brief Set optics 2D model texture at provided selection index.
		\param index Selection index
		\param textureName Name of texture to set (/dz/.../*.paa)
	*/
	proto native void SetOptics2DTexture(int index, string textureName);
	/*!
		\brief Set optics 2D material at provided selection index.
		\param index Selection index
		\param materialName Name of material to set (/dz/.../*.rvmat)
	*/
	proto native void SetOptics2DMaterial(int index, string materialName);
	/*!
		\brief Finds selection index by name.
		\param selectionName Name of selection to find
		\param ignoreCase Whether search ignores case sensitivity
		\return Index of selection or -1 when none.
	*/
	proto native int FindOptics2DSelection(string selectionName, bool ignoreCase = true);
	/*!
		\brief Whether 2D model (`modelOptics`) is set and used for this type.
	*/
	proto native bool IsUsingOptics2DModel();
};

class ItemBarrelType : ItemBaseType
{
};

class ItemSuppressorType : ItemBaseType
{
};

class ItemGrenadeType : ItemBaseType
{
};

class WeaponType : ItemBaseType
{
};

class UnderSlugLauncherType : WeaponType
{
};

class MagazineType : ItemBaseType
{
};
