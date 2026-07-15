class SCARH_Base : RifleBoltLock_Base
{
    override void EEInit()
    {
        super.EEInit();
    }

    void InitFoldingSight()
    {

        if (!FindAttachmentBySlotName("Scarh_FrontSight"))
        {
            if (GetGame().IsServer() || !GetGame().IsMultiplayer())
            {
                GetInventory().CreateAttachment("Scarh_Front_Sight");
            }
        }

        UpdateFoldingSightState();
        ForceSyncSelectionState();
        SetAttachmentsHealth();
    }

    override void EEItemAttached(EntityAI item, string slot_name)
    {
        super.EEItemAttached(item, slot_name);

        UpdateFoldingSightState();
    }

    override void EEItemDetached(EntityAI item, string slot_name)
    {
        super.EEItemDetached(item, slot_name);

        UpdateFoldingSightState();
    }

    override void EEHealthLevelChanged(int oldLevel, int newLevel, string zone)
    {
        super.EEHealthLevelChanged(oldLevel, newLevel, zone);

        if (g_Game.IsClient())
            return;

        SetAttachmentsHealth();
    }

    void SetAttachmentsHealth()
    {
        EntityAI entity;

        entity = FindAttachmentBySlotName("Scarh_FrontSight");
        if (entity)
        {
            entity.SetHealth01("", "Health", GetHealth01());
        }
    }

    void UpdateFoldingSightState()
    {
        EntityAI optic = FindAttachmentBySlotName("weaponOptics");

        if (optic && !optic.IsKindOf("BUISOptic"))
        {
            SetAnimationPhase("opticAttached", 1.0);
        }
        else
        {
            SetAnimationPhase("opticAttached", 0.0);
        }
    }


    override RecoilBase SpawnRecoilObject()
    {
        return new SCARHRecoil(this);
    }

    override void AssembleGun()
    {
        super.AssembleGun();

        InitFoldingSight();
    }

    override void OnDebugSpawn()
    {
        GameInventory inventory = GetInventory();

        inventory.CreateInInventory( "SCAR_StockBttstck" );
        inventory.CreateInInventory( "M4_T3NRDSOptic" );
        inventory.CreateInInventory( "Battery9V" );

        SpawnAttachedMagazine("Mag_SCARH_20Rnd");
    }
};

class Scarh_Front_Sight extends DummyItem {};