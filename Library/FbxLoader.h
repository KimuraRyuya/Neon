// UNIT.02
#pragma once

#include <fbxsdk.h> 
#include "ModelData.h"

#include <iostream>
#include <fstream>
class FbxLoader
{
public:
	// FBX�t�@�C�����烂�f���f�[�^�ǂݍ���
	bool Load(const char* filename, ModelData& data);

private:
	// FBX���烍�[�h
	bool CreateFBX(const char* filename, ModelData& data);
	// ���f���f�[�^���\�z
	bool BuildModel(const char* dirname, FbxScene* fbx_scene, ModelData& data);

	// �m�[�h�f�[�^���\�z
	void BuildNodes(FbxNode* fbx_node, ModelData& data, int parent_node_index);
	void BuildNode(FbxNode* fbx_node, ModelData& data, int parent_node_index);
	
	// ���b�V���f�[�^���\�z
	void BuildMeshes(FbxNode* fbx_node, ModelData& data);
	void BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh, ModelData& data);

	// �}�e���A���f�[�^���\�z
	void BuildMaterials(const char* dirname, FbxScene* fbx_scene, ModelData& data);
	void BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbx_surface_material, ModelData& data);

	// �A�j���[�V�����f�[�^���\�z
	void BuildAnimations(FbxScene* fbx_scene, ModelData& data);

	// �C���f�b�N�X�̌���
	int FindNodeIndex(ModelData& data, const char* name);
	int FindMaterialIndex(FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material);
};
