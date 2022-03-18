#include "framework.h"
#include "CEnemyFactory.h"
#include "CEnemy.h"
#include "AI.h"
#include "CState_Idle.h"
#include "CState_Trace.h"

CEnemy* CEnemyFactory::CreateEnemy(ENEMY_TYPE eEnmType, fPoint pos)
{
	CEnemy* pEnemy = nullptr;

	switch (eEnmType)
	{
	case ENEMY_TYPE::NORMAL:
	{
		pEnemy = new CEnemy;
		pEnemy->SetPos(pos);

		AI* pAI = new AI;
		pAI->AddState(new CState_Idle(ENEMY_STATE::IDLE));
		pAI->AddState(new CState_Trace(ENEMY_STATE::TRACE));
		pAI->SetCurState(ENEMY_STATE::IDLE);

		pEnemy->InitObject(pos, fPoint(100.f, 100.f));

		tEnemyInfo info = {};
		info.fHP = 60.f;
		info.fAtt = 10.f;
		info.fAttRange = 30.f;
		info.fAttDelayTime = 5.f;
		info.fRecogRange = 200.f;
		info.fVelocity = 300.f;

		pEnemy->SetAI(pAI);
	}
		break;
		
	case ENEMY_TYPE::RANGE:
		break;

	case ENEMY_TYPE::BOSS:
		break;

	default:
		break;
	}

	return pEnemy;
}
