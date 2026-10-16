class Land_House_2M1 : BuildingWithFireplaceNasdara
{
	override void OnDebugSpawn()
	{
		//TODO: spawn house rebuilded
	}
};

class Land_House_2M1_Dam2 : BuildingWithFireplaceNasdara
{
	override void OnDebugSpawn()
	{
		ConstructionBase con = ConstructionBase.Cast(GetConstructionBasic());
		
		con.BuildPartServerEx(null,"rubble_rubble1",-1);
		con.BuildPartServerEx(null,"rubble_rubble2",-1);
		con.BuildPartServerEx(null,"wall1_wood",-1);
		con.BuildPartServerEx(null,"wall2_wood",-1);
		con.BuildPartServerEx(null,"wall3_brick",-1);
		con.BuildPartServerEx(null,"roof1",-1);
	}
};
