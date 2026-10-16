class VehicleVFXData
{
    int m_ParticleId = -1;
    int m_EffectId = -1;

    vector m_Position;
    vector m_Orientation;

    ref EffectParticle m_Effect;
    ref ParticleParamsOverrideData m_ParticleParamsData;

    protected int	m_LastPlayedTime = -1;		//!< g_Game.GetTime() (ms) when this effect last started; -1 = never played
    protected float	m_CooldownSec    = 0;		//!< minimum seconds required between playbacks (0 = no guard)

    void VehicleVFXData()
    {
        m_ParticleParamsData = new ParticleParamsOverrideData();
    }
    
    void SetPosition(vector position)
    {
        m_Position = position;
    }

    // --------------------------------------------------------------------------------

    //! set the cooldown window (in seconds); 0 disables the guard
    void SetCooldown(float seconds)
    {
        m_CooldownSec = seconds;
    }
	
    // --------------------------------------------------------------------------------

    float GetCooldown()
    {
        return m_CooldownSec;
    }
	
    // --------------------------------------------------------------------------------

    //! true if enough time has elapsed since the last playback (or it never played / no cooldown set)
    bool CanPlay()
    {
        if (m_CooldownSec <= 0 || m_LastPlayedTime == -1)
            return true;

        //! g_Game.GetTime() is in milliseconds -> compare against cooldown converted to ms
        return (g_Game.GetTime() - m_LastPlayedTime) >= (int)(m_CooldownSec * 1000);
    }
	
    // --------------------------------------------------------------------------------

    void MarkPlayed()
    {
        m_LastPlayedTime = g_Game.GetTime();
    }
	
    // --------------------------------------------------------------------------------

    float GetRemainingCooldown()
    {
        if (m_CooldownSec <= 0 || m_LastPlayedTime == -1)
            return 0;

        float elapsed = (g_Game.GetTime() - m_LastPlayedTime) * 0.001;
        float remaining = m_CooldownSec - elapsed;
        if (remaining > 0)
			return remaining;
		
		return 0.0;
    }

    // --------------------------------------------------------------------------------

    void ResetCooldown()
    {
        m_LastPlayedTime = -1;
    }
}

class VehicleVFXDataParams
{
    vector m_Position;
    vector m_Orientation;
    float  m_CooldownSec;

    void VehicleVFXDataParams(vector position = vector.Zero, vector orientation = vector.Up, float cooldownSec = 0.0)
    {
        m_Position 		= position;
        m_Orientation 	= orientation;
        m_CooldownSec	= cooldownSec;
    }
}

class VehicleVFXComponent
{
    private bool m_SurfaceInfoOverriden;
    private EntityAI m_Parent;
    private SurfaceInfo m_SurfaceInfo;

    protected ref map<int, ref VehicleVFXData> m_EffectsMap;					//!< keyed by EVehicleVFXEffect
    protected ref map<int, ref array<ref VehicleVFXData>> m_EffectGroupsMap;	//!< keyed by EVehicleVFXEffectGroup

    void VehicleVFXComponent(notnull EntityAI parent)
    {
        m_Parent = parent;

        m_EffectsMap = new map<int, ref VehicleVFXData>();
        m_EffectGroupsMap = new map<int, ref array<ref VehicleVFXData>>();
    }

    // --------------------------------------------------------------------------------

    vector ConvertMemPointToPosition(string memoryPointName)
    {
        if (m_Parent.MemoryPointExists(memoryPointName))
            return m_Parent.GetMemoryPointPos(memoryPointName);

        return vector.Zero;
    }

    // --------------------------------------------------------------------------------

    //! register single instance of effect
    void RegisterEffect(EVehicleVFXEffect effectId, typename effectTypename, VehicleVFXDataParams params)
    {
        if (!m_EffectsMap.Contains(effectId))
            m_EffectsMap.Set(effectId, CreateEffectData(effectTypename, params));
        #ifdef DIAG_DEVELOPER
        else
            ErrorEx(string.Format("Effect '%1' is already registered.", EnumTools.EnumToString(EVehicleVFXEffect, effectId)));
        #endif
    }

    // --------------------------------------------------------------------------------

    //! register multiple instances of single type of effect owned by the given group
    void RegisterEffectGroup(EVehicleVFXEffectGroup groupId, typename effectTypename, int count, VehicleVFXDataParams params)
    {
        if (!m_EffectGroupsMap.Contains(groupId))
        {
            array<ref VehicleVFXData> groupData = new array<ref VehicleVFXData>();
            for (int i = 0; i < count; ++i)
                groupData.Insert(CreateEffectData(effectTypename, params));

            m_EffectGroupsMap.Insert(groupId, groupData);
        }
        #ifdef DIAG_DEVELOPER
        else
            ErrorEx(string.Format("Group for '%1' is already registered", EnumTools.EnumToString(EVehicleVFXEffectGroup, groupId)));
        #endif
    }

    // --------------------------------------------------------------------------------

    protected VehicleVFXData CreateEffectData(typename effectTypename, VehicleVFXDataParams params)
    {
        VehicleVFXData data = new VehicleVFXData();
        data.m_Position 	= params.m_Position;
        data.m_Orientation	= params.m_Orientation;
        data.m_Effect 		= EffectParticle.Cast(effectTypename.Spawn());

        data.SetCooldown(params.m_CooldownSec);

        return data;
    }

    // --------------------------------------------------------------------------------

    protected void PlayEffectData(notnull VehicleVFXData data, EntityAI parent)
    {
        //! cooldown guard - refuse to (re)start until the window elapses
        if (!data.CanPlay())
            return;

        SurfaceInfo surfaceInfo = m_SurfaceInfo;
        if (!m_SurfaceInfoOverriden)
        {
            SurfaceDetectionParameters surfaceParams  = new SurfaceDetectionParameters();
            surfaceParams.syncMode = UseObjectsMode.NoLock;
            SurfaceDetectionResult surfaceResult = new SurfaceDetectionResult();
            surfaceParams.position = parent.GetPosition();
            g_Game.GetSurface(surfaceParams, surfaceResult);
            surfaceInfo = surfaceResult.surface;
        }


        if(data.m_EffectId == -1)
        {
            //! do not play - mostly for dynamicaly created/placed particles
            if (data.m_Position == "-1 -1 -1")
                return;

            data.m_Effect.ParticleParamsOverride(data.m_ParticleParamsData);
            data.m_EffectId = SEffectManager.PlayOnObject(data.m_Effect, parent, data.m_Position, data.m_Orientation);

            //! stamp the playback time so the cooldown guard starts counting
            data.MarkPlayed();
        }
        else
        {
            //! created only - start playback
            if (!data.m_Effect.IsPlaying())
            {
                data.m_Effect.Start();
                data.m_ParticleParamsData.ClearAll();

                //! stamp on restart too
                data.MarkPlayed();
            }

            data.m_Effect.SelectFromSurface(surfaceInfo);

            Particle particle = Particle.Cast(data.m_Effect.GetParticle());
            if (particle)
            {
                data.m_Effect.ParticleParamsOverride(data.m_ParticleParamsData);
            }
        }
    }

    // --------------------------------------------------------------------------------

    protected void StopEffectData(VehicleVFXData data)
    {
        if (data && data.m_Effect)
        {
            data.m_Effect.Stop();
            data.m_EffectId = -1;
            data.m_ParticleParamsData.ClearAll();
        }
    }

    // --------------------------------------------------------------------------------

    protected bool IsEffectDataPlaying(VehicleVFXData data)
    {
        if (data && data.m_Effect)
            return data.m_Effect.IsPlaying();

        return false;
    }

    // --------------------------------------------------------------------------------

    protected bool CanPlayEffectData(VehicleVFXData data)
    {
        if (data)
            return data.CanPlay();

        return false;
    }

    // --------------------------------------------------------------------------------

    protected void SetEffectDataPosition(VehicleVFXData data, vector position)
    {
        if (data && data.m_Effect)
        {
            data.m_Position = position;
            data.m_Effect.SetCurrentLocalPosition(position);
        }
    }

    // --------------------------------------------------------------------------------
    //! single effect API
    // --------------------------------------------------------------------------------

    void PlayEffect(EVehicleVFXEffect effectId, EntityAI parent = null)
    {
        if (!parent)
            parent = m_Parent;

        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (data)
            PlayEffectData(data, parent);
    }

    // --------------------------------------------------------------------------------

    void StopEffect(EVehicleVFXEffect effectId)
    {
        StopEffectData(m_EffectsMap.Get(effectId));
    }

    // --------------------------------------------------------------------------------

    void StopAllEffects()
    {
        foreach (int effectId, VehicleVFXData data : m_EffectsMap)
            StopEffectData(data);

        foreach (int groupId, array<ref VehicleVFXData> groupData : m_EffectGroupsMap)
        {
            foreach (VehicleVFXData groupEffectData : groupData)
                StopEffectData(groupEffectData);
        }
    }

    // --------------------------------------------------------------------------------

    void SetEffectCooldown(EVehicleVFXEffect effectId, float seconds)
    {
        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (data)
            data.SetCooldown(seconds);
        #ifdef DIAG_DEVELOPER
        else
            ErrorEx(string.Format("Cannot set cooldown, effect '%1' does not exist.", EnumTools.EnumToString(EVehicleVFXEffect, effectId)));
        #endif
    }

    // --------------------------------------------------------------------------------

    bool CanPlayEffect(EVehicleVFXEffect effectId)
    {
        return CanPlayEffectData(m_EffectsMap.Get(effectId));
    }

    // --------------------------------------------------------------------------------

    float GetEffectRemainingCooldown(EVehicleVFXEffect effectId)
    {
        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (data)
            return data.GetRemainingCooldown();

        return 0;
    }

    // --------------------------------------------------------------------------------

    void ResetEffectCooldown(EVehicleVFXEffect effectId)
    {
        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (data)
            data.ResetCooldown();
    }

    // --------------------------------------------------------------------------------

    bool IsEffectPlayed(EVehicleVFXEffect effectId)
    {
        return IsEffectDataPlaying(m_EffectsMap.Get(effectId));
    }

    // --------------------------------------------------------------------------------

    void SetEffectPosition(EVehicleVFXEffect effectId, vector position)
    {
        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (!data)
        {
            ErrorEx(string.Format("Effect '%1' does not exist. Isn't it group? Consider use of SetEffectGroupPositions", EnumTools.EnumToString(EVehicleVFXEffect, effectId)));
            return;
        }

        SetEffectDataPosition(data, position);
    }

    // --------------------------------------------------------------------------------

    void SetEffectParticleState(EVehicleVFXEffect effectId, int particleId)
    {
        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (data && data.m_Effect)
            data.m_Effect.SetParticleState(particleId);
    }

    // --------------------------------------------------------------------------------

    ref ParticleParamsOverrideData GetParticleParamsOverrideData(EVehicleVFXEffect effectId)
    {
        VehicleVFXData data = m_EffectsMap.Get(effectId);
        if (data && data.m_ParticleParamsData)
            return data.m_ParticleParamsData;

        #ifdef DIAG_DEVELOPER
        ErrorEx(string.Format("No particle params override data set for %1", EnumTools.EnumToString(EVehicleVFXEffect, effectId)));
        #endif

        return null;
    }

    // --------------------------------------------------------------------------------
    //! effect group API
    // --------------------------------------------------------------------------------

    void PlayEffectGroup(EVehicleVFXEffectGroup groupId, EntityAI parent = null)
    {
        if (!parent)
            parent = m_Parent;

        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return;

        foreach (VehicleVFXData data : groupData)
            PlayEffectData(data, parent);
    }

    // --------------------------------------------------------------------------------

    void StopEffectGroup(EVehicleVFXEffectGroup groupId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return;

        foreach (VehicleVFXData data : groupData)
            StopEffectData(data);
    }

    // --------------------------------------------------------------------------------

    void SetEffectGroupCooldown(EVehicleVFXEffectGroup groupId, float seconds)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
        {
            #ifdef DIAG_DEVELOPER
            ErrorEx(string.Format("Cannot set cooldown, group '%1' does not exist.", EnumTools.EnumToString(EVehicleVFXEffectGroup, groupId)));
            #endif
            return;
        }

        foreach (VehicleVFXData data : groupData)
            data.SetCooldown(seconds);
    }

    // --------------------------------------------------------------------------------

    //! true only when every member of the group can play
    bool CanPlayEffectGroup(EVehicleVFXEffectGroup groupId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return false;

        foreach (VehicleVFXData data : groupData)
        {
            if (!CanPlayEffectData(data))
                return false;
        }

        return true;
    }

    // --------------------------------------------------------------------------------

    //! longest remaining cooldown across the group members
    float GetEffectGroupRemainingCooldown(EVehicleVFXEffectGroup groupId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return 0;

        float maxRemaining = 0;
        foreach (VehicleVFXData data : groupData)
        {
            float remaining = data.GetRemainingCooldown();
            if (remaining > maxRemaining)
                maxRemaining = remaining;
        }

        return maxRemaining;
    }

    // --------------------------------------------------------------------------------

    void ResetEffectGroupCooldown(EVehicleVFXEffectGroup groupId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return;

        foreach (VehicleVFXData data : groupData)
            data.ResetCooldown();
    }

    // --------------------------------------------------------------------------------

    //! true when any member of the group is playing
    bool IsEffectGroupPlayed(EVehicleVFXEffectGroup groupId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return false;

        foreach (VehicleVFXData data : groupData)
        {
            if (IsEffectDataPlaying(data))
                return true;
        }

        return false;
    }

    // --------------------------------------------------------------------------------

    void SetEffectGroupPositions(EVehicleVFXEffectGroup groupId, TVectorArray positions)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData || !positions)
            return;

        int count = Math.Min(groupData.Count(), positions.Count());
        for (int i = 0; i < count; ++i)
            SetEffectDataPosition(groupData[i], positions[i]);
    }

    // --------------------------------------------------------------------------------

    void SetEffectGroupParticleState(EVehicleVFXEffectGroup groupId, int particleId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (!groupData)
            return;

        foreach (VehicleVFXData data : groupData)
        {
            if (data.m_Effect)
                data.m_Effect.SetParticleState(particleId);
        }
    }

    // --------------------------------------------------------------------------------

    ref ParticleParamsOverrideData GetEffectGroupParticleParamsOverrideData(EVehicleVFXEffectGroup groupId)
    {
        array<ref VehicleVFXData> groupData = m_EffectGroupsMap.Get(groupId);
        if (groupData)
        {
            foreach (VehicleVFXData data : groupData)
            {
                if (data && data.m_ParticleParamsData)
                    return data.m_ParticleParamsData;
            }
        }

        #ifdef DIAG_DEVELOPER
        ErrorEx(string.Format("No particle params override data set for group %1", EnumTools.EnumToString(EVehicleVFXEffectGroup, groupId)));
        #endif

        return null;
    }

    // --------------------------------------------------------------------------------

    void SetSurfaceInfo(SurfaceInfo surfaceInfo)
    {
        if (surfaceInfo)
        {
            m_SurfaceInfoOverriden = true;
            m_SurfaceInfo = surfaceInfo;

            return;
        }

        m_SurfaceInfoOverriden = false;
    }

    // --------------------------------------------------------------------------------

    #ifdef DIAG_DEVELOPER
    void ListGroups()
    {
        foreach (int groupId, array<ref VehicleVFXData> groupData : m_EffectGroupsMap)
            Print(string.Format("[VFX] group '%1' members=%2", EnumTools.EnumToString(EVehicleVFXEffectGroup, groupId), groupData.Count()));
    }

    // --------------------------------------------------------------------------------

    //! Dumps the cooldown state of every registered effect to the log.
    void ListCooldowns()
    {
        foreach (int effectId, VehicleVFXData data : m_EffectsMap)
            Print(string.Format("[VFX] '%1' cooldown=%2s canPlay=%3 remaining=%4s", EnumTools.EnumToString(EVehicleVFXEffect, effectId), data.GetCooldown(), data.CanPlay(), data.GetRemainingCooldown()));

        foreach (int groupId, array<ref VehicleVFXData> groupData : m_EffectGroupsMap)
        {
            foreach (int index, VehicleVFXData groupEffectData : groupData)
                Print(string.Format("[VFX] group '%1'[%2] cooldown=%3s canPlay=%4 remaining=%5s", EnumTools.EnumToString(EVehicleVFXEffectGroup, groupId), index, groupEffectData.GetCooldown(), groupEffectData.CanPlay(), groupEffectData.GetRemainingCooldown()));
        }
    }
    #endif
}
