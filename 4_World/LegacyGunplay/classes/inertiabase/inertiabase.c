class PlayerInertiaConstants
{
	static ref PlayerInertiaConstants instance;

	void PlayerInertiaConstants()
	{
		instance = this;
	}

	void Set(string varName, float value)
	{
		EnScript.SetClassVar(instance, varName, 0, value);
	}

	float Get(string varName)
	{
		if (varName == string.Empty)
			return -1;

		float res;
		EnScript.GetClassVar(instance, varName, -1, res);
		return res;
	}

	float DEFAULT_STRENGTH = 20;
	float DEFAULT_SMOOTH_TIME = 0.45;

	float MIN_SMOOTH_TIME = 0.3;
	float MAX_SMOOTH_TIME = 0.6;
	// this is max inertia multiplier
	float MAX_STRENGTH = 90;
	// this is max aim change amount to multiply by strength
	float MAX_TURN_CHANGE = 25;

	float ERECT_MODIFIER = 1;
	float CROUCH_MODIFIER = 1.15;
	float PRONE_MODIFIER = 1.5;

	float BIPOD_MODIFIER = 1.5;
}

class InertiaBase
{
	protected Weapon_Base m_Weapon;
	protected PlayerBase m_Player;

	protected float m_velocityYaw[1];
	protected float m_velocityPitch[1];
	protected float m_DynamicsModifier;

	void InertiaBase(Weapon_Base weapon)
	{
		//Make the PlayerInertiaConstants class if we haven't yet
		if (!PlayerInertiaConstants.instance)
			new PlayerInertiaConstants;

		m_Weapon = weapon;
		m_Player = PlayerBase.Cast(weapon.GetHierarchyRootPlayer());

		if (m_Weapon)
		{
			m_Weapon.GetPropertyModifierObject().UpdateModifiers();
		}

		m_DynamicsModifier = 1;
	}

	void ~InertiaBase()
	{
	}

	float GetDynamicsModifier()
	{
		float dynamics_modifier = 1;

		if (!m_Weapon)
			return dynamics_modifier;

		float attachments_modifier = m_Weapon.GetPropertyModifierObject().m_InertiaModifier;
		dynamics_modifier *= attachments_modifier;

		float barrel_length = m_Weapon.GetPropertyModifierObject().m_BarrelLength * 1.6;
		dynamics_modifier *= barrel_length;
		
		float weapon_weight = m_Weapon.GetPropertyModifierObject().m_Weight * 0.4;
		dynamics_modifier *= weapon_weight;

		float stance_modifier = GetStanceModifier();
		dynamics_modifier *= stance_modifier;

		float bipod_modifier = GetBipodModifier();
		dynamics_modifier *= bipod_modifier;

		DbgPrintInertiaBase("bipod_modifier: "+bipod_modifier);
		DbgPrintInertiaBase("stance_modifier: "+stance_modifier);
		DbgPrintInertiaBase("attachments_modifier: "+attachments_modifier);
		DbgPrintInertiaBase("barrel_length: "+barrel_length);
		DbgPrintInertiaBase("weapon_weight: "+weapon_weight);
		DbgPrintInertiaBase("dynamics_modifier: "+dynamics_modifier);

		return dynamics_modifier;
	}

	float GetDynamicsStrength()
	{
		return Math.Clamp(PlayerInertiaConstants.instance["DEFAULT_STRENGTH"] * m_DynamicsModifier, 0, PlayerInertiaConstants.instance["MAX_STRENGTH"]);
	}

	float GetDynamicsSmoothTime()
	{
		float dynamics_smoothing = PlayerInertiaConstants.instance["DEFAULT_SMOOTH_TIME"];

		return Math.Clamp(dynamics_smoothing * m_DynamicsModifier * 1.5, PlayerInertiaConstants.instance["MIN_SMOOTH_TIME"], PlayerInertiaConstants.instance["MAX_SMOOTH_TIME"]);
	}

	float GetBipodModifier()
	{
		float modifier = 1;

		if (m_Weapon && m_Weapon.HasBipodDeployed() && m_Player && !m_Player.IsUsingBipod())
		{
			modifier = PlayerInertiaConstants.instance["BIPOD_MODIFIER"];
		}

		return modifier;
	}

	/** 
	 * Get a modifier from player stance.
	 * Lower stance means more inertia
	 * 
	*/
	float GetStanceModifier()
	{
		float stance_modifier = 1;
		if (!m_Player) return stance_modifier;

		if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDERECT))
		{
			stance_modifier *= PlayerInertiaConstants.instance["ERECT_MODIFIER"];
		}
		else if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDCROUCH))
		{
			stance_modifier *= PlayerInertiaConstants.instance["CROUCH_MODIFIER"];
		}
		else if (m_Player.IsPlayerInStance(DayZPlayerConstants.STANCEMASK_RAISEDPRONE))
		{
			stance_modifier *= PlayerInertiaConstants.instance["PRONE_MODIFIER"];
		}

		return stance_modifier;
	}

	/** 
	 * We are applying the inertia directly onto the aiming model
	 * hands_offset_x/y should be the final values for the hands offset before inertia
	 * 
	*/
	void Update(float pDt, SDayZPlayerAimingModel pModel, float hands_offset_x, float hands_offset_y, out float final_offset_x, out float final_offset_y)
	{
		DbgPrintInertiaBase("DbgPrintInertiaBase | Player: "+m_Player+" | Update");

		//! get aim changes to apply our inertia
		HumanInputController hic = m_Player.GetInputController();
		float aimChangeX = hic.GetAimChange()[0] * Math.RAD2DEG;
		float aimChangeY = hic.GetAimChange()[1] * Math.RAD2DEG;

		DbgPrintInertiaBase("aimChangeX: "+aimChangeX);
		DbgPrintInertiaBase("aimChangeY: "+aimChangeY);

		m_DynamicsModifier = GetDynamicsModifier();
		float dynamics_strength = GetDynamicsStrength();
		float dynamics_smooth_time = GetDynamicsSmoothTime();
		
		DbgPrintInertiaBase("dynamics_strength: "+dynamics_strength);
		DbgPrintInertiaBase("dynamics_smooth_time: "+dynamics_smooth_time);
		
		float inertia_value_x = -(dynamics_strength * aimChangeX);
		float inertia_value_y = -(dynamics_strength * aimChangeY);

		DbgPrintInertiaBase("inertia_value_x: "+inertia_value_x);
		DbgPrintInertiaBase("inertia_value_y: "+inertia_value_y);

		final_offset_x = Math.SmoothCD(pModel.m_fAimXHandsOffset + hands_offset_x, inertia_value_x + hands_offset_x, m_velocityYaw, dynamics_smooth_time, 1000, pDt);
		final_offset_y = Math.SmoothCD(pModel.m_fAimYHandsOffset + hands_offset_y, inertia_value_y + hands_offset_y, m_velocityPitch, dynamics_smooth_time, 1000, pDt);

		DbgPrintInertiaBase("-------------------");
	}

	void DbgPrintInertiaBase(string val)
	{
		return;
		Print("DbgPrintInertiaBase | " + val);
	}
}