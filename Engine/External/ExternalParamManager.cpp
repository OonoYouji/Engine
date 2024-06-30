#include <ExternalParamManager.h>




/// ===================================================
/// インスタンス確保
/// ===================================================
ExternalParamManager* ExternalParamManager::GetInstance() {
	static ExternalParamManager instance;
	return &instance;
}



/// ===================================================
/// グループの生成
/// ===================================================
Epm::Group& ExternalParamManager::CreateGroup(const std::string& key) {
	return datas_[key];
}


