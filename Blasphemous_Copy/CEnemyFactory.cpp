#include "framework.h"
#include "CEnemyFactory.h"
#include "CEnemy.h"
#include "AI.h"
#include "CState_Idle.h"
#include "CState_Trace.h"
#include "CState_Attack.h"
#include "CState_Hit.h"
#include "CState_Dead.h"
#include "CState_Patrol.h"

#include "CEnemy_Acolyte.h"
#include "CEnemy_Stoner.h"
#include "CWeapon.h"
#include "CSpear.h"
#include "CScene.h"

CEnemy* CEnemyFactory::CreateEnemy(ENEMY_TYPE eEnmType, fPoint pos)
{
	CEnemy* pEnemy = nullptr;

	switch (eEnmType)
	{
	case ENEMY_TYPE::NORMAL:
	{
		AI* pAI = new AI;
		pAI->AddState(new CState_Idle(ENEMY_STATE::IDLE));
		pAI->AddState(new CState_Trace(ENEMY_STATE::TRACE));
		pAI->AddState(new CState_Attack(ENEMY_STATE::ATTACK));
		pAI->AddState(new CState_Hit(ENEMY_STATE::HIT));
		pAI->AddState(new CState_Dead(ENEMY_STATE::DEAD));
		pAI->AddState(new CState_Patrol(ENEMY_STATE::PATROL));
		pAI->SetCurState(ENEMY_STATE::PATROL);

		pEnemy = new CEnemy_Acolyte;
		pEnemy->SetPos(pos);

		pEnemy->SetAI(pAI);
	}
		break;
		
	case ENEMY_TYPE::RANGE:
	{
		AI* pAI = new AI;
		pAI->AddState(new CState_Idle(ENEMY_STATE::IDLE));
		pAI->AddState(new CState_Trace(ENEMY_STATE::TRACE));
		pAI->AddState(new CState_Attack(ENEMY_STATE::ATTACK));
		pAI->AddState(new CState_Hit(ENEMY_STATE::HIT));
		pAI->AddState(new CState_Dead(ENEMY_STATE::DEAD));
		pAI->AddState(new CState_Patrol(ENEMY_STATE::PATROL));
		pAI->SetCurState(ENEMY_STATE::PATROL);

		pEnemy = new CEnemy_Stoner;
		pEnemy->SetPos(pos);

		pEnemy->SetAI(pAI);
	}
		break;

	case ENEMY_TYPE::BOSS:
		break;

	default:
		break;
	}

	return pEnemy;
}
