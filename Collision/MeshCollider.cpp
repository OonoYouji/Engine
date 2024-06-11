#include <MeshCollider.h>

#include <ImGuiManager.h>


namespace {

#pragma region 宣言

	/// <summary>
	/// 指定したベクトル方向へ最も遠い点を見つける
	/// </summary>
	Vec3f Support(const std::vector<Vec3f>& vertices, const Vec3f& direction);

	/// <summary>
	/// 二つのオブジェクトのミンコフスキー差のサポート写像を計算する
	/// </summary>
	Vec3f MinkowskiDifferenceSupport(const std::vector<Vec3f>& vertices1, const std::vector<Vec3f>& vertices2, const Vec3f& direction);

	/// <summary>
	/// 二つの頂点配列を使用してミンコフスキー差を求める
	/// </summary>
	std::vector<Vec3f> MinkowskiDifference(const std::vector<Vec3f>& a, const std::vector<Vec3f>& b);

	/// <summary>
	/// vertices内に原点が含まれるかを確認する
	/// </summary>
	/// <returns></returns>
	bool ContainsOrigin(std::vector<Vec3f>& vertices, Vec3f& direction);

	/// <summary>
	/// ConstainOrigin()の引数verticesのサイズが "2" の時の処理
	/// </summary>
	bool HandleLineCase(std::vector<Vec3f>& vertices, Vec3f& direction);

	/// <summary>
	/// ConstainOrigin()の引数verticesのサイズが "3" の時の処理
	/// </summary>
	bool HandleTriangleCase(std::vector<Vec3f>& vertices, Vec3f& direction);

	/// <summary>
	/// ConstainOrigin()の引数verticesのサイズが "4" の時の処理
	/// </summary>
	bool HandleTetrahedronCase(std::vector<Vec3f>& vertices, Vec3f& direction);

#pragma endregion


#pragma region 定義

	Vec3f Support(const std::vector<Vec3f>& vertices, const Vec3f& direction) {
		float maxDot = 0.0f;
		Vec3f result = vertices[0];
		for(const auto& vertex : vertices) {
			float d = Vec3f::Dot(vertex, direction);
			if(d > maxDot) {
				maxDot = d;
				result = vertex;
			}
		}
		return result;
	}

	Vec3f MinkowskiDifferenceSupport(const std::vector<Vec3f>& vertices1, const std::vector<Vec3f>& vertices2, const Vec3f& direction) {
		return Support(vertices1, direction) - Support(vertices2, -direction);
	}

	std::vector<Vec3f> MinkowskiDifference(const std::vector<Vec3f>& a, const std::vector<Vec3f>& b) {
		std::vector<Vec3f> result;
		for(const auto& av : a) {
			for(const auto& bv : b) {
				result.push_back(av - bv);
			}
		}
		return result;
	}

	bool ContainsOrigin(std::vector<Vec3f>& vertices, Vec3f& direction) {
		switch(vertices.size()) {
		case 2: return HandleLineCase(vertices, direction);
		case 3: return HandleTriangleCase(vertices, direction);
		case 4: return HandleTetrahedronCase(vertices, direction);
		}

		return false;
	}

	bool HandleLineCase(std::vector<Vec3f>& vertices, Vec3f& direction) {
		Vec3f a = vertices[1];
		Vec3f b = vertices[0];
		Vec3f ab = b - a;
		Vec3f ao = -a;
		if(Vec3f::Dot(ab, ao) > 0.0f) {
			direction = Vec3f::Cross(Vec3f::Cross(ab, ao), ab);
		} else {
			vertices = { a };
			direction = ao;
		}
		return false;
	}

	bool HandleTriangleCase(std::vector<Vec3f>& vertices, Vec3f& direction) {
		Vec3f a = vertices[2];
		Vec3f b = vertices[1];
		Vec3f c = vertices[0];
		Vec3f ab = b - a;
		Vec3f ac = c - a;
		Vec3f ao = -a;
		Vec3f abc = Vec3f::Cross(ab, ac);

		if(Vec3f::Dot(Vec3f::Cross(abc, ac), ao) > 0) {
			if(Vec3f::Dot(ac, ao) > 0) {
				vertices = { c, a };
				direction = Vec3f::Cross(Vec3f::Cross(ac, ao), ac);
			} else {
				return HandleLineCase(vertices, direction);
			}
		} else {
			if(Vec3f::Dot(Vec3f::Cross(ab, abc), ao) > 0) {
				return HandleLineCase(vertices, direction);
			} else {
				if(Vec3f::Dot(abc, ao) > 0) {
					direction = abc;
				} else {
					vertices = { b, c, a };
					direction = -abc;
				}
			}
		}
		return false;
	}

	bool HandleTetrahedronCase(std::vector<Vec3f>& vertices, Vec3f& direction) {
		Vec3f a = vertices[3];
		Vec3f b = vertices[2];
		Vec3f c = vertices[1];
		Vec3f d = vertices[0];
		Vec3f ab = b - a;
		Vec3f ac = c - a;
		Vec3f ad = d - a;
		Vec3f ao = -a;

		Vec3f abc = Vec3f::Cross(ab, ac);
		Vec3f acd = Vec3f::Cross(ac, ad);
		Vec3f adb = Vec3f::Cross(ad, ab);

		if(Vec3f::Dot(abc, ao) > 0) {
			vertices = { c, b, a };
			direction = abc;
			return ContainsOrigin(vertices, direction);
		}

		if(Vec3f::Dot(acd, ao) > 0) {
			vertices = { d, c, a };
			direction = acd;
			return ContainsOrigin(vertices, direction);
		}

		if(Vec3f::Dot(adb, ao) > 0) {
			vertices = { b, d, a };
			direction = adb;
			return ContainsOrigin(vertices, direction);
		}

		return true;
	}


#pragma endregion

} ///- namespace



MeshCollider::MeshCollider() {}
MeshCollider::~MeshCollider() {}


/// --------------------------------
/// 初期化
/// --------------------------------
void MeshCollider::Initialize() {

	vertices_.clear();
	for(const auto& vertexData : pModel_->GetVertexDatas()) {
		vertices_.push_back(Mat4::Transform(Vec3f::Convert4To3(vertexData.position), pWorldTarnsform_->worldMatrix));
	}

}



/// --------------------------------
/// 更新
/// --------------------------------
void MeshCollider::Update() {

	vertices_.clear();
	for(const auto& vertexData : pModel_->GetVertexDatas()) {
		vertices_.push_back(Mat4::Transform(Vec3f::Convert4To3(vertexData.position), pWorldTarnsform_->worldMatrix));
	}

}



/// --------------------------------
/// 描画 : 当たり判定用メッシュの描画
/// --------------------------------
void MeshCollider::Draw() {
#ifdef _DEBUG



#endif // _DEBUG
}


void MeshCollider::DebugDraw(const std::string& windowName) {
#ifdef _DEBUG
	ImGui::Begin(windowName.c_str());

	ImGui::Text("RepeatNum : %d", repeatNum_);

	ImGui::End();
#endif // _DEBUG
}


bool MeshCollider::IsCollision(const std::vector<Vec3f>& vertices, const WorldTransform& worldTransform) {
	std::vector<Vec3f> worldVertices;
	for(const auto& vertex : vertices) {
		worldVertices.push_back(Mat4::Transform(vertex, worldTransform.worldMatrix));
	}
	
	Vec3f direction = { 1, 0, 0 };
	std::vector<Vec3f> simplex = { MinkowskiDifferenceSupport(vertices_, worldVertices, direction) };

	direction = -simplex[0];

	repeatNum_ = 0;
	while(repeatNum_ < 50) {
		Vec3f newPoint = MinkowskiDifferenceSupport(vertices_, worldVertices, direction);
		if(Vec3f::Dot(newPoint, direction) <= 0) {
			return false; // No collision
		}
		simplex.push_back(newPoint);
		if(ContainsOrigin(simplex, direction)) {
			return true; // Collision
		}

		repeatNum_++;
	}

	return false;


}

