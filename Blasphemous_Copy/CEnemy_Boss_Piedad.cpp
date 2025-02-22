#include "framework.h"
#include "CEnemy_Boss_Piedad.h"
#include "CCollider.h"
#include "CAnimator.h"
#include "CAnimation.h"
#include "CEffect_GroundSmash.h"
#include "CEffect.h"

CEnemy_Boss_Piedad::CEnemy_Boss_Piedad()
{
	SetName(L"Piedad");
	SetScale(fPoint(80.f, 130.f));
	m_fAtkAccTime = 0.f;

	tEnemyInfo info = {};
	info.fCurHp = 1000.f;
	info.fMaxHp = 1000.f;
	info.fAtt = 20.f;
	info.fAttRange = 1000.f;
	info.fAttDelayTime = 3.f;
	info.fRecogRange = 1000.f;
	info.fVelocity = 200.f;
	info.fMaxVelocity = 200.f;
	info.fInvTime = 0.1f;
	info.iMoney = 20;
	info.pWeapon = nullptr;
	SetEnemyInfo(info);

	CreateCollider();
	GetCollider()->SetScale(GetScale() * 2.f);
	GetCollider()->SetOffsetPos(fPoint(0.f, 70.f));

	Init_Animation();
}

CEnemy_Boss_Piedad::~CEnemy_Boss_Piedad()
{
}

CEnemy_Boss_Piedad* CEnemy_Boss_Piedad::Clone()
{
	return nullptr;
}

void CEnemy_Boss_Piedad::update()
{
	Update_Animation();

	GetAnimator()->update();

	CEnemy::update();

	if (GetAI()->GetCurState()->GetState() == ENEMY_STATE::SLEEP)
	{
		if (CSceneManager::GetInst()->GetCurrentScene()->GetSceneGroup() == GROUP_SCENE::BOSS)
			GetAnimator()->FindAnimation(L"Piedad_CutScene")->SetFrame(0);

		if (GetAnimator()->FindAnimation(L"Piedad_CutScene")->IsAnimDone())
		{
			if (GetAnimator()->FindAnimation(L"Piedad_CutScene2")->IsAnimDone())
				CGameManager::GetInst()->SetDisableControl(false);

			GetAnimator()->Play(L"Piedad_CutScene2");
		}
	}
}

void CEnemy_Boss_Piedad::render()
{
	component_render();
}

void CEnemy_Boss_Piedad::Update_Animation()
{
	if (GetAI()->GetPrevState() != nullptr &&
		GetAI()->GetCurState()->GetState() == GetAI()->GetPrevState()->GetState())
	{
		switch (GetAI()->GetCurState()->GetState())
		{
		case ENEMY_STATE::TRACE:
			if (GetDir().x > 0.f)
			{
				GetAnimator()->Play(L"Piedad_Walk_Right");
			}
			else
			{
				GetAnimator()->Play(L"Piedad_Walk_Left");
			}
		}
		return;
	}

	switch (GetAI()->GetCurState()->GetState())
	{
	case ENEMY_STATE::SLEEP:
	{
		GetAnimator()->Play(L"Piedad_CutScene");
	}break;
	case ENEMY_STATE::IDLE:
	{
		if (GetDir().x > 0.f)
		{
			GetAnimator()->FindAnimation(L"Piedad_Idle_Right")->SetFrame(0);
			GetAnimator()->Play(L"Piedad_Idle_Right");
		}
		else
		{
			GetAnimator()->FindAnimation(L"Piedad_Idle_Left")->SetFrame(0);
			GetAnimator()->Play(L"Piedad_Idle_Left");
		}
	}break;
	case ENEMY_STATE::TRACE:
	{
		GetAnimator()->FindAnimation(L"Piedad_Walk_Left")->SetFrame(0);
		GetAnimator()->FindAnimation(L"Piedad_Walk_Right")->SetFrame(0);

		if (GetDir().x > 0.f)
		{
			GetAnimator()->Play(L"Piedad_Walk_Right");
		}
		else
		{
			GetAnimator()->Play(L"Piedad_Walk_Left");
		}
	}break;
	case ENEMY_STATE::BOSSPATERN1:
	{
		GetAnimator()->FindAnimation(L"Piedad_GroundSmash_Left")->SetFrame(0);
		GetAnimator()->FindAnimation(L"Piedad_GroundSmash_Right")->SetFrame(0);

		if (GetDir().x > 0.f)
		{
			GetAnimator()->Play(L"Piedad_GroundSmash_Right");
		}
		else
		{
			GetAnimator()->Play(L"Piedad_GroundSmash_Left");
		}
	}break;
	case ENEMY_STATE::DEAD:
	{
		if (GetDir().x > 0.f)
		{
			GetAnimator()->FindAnimation(L"Piedad_Death_Right")->SetFrame(0);
			GetAnimator()->Play(L"Piedad_Death_Right");
		}
		else
		{
			GetAnimator()->FindAnimation(L"Piedad_Death_Left")->SetFrame(0);
			GetAnimator()->Play(L"Piedad_Death_Left");
		}
	}break;

	default:
		break;
	}

	GetAI()->SetPrevState(GetAI()->GetCurState()->GetState());
}

void CEnemy_Boss_Piedad::Init_Animation()
{
#pragma region AnimationRegistration

	SetImage(L"Piedad_CutScene", L"texture\\Enemy\\Piedad\\CutScene\\pietat_boss_sprite_sheet_part_1.png");
	m_pCutScenePart2 = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_CutScene2", L"texture\\Enemy\\Piedad\\CutScene\\pietat_boss_sprite_sheet_part_2.png");

	m_pIdleImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Idle", L"texture\\Enemy\\Piedad\\Idle\\Pietat_Idle_Anim.png");

	m_pWalkImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Walk", L"texture\\Enemy\\Piedad\\Walk\\pietat_walk_anim.png");
	m_pWalkToIdleImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Walk", L"texture\\Enemy\\Piedad\\Walk\\pietat_walk_to_idle_anim.png");

	m_pGroundSmashImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_GroundSmash", L"texture\\Enemy\\Piedad\\GroundSmash\\Pietat_GroundSmash_Right.png");

	m_pSlashImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Slash", L"texture\\Enemy\\Piedad\\Slash\\pietat_slash_anim.png");

	m_pSpitImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Spit", L"texture\\Enemy\\Piedad\\Spit\\pietat_spit_loop_anim.png");
	m_pSpitBackIdleImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_SpitBackIdle", L"texture\\Enemy\\Piedad\\Spit\\pietat_spit_back_to_idle_anim.png");
	m_pSpitStartImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_SpitStart", L"texture\\Enemy\\Piedad\\Spit\\pietat_spit_start_anim.png");

	m_pStompImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Stomp", L"texture\\Enemy\\Piedad\\Stomp\\pietat_stomp_anim.png");

	m_pTurnAroundImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Turn", L"texture\\Enemy\\Piedad\\TurnAround\\pietat_turnaround_anim.png");

	m_pDeathImg = CResourceManager::GetInst()->LoadD2DImage(
		L"Piedad_Death", L"texture\\Enemy\\Piedad\\Death\\pietat_death_anim.png");

	CreateAnimator();
	GetAnimator()->CreateAnimation(L"Piedad_CutScene",
		GetImage(), fPoint(0.f, 0.f), fPoint(500.f, 272.f), fPoint(500.f, 0.f), 10, 0.15f, 48, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_CutScene2",
		m_pCutScenePart2, fPoint(0.f, 0.f), fPoint(500.f, 272.f), fPoint(500.f, 0.f), 10, 0.13f, 39, false, false);

	GetAnimator()->CreateAnimation(L"Piedad_Idle_Right",
		m_pIdleImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 5, 0.1f, 13, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_Idle_Left",
		m_pIdleImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 5, 0.1f, 13, false, true);


	GetAnimator()->CreateAnimation(L"Piedad_Walk_Right",
		m_pWalkImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 4, 0.1f, 16, true, false);
	GetAnimator()->CreateAnimation(L"Piedad_Walk_Left",
		m_pWalkImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 4, 0.1f, 16, true, true);

	GetAnimator()->CreateAnimation(L"Piedad_WalkToIdle_Right",
		m_pWalkToIdleImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 5, 0.1f, 10, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_WalkToIdle_Left",
		m_pWalkToIdleImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 5, 0.1f, 10, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_GroundSmash_Right",
		m_pGroundSmashImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 10, 0.1f, 50, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_GroundSmash_Left",
		m_pGroundSmashImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 10, 0.1f, 50, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_Slash_Right",
		m_pSlashImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 10, 0.1f, 52, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_Slash_Left",
		m_pSlashImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 10, 0.1f, 52, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_SpitLoop_Right",
		m_pSpitImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 10, 0.1f, 15, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_SpitLoop_Left",
		m_pSpitImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 10, 0.1f, 15, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_SpitBackToIdle_Right",
		m_pSpitBackIdleImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 0, 0.1f, 5, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_SpitBackToIdle_Left",
		m_pSpitBackIdleImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 0, 0.1f, 5, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_SpitStart_Right",
		m_pSpitStartImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 0, 0.1f, 10, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_SpitStart_Left",
		m_pSpitStartImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 0, 0.1f, 10, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_Stomp_Right",
		m_pStompImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 7, 0.1f, 19, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_Stomp_Left",
		m_pStompImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 7, 0.1f, 19, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_Turn_Right",
		m_pTurnAroundImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 5, 0.1f, 13, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_Turn_Left",
		m_pTurnAroundImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 5, 0.1f, 13, false, true);

	GetAnimator()->CreateAnimation(L"Piedad_Death_Right",
		m_pDeathImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 7, 0.1f, 37, false, false);
	GetAnimator()->CreateAnimation(L"Piedad_Death_Left",
		m_pDeathImg, fPoint(0.f, 0.f), fPoint(382.f, 300.f), fPoint(382.f, 0.f), 7, 0.1f, 37, false, true);

#pragma endregion

	CAnimation* pAnim = GetAnimator()->FindAnimation(L"Piedad_CutScene");
	for (int i = 0; i < 48; ++i)
	{
		pAnim->GetFrame(i).fptOffset.x -= 7.f;
		pAnim->GetFrame(i).fptOffset.y -= 67.f;
	}
	for (int i = 0; i < 16; ++i)
	{
		pAnim->GetFrame(i).fDuration = 0.04f;
	}

	pAnim = GetAnimator()->FindAnimation(L"Piedad_CutScene2");
	for (int i = 0; i < 39; ++i)
	{
		pAnim->GetFrame(i).fptOffset.x -= 7.f;
		pAnim->GetFrame(i).fptOffset.y -= 67.f;
	}
}

void CEnemy_Boss_Piedad::Attack()
{
	switch (GetAI()->GetCurState()->GetState())
	{
	case ENEMY_STATE::BOSSPATERN1:
	{
		float iMoveStartTime = GetAnimator()->GetCurAnim()->GetFrame(0).fDuration * 30;

		if (GetAttCount() == 0)
		{
			m_fAtkAccTime += fDeltaTime;

			if (iMoveStartTime <= m_fAtkAccTime)
			{
				CEffect_GroundSmash* pFx = new CEffect_GroundSmash;
				pFx->SetOwnerObj(this);
				pFx->SetPos(fPoint(GetPos().x - 130.f, GetPos().y + 170.f));
				pFx->SetDuration(0.3f);
				CreateObj(pFx, GROUP_GAMEOBJ::ENEMY_ATT_FX);

				CEffect_GroundSmash* pFx2 = new CEffect_GroundSmash;
				pFx2->SetOwnerObj(this);
				pFx2->SetPos(fPoint(GetPos().x + 130.f, GetPos().y + 170.f));
				pFx2->SetDuration(0.3f);
				CreateObj(pFx2, GROUP_GAMEOBJ::ENEMY_ATT_FX);

				CSoundManager::GetInst()->Play(L"Piedad_SmashVoice");
				CSoundManager::GetInst()->Play(L"Piedad_Smash");

				if (GetEnemyInfo().fCurHp / GetEnemyInfo().fMaxHp > 0.7f)
				{
					pFx->SpawnThorns(3);
					pFx2->SpawnThorns(3);
				}
				else if (GetEnemyInfo().fCurHp / GetEnemyInfo().fMaxHp > 0.4f)
				{
					pFx->SpawnThorns(4);
					pFx2->SpawnThorns(4);
				}
				else
				{
					pFx->SpawnThorns(5);
					pFx2->SpawnThorns(5);
				}

				SetAttCount(1);
				m_fAtkAccTime = 0;
			}
		}
	}
	break;
	case ENEMY_STATE::BOSSPATERN2:
		break;
	case ENEMY_STATE::BOSSPATERN3:
		break;
	}
}
