#pragma once
#include <DirectXMath.h>
#include "Collision.h"
#include "../Library/Model.h"
#include "../Library/ModelResource.h"
#include "../Library/GPUParticle/EmitParticle.h"

// �G�l�~�[�̃o�b�t�@�p�ϐ����p�����[�^�[
struct EnemyBufferInfo
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotate;
	DirectX::XMFLOAT3 size;
	float hp;
	// 40 byte
};

class Enemy
{
private:

public:
	// �G�l�~�[�̐F�̎��
	enum ColorType
	{
		Red,
		Yellow,
		Purple
	};

	// �G�l�~�[�̍s���p�^�[��
	enum ActionType
	{
		Top_to_Bottom_Liner,
		Bottom_to_Top_Liner,
		Right_to_LeftLiner,
		Left_to_Right_Liner,
		Chase,
		Reflection,
		WARNING
	};

	std::shared_ptr<Model> model;
	std::shared_ptr<Model> tmpModel;
	std::unique_ptr<CollisionPrimitive> enmCollision;

	float angle;
	float speed;

	ColorType colorType;
	ActionType actionType;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 target;

	float spawnTimer;
	float spawnMaxTimer;
	
	bool hasNowSpawn;
	bool isExist;

	void Initialize(EnemyBufferInfo& enm);

	bool Pop(EnemyBufferInfo& enm);
	/// <summary>
	/// �v���C���[��ǂ�������G�l�~�[
	/// </summary>
	/// <param name="enm"> �G�l�~�[�̊�{�p�����[�^�[�̕ۑ��� </param>
	/// <param name="type"> �s���^�C�v </param>
	/// <param name="level"> �v���C���[�̃��x�� </param>
	/// <param name="_model"> �G�l�~�[�̃��f�� </param>
	/// <param name="_spawnModel"> �G�l�~�[�̃X�|�[�����f�� </param>
	/// <returns></returns>
	bool ChasePop(EnemyBufferInfo& enm, Enemy::ActionType type, int level, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel);
	
	/// <summary>
	/// ������Ɉړ�����G�l�~�[
	/// </summary>
	/// <param name="enm"> �G�l�~�[�̊�{�p�����[�^�[�̕ۑ��� </param>
	/// <param name="p"> �X�|�[���ʒu </param>
	/// <param name="level"> �v���C���[�̃��x�� </param>
	/// <param name="type"> �G�l�~�[�̍s���p�^�[�� </param>
	/// <param name="_model"> �G�l�~�[�̃��f�� </param>
	/// <param name="_spawnModel"> �G�l�~�[�̃X�|�[�����̃��f�� </param>
	/// <returns></returns>
	bool LinearPop(EnemyBufferInfo& enm, DirectX::XMFLOAT3 p, int level, Enemy::ActionType type, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel);
	
	/// <summary>
	/// �����_���Ȑi�s�x�N�g���̂ق��ɐi�ރG�l�~�[
	/// </summary>
	/// <param name="enm"> �G�l�~�[�̊�{�p�����[�^�[�̕ۑ��� </param>
	/// <param name="type"> �s���p�^�[�� </param>
	/// <param name="level"> �v���C���[�̃��x�� </param>
	/// <param name="_model"> �G�l�~�[�̃��f�� </param>
	/// <param name="_spawnModel"> �G�l�~�[�̃X�|�[�����̃��f�� </param>
	/// <returns></returns>
	bool ReflexPop(EnemyBufferInfo& enm, Enemy::ActionType type, int level, std::shared_ptr<Model> _model, std::shared_ptr<Model> _spawnModel);

	// �G�l�~�[�̍s���֐�
	void Move(EnemyBufferInfo& enm, float elapsedTime);
	// ���[���h���W�v�Z
	void GiveModelInfo(EnemyBufferInfo& enm);
	// �G�l�~�[�̃p�����[�^�[���N���A
	void EnemyInfClear(EnemyBufferInfo& enm);

	Enemy() {}


	~Enemy() {}
private:

};
