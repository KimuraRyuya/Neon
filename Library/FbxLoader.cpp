#pragma comment(lib, "Shlwapi.lib") 
#include <shlwapi.h>
#include <functional>
#include "Logger.h"
#include "FbxLoader.h"

// FbxDouble2 → XMFLOAT2
inline DirectX::XMFLOAT2 FbxDouble2ToFloat2(const FbxDouble2& fbx_value)
{
	return DirectX::XMFLOAT2(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1])
	);
}

// FbxDouble3 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble3ToFloat3(const FbxDouble3& fbx_value)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	);
}

// FbxDouble4 → XMFLOAT3
inline DirectX::XMFLOAT3 FbxDouble4ToFloat3(const FbxDouble4& fbx_value)
{
	return DirectX::XMFLOAT3(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4 FbxDouble4ToFloat4(const FbxDouble4& fbx_value)
{
	return DirectX::XMFLOAT4(
		static_cast<float>(fbx_value[0]),
		static_cast<float>(fbx_value[1]),
		static_cast<float>(fbx_value[2]),
		static_cast<float>(fbx_value[3])
	);
}

// FbxDouble4 → XMFLOAT4
inline DirectX::XMFLOAT4X4 FbxAMatrixToFloat4x4(const FbxAMatrix& fbx_value)
{
	return DirectX::XMFLOAT4X4(
		static_cast<float>(fbx_value[0][0]),
		static_cast<float>(fbx_value[0][1]),
		static_cast<float>(fbx_value[0][2]),
		static_cast<float>(fbx_value[0][3]),
		static_cast<float>(fbx_value[1][0]),
		static_cast<float>(fbx_value[1][1]),
		static_cast<float>(fbx_value[1][2]),
		static_cast<float>(fbx_value[1][3]),
		static_cast<float>(fbx_value[2][0]),
		static_cast<float>(fbx_value[2][1]),
		static_cast<float>(fbx_value[2][2]),
		static_cast<float>(fbx_value[2][3]),
		static_cast<float>(fbx_value[3][0]),
		static_cast<float>(fbx_value[3][1]),
		static_cast<float>(fbx_value[3][2]),
		static_cast<float>(fbx_value[3][3])
	);
}

// FBXファイルからモデルデータ読み込み
bool FbxLoader::Load(const char* filename, ModelData& data)
{
	//use cireal
	if (PathFileExistsA((std::string(filename) + ".bin").c_str()))
	{
		// Create from 'fbx_filename' file.
		data.LoadBinaryFile(filename);
	}
	else
	{
		// Create from 'fbx_filename' file.
		CreateFBX(filename, data);
		data.CreateBinaryFile(filename);
	}

	return true;
}

// FBXからロード
bool FbxLoader::CreateFBX(const char* filename, ModelData& data)
{
	FbxManager* fbx_manager = FbxManager::Create();

	// FBXに対する入出力を定義する
	FbxIOSettings* fbx_ios = FbxIOSettings::Create(fbx_manager, IOSROOT);	// 特別な理由がない限りIOSROOTを指定
	fbx_manager->SetIOSettings(fbx_ios);

	// インポータを生成
	FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");
	bool result = fbx_importer->Initialize(filename, -1, fbx_manager->GetIOSettings());	// -1でファイルフォーマット自動判定
	if (result)
	{
		// SceneオブジェクトにFBXファイル内の情報を流し込む
		FbxScene* fbx_scene = FbxScene::Create(fbx_manager, "scene");
		fbx_importer->Import(fbx_scene);
		fbx_importer->Destroy();	// シーンを流し込んだらImporterは解放してOK

									// ジオメトリを三角形化しておく
		FbxGeometryConverter fbx_geometry_converter(fbx_manager);
		fbx_geometry_converter.Triangulate(fbx_scene, true);
		fbx_geometry_converter.RemoveBadPolygonsFromMeshes(fbx_scene);

		// ディレクトリパス取得
		char dirname[256];
		::_splitpath_s(filename, nullptr, 0, dirname, 256, nullptr, 0, nullptr, 0);

		// モデル構築
		result = BuildModel(dirname, fbx_scene, data);
	}
	else
	{
		ECC_LOG("FbxImporter::Initialize() : Failed!!\n");
	}

	// マネージャ解放
	fbx_manager->Destroy();		// 関連するすべてのオブジェクトが解放される

	return result;
}

// FBXシーンからモデルデータを構築する
bool FbxLoader::BuildModel(const char* dirname, FbxScene* fbx_scene, ModelData& data)
{
	FbxNode* fbx_root_node = fbx_scene->GetRootNode();

	BuildNodes(fbx_root_node, data, -1);
	BuildMeshes(fbx_root_node, data);
	BuildMaterials(dirname, fbx_scene, data);
	BuildAnimations(fbx_scene, data);

	return true;
}

// FBXノードを再帰的に辿ってデータを構築する
void FbxLoader::BuildNodes(FbxNode* fbx_node, ModelData& data, int parent_node_index)
{
	FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
	FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

	if (fbx_node_attribute != nullptr)
	{
		fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
	}

	switch (fbx_node_attribute_type)
	{
	case FbxNodeAttribute::eUnknown:
	case FbxNodeAttribute::eNull:
	case FbxNodeAttribute::eMarker:
	case FbxNodeAttribute::eMesh:
	case FbxNodeAttribute::eSkeleton:
		BuildNode(fbx_node, data, parent_node_index);
		break;
	}

	// 再帰的に子ノードを処理する
	parent_node_index = static_cast<int>(data.nodes.size() - 1);
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		BuildNodes(fbx_node->GetChild(i), data, parent_node_index);
	}
}

// FBXノードからノードデータを構築する
void FbxLoader::BuildNode(FbxNode* fbx_node, ModelData& data, int parent_node_index)
{
	FbxAMatrix& fbx_local_transform = fbx_node->EvaluateLocalTransform();

	ModelData::Node node;
	node.name = fbx_node->GetName();
	node.parentIndex = parent_node_index;
	node.scale = FbxDouble4ToFloat3(fbx_local_transform.GetS());
	node.rotate = FbxDouble4ToFloat4(fbx_local_transform.GetQ());
	node.translate = FbxDouble4ToFloat3(fbx_local_transform.GetT());

	data.nodes.emplace_back(node);
}

// FBXノードを再帰的に辿ってメッシュデータを構築する
void FbxLoader::BuildMeshes(FbxNode* fbx_node, ModelData& data)
{
	FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
	FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;

	if (fbx_node_attribute != nullptr)
	{
		fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
	}

	switch (fbx_node_attribute_type)
	{
	case FbxNodeAttribute::eMesh:
		BuildMesh(fbx_node, static_cast<FbxMesh*>(fbx_node_attribute), data);
		break;
	}

	// 再帰的に子ノードを処理する
	for (int i = 0; i < fbx_node->GetChildCount(); ++i)
	{
		BuildMeshes(fbx_node->GetChild(i), data);
	}
}

// FBXメッシュからメッシュデータを構築する
void FbxLoader::BuildMesh(FbxNode* fbx_node, FbxMesh* fbx_mesh, ModelData& data)
{
	int fbx_control_points_count = fbx_mesh->GetControlPointsCount();
	int fbx_polygon_vertex_count = fbx_mesh->GetPolygonVertexCount();
	const int* fbx_polygon_vertices = fbx_mesh->GetPolygonVertices();

	int fbx_material_count = fbx_node->GetMaterialCount();
	int fbx_polygon_count = fbx_mesh->GetPolygonCount();

	data.meshes.emplace_back(ModelData::Mesh());
	ModelData::Mesh& mesh = data.meshes.back();
	mesh.indices.resize(fbx_polygon_count * 3);
	mesh.subsets.resize(fbx_material_count > 0 ? fbx_material_count : 1);
	mesh.nodeIndex = FindNodeIndex(data, fbx_node->GetName());

	// サブセットのマテリアル設定
	for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
	{
		const FbxSurfaceMaterial* fbx_surface_material = fbx_node->GetMaterial(fbx_material_index);

		ModelData::Subset& subset = mesh.subsets.at(fbx_material_index);
		subset.materialIndex = FindMaterialIndex(fbx_node->GetScene(), fbx_surface_material);
	}

	// サブセットの頂点インデックス範囲設定
	if (fbx_material_count > 0)
	{
		for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
		{
			int fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
			mesh.subsets.at(fbx_material_index).indexCount += 3;
		}

		int offset = 0;
		for (ModelData::Subset& subset : mesh.subsets)
		{
			subset.startIndex = offset;
			offset += subset.indexCount;

			subset.indexCount = 0;
		}
	}

	// 頂点影響力データ
	struct BoneInfluence
	{
		int use_count = 0;
		int indices[4] = { 0, 0, 0, 0 };
		float weights[4] = { 1.0f, 0.0f, 0.0f, 0.0f };

		void Add(int index, float weight)
		{
			indices[use_count] = index;
			weights[use_count] = weight;
			use_count++;
		}
	};
	// 頂点影響力データを抽出する
	std::vector<BoneInfluence> bone_influences;
	{
		bone_influences.resize(fbx_control_points_count);

		FbxAMatrix fbx_geometric_transform(
			fbx_node->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricRotation(FbxNode::eSourcePivot),
			fbx_node->GetGeometricScaling(FbxNode::eSourcePivot)
		);

		// スキニングに必要な情報を取得する
		int fbx_deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int fbx_deformer_index = 0; fbx_deformer_index < fbx_deformer_count; ++fbx_deformer_index)
		{
			FbxSkin* fbx_skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(fbx_deformer_index, FbxDeformer::eSkin));

			int fbx_cluster_count = fbx_skin->GetClusterCount();
			for (int fbx_cluster_index = 0; fbx_cluster_index < fbx_cluster_count; ++fbx_cluster_index)
			{
				FbxCluster* fbx_cluster = fbx_skin->GetCluster(fbx_cluster_index);

				// 頂点影響力データを抽出する
				{
					int fbx_cluster_control_point_indices_count = fbx_cluster->GetControlPointIndicesCount();
					const int* fbx_control_point_indices = fbx_cluster->GetControlPointIndices();
					const double* fbx_control_point_weights = fbx_cluster->GetControlPointWeights();

					for (int i = 0; i < fbx_cluster_control_point_indices_count; ++i)
					{
						BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_indices[i]);

						bone_influence.Add(fbx_cluster_index, static_cast<float>(fbx_control_point_weights[i]));
					}
				}

				// ボーン変換行列用の逆行列の計算をする
				{
					// メッシュ空間からワールド空間への変換行列
					FbxAMatrix fbx_mesh_space_transform;
					fbx_cluster->GetTransformMatrix(fbx_mesh_space_transform);

					// ボーン空間からワールド空間への変換行列
					FbxAMatrix fbx_bone_space_transform;
					fbx_cluster->GetTransformLinkMatrix(fbx_bone_space_transform);

					// ボーン逆行列を計算する
					const char* fbx_name = fbx_node->GetName();
					FbxAMatrix fbx_inverse_transform = fbx_bone_space_transform.Inverse() * fbx_mesh_space_transform * fbx_geometric_transform;

					DirectX::XMFLOAT4X4 inverse_transform = FbxAMatrixToFloat4x4(fbx_inverse_transform);
					ModelData::InverseTransform ModelDataInverseTransform;
					ModelDataInverseTransform.inverseTransforms = inverse_transform;
					mesh.inverseTransforms.emplace_back(ModelDataInverseTransform);

					int node_index = FindNodeIndex(data, fbx_cluster->GetLink()->GetName());
					mesh.nodeIndices.emplace_back(node_index);
				}
			}
		}
	}

	// UVセット名
	FbxStringList fbx_uv_names;
	fbx_mesh->GetUVSetNames(fbx_uv_names);

	// 頂点データ
	const FbxVector4* fbx_control_points = fbx_mesh->GetControlPoints();
	for (int fbx_polygon_index = 0; fbx_polygon_index < fbx_polygon_count; ++fbx_polygon_index)
	{
		// ポリゴンに適用されているマテリアルインデックスを取得する
		int fbx_material_index = 0;
		if (fbx_material_count > 0)
		{
			fbx_material_index = fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(fbx_polygon_index);
		}

		ModelData::Subset& subset = mesh.subsets.at(fbx_material_index);
		const int index_offset = subset.startIndex + subset.indexCount;

		for (int fbx_vertex_index = 0; fbx_vertex_index < 3; ++fbx_vertex_index)
		{
			ModelData::Vertex vertex;

			int fbx_control_point_index = fbx_mesh->GetPolygonVertex(fbx_polygon_index, fbx_vertex_index);
			// Position
			{
				vertex.position = FbxDouble4ToFloat3(fbx_control_points[fbx_control_point_index]);
			}

			// Weight
			{
				BoneInfluence& bone_influence = bone_influences.at(fbx_control_point_index);
				vertex.boneIndex.x = bone_influence.indices[0];
				vertex.boneIndex.y = bone_influence.indices[1];
				vertex.boneIndex.z = bone_influence.indices[2];
				vertex.boneIndex.w = bone_influence.indices[3];
				vertex.boneWeight.x = bone_influence.weights[0];
				vertex.boneWeight.y = bone_influence.weights[1];
				vertex.boneWeight.z = bone_influence.weights[2];
				vertex.boneWeight.w = bone_influence.weights[3];
			}

			// Normal
			if (fbx_mesh->GetElementNormalCount() > 0)
			{
				FbxVector4 fbx_normal;
				fbx_mesh->GetPolygonVertexNormal(fbx_polygon_index, fbx_vertex_index, fbx_normal);
				vertex.normal = FbxDouble4ToFloat3(fbx_normal);
			}
			else
			{
				vertex.normal = DirectX::XMFLOAT3(0, 0, 0);
			}

			// Texcoord
			if (fbx_mesh->GetElementUVCount() > 0)
			{
				bool fbx_unmapped_uv;
				FbxVector2 fbx_uv;
				fbx_mesh->GetPolygonVertexUV(fbx_polygon_index, fbx_vertex_index, fbx_uv_names[0], fbx_uv, fbx_unmapped_uv);
				fbx_uv[1] = 1.0 - fbx_uv[1];
				vertex.texcoord = FbxDouble2ToFloat2(fbx_uv);
			}
			else
			{
				vertex.texcoord = DirectX::XMFLOAT2(0, 0);
			}

			mesh.indices.at(index_offset + fbx_vertex_index) = static_cast<int>(mesh.vertices.size());
			mesh.vertices.emplace_back(vertex);
		}

		subset.indexCount += 3;
	}
}

// FBXシーン内のFBXマテリアルからマテリアルデータを構築する
void FbxLoader::BuildMaterials(const char* dirname, FbxScene* fbx_scene, ModelData& data)
{
	int fbx_material_count = fbx_scene->GetMaterialCount();

	if (fbx_material_count > 0)
	{
		for (int fbx_material_index = 0; fbx_material_index < fbx_material_count; ++fbx_material_index)
		{
			FbxSurfaceMaterial* fbx_surface_material = fbx_scene->GetMaterial(fbx_material_index);

			BuildMaterial(dirname, fbx_surface_material, data);
		}
	}
	else
	{
		ModelData::Material material;
		material.diffuseColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		data.materials.emplace_back(material);
	}
}

// FBXマテリアルからマテリアルデータを構築する
void FbxLoader::BuildMaterial(const char* dirname, FbxSurfaceMaterial* fbx_surface_material, ModelData& data)
{
	bool ret = false;
	ModelData::Material material;

	FbxProperty fbx_diffuse_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
	FbxProperty fbx_diffuse_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sDiffuseFactor);

	FbxProperty fbx_ambient_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sAmbient);
	FbxProperty fbx_ambient_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sAmbientFactor);

	FbxProperty fbx_specular_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sSpecular);
	FbxProperty fbx_specular_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sSpecularFactor);

	FbxProperty fbx_emissive_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sEmissive);
	FbxProperty fbx_emissive_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sEmissiveFactor);

	if (fbx_diffuse_property.IsValid() && fbx_diffuse_factor_property.IsValid())
	{
		FbxDouble  fbx_factor = fbx_diffuse_factor_property.Get<FbxDouble>();
		FbxDouble3 fbx_color = fbx_diffuse_property.Get<FbxDouble3>();

		material.diffuseColor.x = static_cast<float>(fbx_color[0] * fbx_factor);
		material.diffuseColor.y = static_cast<float>(fbx_color[1] * fbx_factor);
		material.diffuseColor.z = static_cast<float>(fbx_color[2] * fbx_factor);
		material.diffuseColor.w = 1.0f;

		fbx_factor = fbx_ambient_property.Get<FbxDouble>();
		fbx_color = fbx_ambient_factor_property.Get<FbxDouble3>();

		material.ambientColor.x = static_cast<float>(fbx_color[0] * fbx_factor);
		material.ambientColor.y = static_cast<float>(fbx_color[1] * fbx_factor);
		material.ambientColor.z = static_cast<float>(fbx_color[2] * fbx_factor);
		material.ambientColor.w = 1.0f;

		fbx_factor = fbx_specular_property.Get<FbxDouble>();
		fbx_color = fbx_specular_factor_property.Get<FbxDouble3>();

		material.specularColor.x = static_cast<float>(fbx_color[0] * fbx_factor);
		material.specularColor.y = static_cast<float>(fbx_color[1] * fbx_factor);
		material.specularColor.z = static_cast<float>(fbx_color[2] * fbx_factor);
		material.specularColor.w = 1.0f;

		fbx_factor = fbx_emissive_property.Get<FbxDouble>();
		fbx_color = fbx_emissive_factor_property.Get<FbxDouble3>();

		material.emissiveColor.x = static_cast<float>(fbx_color[0] * fbx_factor);
		material.emissiveColor.y = static_cast<float>(fbx_color[1] * fbx_factor);
		material.emissiveColor.z = static_cast<float>(fbx_color[2] * fbx_factor);
		material.emissiveColor.w = 1.0f;
	}
#if 0
	FbxProperty fbx_transparency_factor_property = fbx_surface_material->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (fbx_transparency_factor_property.IsValid())
	{
		FbxDouble fbx_factor = fbx_transparency_factor_property.Get<FbxDouble>();

		material.color.w = static_cast<float>(fbx_factor);
	}
#endif
	if (fbx_diffuse_property.IsValid())
	{
		int fbx_texture_count = fbx_diffuse_property.GetSrcObjectCount<FbxFileTexture>();
		if (fbx_texture_count > 0)
		{
			FbxFileTexture* fbx_texture = fbx_diffuse_property.GetSrcObject<FbxFileTexture>();

			char filename[256];
			::_makepath_s(filename, 256, nullptr, dirname, fbx_texture->GetRelativeFileName(), nullptr);
			material.textureFilename = filename;
		}
	}

	data.materials.emplace_back(material);
}

// アニメーションデータを構築
void FbxLoader::BuildAnimations(FbxScene* fbx_scene, ModelData& data)
{
	// すべてのアニメーション名を取得
	FbxArray<FbxString*> fbx_anim_stack_names;
	fbx_scene->FillAnimStackNameArray(fbx_anim_stack_names);

	int fbx_animation_count = fbx_anim_stack_names.Size();
	for (int fbx_animation_index = 0; fbx_animation_index < fbx_animation_count; ++fbx_animation_index)
	{
		data.animations.emplace_back(ModelData::Animation());
		ModelData::Animation& animation = data.animations.back();

		// アニメーションデータのサンプリング設定
		FbxTime::EMode fbx_time_mode = fbx_scene->GetGlobalSettings().GetTimeMode();
		FbxTime fbx_frame_time;
		fbx_frame_time.SetTime(0, 0, 0, 1, 0, fbx_time_mode);

		float sampling_rate = static_cast<float>(fbx_frame_time.GetFrameRate(fbx_time_mode));
		float sampling_time = 1.0f / sampling_rate;

		FbxString* fbx_anim_stack_name = fbx_anim_stack_names.GetAt(fbx_animation_index);
		FbxAnimStack* fbx_anim_stack = fbx_scene->FindMember<FbxAnimStack>(fbx_anim_stack_name->Buffer());

		// 再生するアニメーションを指定する。
		fbx_scene->SetCurrentAnimationStack(fbx_anim_stack);

		// アニメーションの再生開始時間と再生終了時間を取得する
		FbxTakeInfo* fbx_take_info = fbx_scene->GetTakeInfo(fbx_anim_stack_name->Buffer());
		FbxTime fbx_start_time = fbx_take_info->mLocalTimeSpan.GetStart();
		FbxTime fbx_end_time = fbx_take_info->mLocalTimeSpan.GetStop();

		// 抽出するデータは60フレーム基準でサンプリングする
		FbxTime fbx_sampling_step;
		fbx_sampling_step.SetTime(0, 0, 1, 0, 0, fbx_time_mode);
		fbx_sampling_step = static_cast<FbxLongLong>(fbx_sampling_step.Get() * sampling_time);

		int start_frame = static_cast<int>(fbx_start_time.Get() / fbx_sampling_step.Get());
		int end_frame = static_cast<int>(fbx_end_time.Get() / fbx_sampling_step.Get());
		int frame_count = static_cast<int>((fbx_end_time.Get() - fbx_start_time.Get()) / fbx_sampling_step.Get());

		// アニメーションの対象となるノードを列挙する
		std::vector<FbxNode*> fbx_nodes;
		std::function<void(FbxNode*)> traverse = [&](FbxNode* fbx_node)
		{
			FbxNodeAttribute* fbx_node_attribute = fbx_node->GetNodeAttribute();
			FbxNodeAttribute::EType fbx_node_attribute_type = FbxNodeAttribute::EType::eUnknown;
			if (fbx_node_attribute != nullptr)
			{
				fbx_node_attribute_type = fbx_node_attribute->GetAttributeType();
			}

			switch (fbx_node_attribute_type)
			{
			case FbxNodeAttribute::eUnknown:
			case FbxNodeAttribute::eNull:
			case FbxNodeAttribute::eMarker:
			case FbxNodeAttribute::eMesh:
			case FbxNodeAttribute::eSkeleton:
				fbx_nodes.emplace_back(fbx_node);
				break;
			}

			for (int i = 0; i < fbx_node->GetChildCount(); i++)
			{
				traverse(fbx_node->GetChild(i));
			}
		};
		traverse(fbx_scene->GetRootNode());

		// アニメーションデータを抽出する
		animation.secondsLength = frame_count * sampling_time;
		animation.keyframes.resize(frame_count + 1);

		float seconds = 0.0f;
		ModelData::Keyframe* keyframe = animation.keyframes.data();
		size_t fbx_node_count = fbx_nodes.size();
		FbxTime fbx_current_time = fbx_start_time;
		for (FbxTime fbx_current_time = fbx_start_time; fbx_current_time < fbx_end_time; fbx_current_time += fbx_sampling_step, ++keyframe)
		{
			// キーフレーム毎の姿勢データを取り出す。
			keyframe->seconds = seconds;
			keyframe->nodeKeys.resize(fbx_node_count);
			for (size_t fbx_node_index = 0; fbx_node_index < fbx_node_count; ++fbx_node_index)
			{
				ModelData::NodeKeyData& key_data = keyframe->nodeKeys.at(fbx_node_index);
				FbxNode* fbx_node = fbx_nodes.at(fbx_node_index);

				// 指定時間のローカル行列からスケール値、回転値、移動値を取り出す。
				const FbxAMatrix& fbx_local_transform = fbx_node->EvaluateLocalTransform(fbx_current_time);

				key_data.scale = FbxDouble4ToFloat3(fbx_local_transform.GetS());
				key_data.rotate = FbxDouble4ToFloat4(fbx_local_transform.GetQ());
				key_data.translate = FbxDouble4ToFloat3(fbx_local_transform.GetT());
			}
			seconds += sampling_time;
		}
	}

	// 後始末
	for (int i = 0; i < fbx_animation_count; i++)
	{
		delete fbx_anim_stack_names[i];
	}
}

// ノードインデックスを取得する
int FbxLoader::FindNodeIndex(ModelData& data, const char* name)
{
	for (size_t i = 0; i < data.nodes.size(); ++i)
	{
		if (data.nodes[i].name == name)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

// マテリアルインデックスを取得する
int FbxLoader::FindMaterialIndex(FbxScene* fbx_scene, const FbxSurfaceMaterial* fbx_surface_material)
{
	int fbx_material_count = fbx_scene->GetMaterialCount();

	for (int i = 0; i < fbx_material_count; ++i)
	{
		if (fbx_scene->GetMaterial(i) == fbx_surface_material)
		{
			return i;
		}
	}
	return -1;
}

