
#include "Logger.h"
#include "Model.h"
#include "Misc.h"

Model::Model(std::shared_ptr<ModelResource>& resource)
{
	modelResource = resource;

	// �m�[�h
	const std::vector<ModelData::Node>& res_nodes = resource->GetNodes();

	nodes.resize(res_nodes.size());
	for (size_t node_index = 0; node_index < nodes.size(); ++node_index)
	{
		auto&& src = res_nodes.at(node_index);
		auto&& dst = nodes.at(node_index);

		dst.name = src.name.c_str();
		dst.parent = src.parentIndex  >= 0 ? &nodes.at(src.parentIndex) : nullptr;
		dst.scale = src.scale;
		dst.rotate = src.rotate;
		dst.translate = src.translate;
	}
}

// �A�j���[�V�����Đ�
void Model::PlayAnimation(int animation_index, bool loop)
{
	currentAnimation = animation_index;
	loopAnimation = loop;
	endAnimation = false;
	currentSeconds = 0.0f;
}

// �A�j���[�V�����v�Z
void Model::UpdateAnimation(float elapsed_time)
{
	if (currentAnimation < 0)
	{
		return;
	}

	if (modelResource->GetAnimations().empty())
	{
		return;
	}

	const ModelData::Animation& animation = modelResource->GetAnimations().at(currentAnimation);

	const std::vector<ModelData::Keyframe>& keyframes = animation.keyframes;
	int key_count = static_cast<int>(keyframes.size());
	for (int key_index = 0; key_index < key_count - 1; ++key_index)
	{
		// ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
		const ModelData::Keyframe& keyframe0 = keyframes.at(key_index);
		const ModelData::Keyframe& keyframe1 = keyframes.at(key_index + 1);
		if (currentSeconds >= keyframe0.seconds && currentSeconds < keyframe1.seconds)
		{
			float rate = (currentSeconds - keyframe0.seconds / keyframe1.seconds - keyframe0.seconds);

			assert(nodes.size() == keyframe0.nodeKeys.size());
			assert(nodes.size() == keyframe1.nodeKeys.size());
			int node_count = static_cast<int>(nodes.size());
			for (int node_index = 0; node_index < node_count; ++node_index)
			{
				// �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
				const ModelData::NodeKeyData& key0 = keyframe0.nodeKeys.at(node_index);
				const ModelData::NodeKeyData& key1 = keyframe1.nodeKeys.at(node_index);

				Node& node = nodes[node_index];
				
				DirectX::XMVECTOR s0 = DirectX::XMLoadFloat3(&key0.scale);
				DirectX::XMVECTOR s1 = DirectX::XMLoadFloat3(&key1.scale);
				DirectX::XMVECTOR r0 = DirectX::XMLoadFloat4(&key0.rotate);
				DirectX::XMVECTOR r1 = DirectX::XMLoadFloat4(&key1.rotate);
				DirectX::XMVECTOR t0 = DirectX::XMLoadFloat3(&key0.translate);
				DirectX::XMVECTOR t1 = DirectX::XMLoadFloat3(&key1.translate);

				DirectX::XMVECTOR s = DirectX::XMVectorLerp(s0, s1, rate);
				DirectX::XMVECTOR r = DirectX::XMQuaternionSlerp(r0, r1, rate);
				DirectX::XMVECTOR t = DirectX::XMVectorLerp(t0, t1, rate);

				DirectX::XMStoreFloat3(&node.scale, s);
				DirectX::XMStoreFloat4(&node.rotate, r);
				DirectX::XMStoreFloat3(&node.translate, t);
			}
			break;
		}
	}

	// �ŏI�t���[������
	if (endAnimation)
	{
		endAnimation = false;
		currentAnimation = -1;
		return;
	}

	// ���Ԍo��
	currentSeconds += elapsed_time;
	if (currentSeconds >= animation.secondsLength)
	{
		if (loopAnimation)
		{
			currentSeconds -= animation.secondsLength;
		}
		else
		{
			currentSeconds = animation.secondsLength;
			endAnimation = true;
		}
	}
}

// ���[�J���ϊ��s��v�Z
void Model::CalculateLocalTransform()
{
	for (Node& node : nodes)
	{
		DirectX::XMMATRIX scale, rotate, translate, local;
		scale = DirectX::XMMatrixScaling(node.scale.x, node.scale.y, node.scale.z);
		rotate = DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(node.rotate.x, node.rotate.y, node.rotate.z, node.rotate.w));
		translate = DirectX::XMMatrixTranslation(node.translate.x, node.translate.y, node.translate.z);

		local = scale * rotate * translate;
		DirectX::XMStoreFloat4x4(&node.localTransform, local);
	}
}

// ���[���h�ϊ��s��v�Z
void Model::CalculateWorldTransform(const DirectX::XMMATRIX& world_transform)
{
	DirectX::XMStoreFloat4x4(&world, world_transform);
	for (Node& node : nodes)
	{
		if (node.parent != nullptr)
		{
			DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.localTransform);
			DirectX::XMMATRIX parent_world_transform = DirectX::XMLoadFloat4x4(&node.parent->worldTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, local_transform * parent_world_transform);
		}
		else
		{
			DirectX::XMMATRIX local_transform = DirectX::XMLoadFloat4x4(&node.localTransform);
			DirectX::XMStoreFloat4x4(&node.worldTransform, local_transform * world_transform);
		}
	}
}
