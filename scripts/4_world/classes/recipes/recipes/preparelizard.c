class PrepareLizard extends PrepareAnimal
{
    override void Init()
    {
        super.Init();

        //----------------------------------------------------------------------------------------------------------------------
        //ingredient 1
        InsertIngredient(0,"DeadLizard", DayZPlayerConstants.CMD_ACTIONFB_ANIMALSKINNING, false);//you can insert multiple ingredients this way

        m_IngredientAddHealth[0] = 0;// 0 = do nothing
        m_IngredientSetHealth[0] = -1; // -1 = do nothing
        m_IngredientAddQuantity[0] = 0;// 0 = do nothing
        m_IngredientDestroy[0] = true;//true = destroy, false = do nothing

        //----------------------------------------------------------------------------------------------------------------------
        //result1
        AddResult("Bone");//add results here
        m_ResultSetFullQuantity[0] = false; //true = set full quantity, false = do nothing
        m_ResultSetQuantity[0] = -1; // -1 = do nothing
        m_ResultSetHealth[0] = -1; // -1 = do nothing
        m_ResultInheritsHealth[0] = 0;
        m_ResultInheritsColor[0] = -1;
        m_ResultToInventory[0] = -2;
        m_ResultReplacesIngredient[0] = 0;

        //result2
        AddResult("LizardPelt");//add results here
        m_ResultSetFullQuantity[1] = false; //true = set full quantity, false = do nothing
        m_ResultSetQuantity[1] = -1; // -1 = do nothing
        m_ResultSetHealth[1] = -1; // -1 = do nothing
        m_ResultInheritsHealth[1] = 0;
        m_ResultInheritsColor[1] = -1;
        m_ResultToInventory[1] = -2;
        m_ResultReplacesIngredient[1] = 0;
    }

    override void Do(ItemBase ingredients[], PlayerBase player, array<ItemBase> results, float specialty_weight)//gets called upon recipe's completion
    {
        ItemBase deadLizard = ingredients[0];
        ItemBase resultBones = results[0];

        ItemBase result = ItemBase.Cast(player.SpawnEntityOnGroundRaycastDispersed("LizardMeat"));
        MiscGameplayFunctions.TransferItemProperties(deadLizard, result);
        result.SetQuantityNormalized(Math.RandomFloatInclusive(0.8, 1));

        MiscGameplayFunctions.TransferItemProperties(deadLizard, resultBones);
        resultBones.SetQuantity(Math.RandomIntInclusive(2, 5));

        SetBloodyHands(ingredients, player);
    }
}