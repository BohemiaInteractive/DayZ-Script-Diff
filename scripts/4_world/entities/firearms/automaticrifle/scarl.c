class SCARL_Base : SCARH_Base
{
    
    override RecoilBase SpawnRecoilObject()
    {
        return new SCARLRecoil(this);
    }
	
	override void OnDebugSpawn()
    {
        GameInventory inventory = GetInventory();

        inventory.CreateInInventory( "SCAR_StockBttstck" );
        inventory.CreateInInventory( "M4_T3NRDSOptic" );
        inventory.CreateInInventory( "Battery9V" );

        SpawnAttachedMagazine("Mag_STANAG_30Rnd");
    }
};