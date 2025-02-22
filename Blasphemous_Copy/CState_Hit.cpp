#include "framework.h"
#include "CState_Hit.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CScene.h"
#include "CAnimator.h"
#include "CAnimation.h"

CState_Hit::CState_Hit(ENEMY_STATE eState)
	: CState(eState)
{
	CSoundManager::GetInst()->AddSound(L"Enemy_Hit1", L"sound\\SoundEffects\\Penitent\\PENITENT_ENEMY_HIT_1.wav", false);
	CSoundManager::GetInst()->AddSound(L"Enemy_Hit2", L"sound\\SoundEffects\\Penitent\\PENITENT_ENEMY_HIT_2.wav", false);
	CSoundManager::GetInst()->AddSound(L"Enemy_Hit3", L"sound\\SoundEffects\\Penitent\\PENITENT_ENEMY_HIT_3.wav", false);
	CSoundManager::GetInst()->AddSound(L"Enemy_Hit4", L"sound\\SoundEffects\\Penitent\\PENITENT_ENEMY_HIT_4.wav", false);
}

CState_Hit::~CState_Hit()
{
}

static float fHitAccTime;

void CState_Hit::update()
{
	fHitAccTime += fDeltaTime;
	CEnemy* pEnemy = GetEnemy();

	if (pEnemy->GetAnimator()->GetCurAnim()->GetAnimDuration() <= fHitAccTime
		&& pEnemy->GetAnimator()->GetCurAnim()->IsAnimDone())
	{
		ChangeAIState(GetAI(), ENEMY_STATE::TRACE);
		fHitAccTime = 0.f;
	}

	CPlayer* pPlayer = CPlayer::GetPlayer();
	fVector2D fvPlayerPos = pPlayer->GetPos();
	fVector2D fvEnemyPos = GetEnemy()->GetPos();

	fVector2D fvEnemyDir = fvPlayerPos - fvEnemyPos;
	float fLength = fvEnemyDir.Length();

	static float fAttAccTime;
	fAttAccTime += fDeltaTime;
	if (GetEnemy()->GetEnemyInfo().fAttDelayTime <= fAttAccTime)
	{
		if (fLength < GetEnemy()->GetEnemyInfo().fAttRange && GetEnemy()->GetEnemyInfo().iAttCount == 0)
		{
			ChangeAIState(GetAI(), ENEMY_STATE::ATTACK);
			fHitAccTime = 0.f;
		}
	}
}

void CState_Hit::Enter()
{
	fHitAccTime = 0.f;
}

void CState_Hit::Exit()
{
}
