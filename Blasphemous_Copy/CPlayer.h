#pragma once
#include "CGameObject.h"

#define MAX_SPEED (300.f)

class CD2DImage;
class CPlayerSword;

enum class PLAYER_STATE
{
	IDLE,
	CROUCH,
	CLIMB,
	RUN,
	DODGE,
	ATTACK,
	JUMP,
	JUMPOFF,
	DEAD,
};

enum class PLAYER_ATTACK_STATE
{
	FIRST_SLASH,
	SECOND_SLASH,
	THIRD_SLASH,

	JUMP_SLASH,

	CROUCH_SLASH,
	
	NONE,
};

struct tPlayerAbility
{
	float fMaxHp;
	float fCurHp;
	float fMaxMp;
	float fCurMp;

	UINT  iHpPotionCount;
	float fHpRecoveryAmount;

	float fAtt;

	USHORT  sMoney;
};

class CPlayer : public CGameObject
{
	
private:
	CD2DImage* m_pImg;
	CD2DImage* m_pDashImg;

	PLAYER_STATE m_eCurState;		// 현재 플레이어의 상태
	PLAYER_STATE m_ePrevState;		// 이전 플레이어의 상태

	PLAYER_ATTACK_STATE m_eCurAttState; // 현재 플레이어 어택 상태
	PLAYER_ATTACK_STATE m_ePreAttState; // 이전 플레이어 어택 상태

	tPlayerAbility m_tAbility;

	fVector2D m_fvCurDir;
	fVector2D m_fvPrevDir;

	float m_fVelocity;
	float m_fMaxVelocity;
	float m_fAccelGravity;
	float m_fJumpPower;
	float m_fFrictionValue;
	bool  m_bIsActing;
	bool  m_bIsGrounded;

	float m_fAtkAccTime;
	float m_fAttackDelay;
	UINT  m_iComboCount;
	bool  m_bIsAttacking;

	float m_fDodgeAccTime;
	float m_fDodgeDelay;
	float m_fDodgeDelayAccTime;
	bool  m_bIsInvincible;

	CPlayerSword* m_pSword;

public:
	CPlayer();
	virtual ~CPlayer();

	virtual CPlayer* Clone() override;

public:
	virtual void update() final;
	void update_state();
	void update_move();
	void update_animation();

	virtual void render() final;
	virtual void component_render() final;
	virtual void debug_render() final;

public:
	void InitAbility();
	void InitAnimation();

public:
	void Jump();
	void Dash();

	const float		 GetVelocity();
	const fVector2D& GetDirVector();

	const tPlayerAbility& GetPlayerAbility();
	void SetPlayerAbility(tPlayerAbility tAbility);

	const UINT GetAttackCount() { return m_iComboCount; }
	void SetAttackCount(const UINT count) { m_iComboCount = count; }

	void SetWeapon(CPlayerSword* weapon) { m_pSword = weapon; }

public:
	virtual void OnCollision(CCollider* target) override;
	virtual void OnCollisionEnter(CCollider* target) override;
	virtual void OnCollisionExit(CCollider* target) override;
};

