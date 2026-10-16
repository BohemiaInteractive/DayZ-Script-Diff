class ThermalBiasHandler
{
    protected PlayerBase m_Player;
    protected PlayerStat<float> m_StatCache;
    protected float m_TemporaryResistanceTime;

    void ThermalBiasHandler(PlayerBase player)
    {
        m_Player = player;
        m_TemporaryResistanceTime = 0.0;
    }

    private PlayerStat<float> GetStat()
    {
        if (!m_StatCache && m_Player.GetPlayerStats())
            m_StatCache = PlayerStat<float>.Cast(m_Player.GetPlayerStats().GetStatObject(EPlayerStats_current.THERMAL_BIAS));

        return m_StatCache;
    }

    void OnTick(float deltaT)
    {
        if (m_TemporaryResistanceTime > 0.0)
        {
            m_TemporaryResistanceTime -= deltaT;
            if (m_TemporaryResistanceTime < 0.0)
                m_TemporaryResistanceTime = 0.0;
        }
    }

    float Get()
    {
        PlayerStat<float> stat = GetStat();
        if (stat)
            return stat.Get();

        return 0.0;
    }

    void Add(float value)
    {
        if (m_TemporaryResistanceTime > 0.0)
            return;

        PlayerStat<float> stat = GetStat();
        if (stat)
            stat.Add(value);
    }

    void Set(float value)
    {
        PlayerStat<float> stat = GetStat();
        if (stat)
            stat.Set(value);
    }

    void SetTemporaryResistance(float time)
    {
        m_TemporaryResistanceTime = Math.Clamp(time, 0.0, float.MAX);
    }

    float GetTemporaryResistance()
    {
        return m_TemporaryResistanceTime;
    }

    void OnStoreSave(ParamsWriteContext ctx)
    {
        ctx.Write(m_TemporaryResistanceTime);
    }

    bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!ctx.Read(m_TemporaryResistanceTime))
            return false;

        return true;
    }
}
