#include "InputImage.h"


#include <ImGuiManager.h>





InputImage* InputImage::GetInstance() {
	static InputImage instance;
	return &instance;
}



void InputImage::Initialize() {
	directionFilePath_ = "./Resources/Images/";

	//inputName_ = const_cast<char*>("name");

	filePath_ = TCHAR();

}





void InputImage::Update() {
#ifdef _DEBUG

	ImGui::Begin("Image Input/Output");


	/// --------------------------------------
	/// 階層構造: 入力
	/// --------------------------------------
	if(ImGui::TreeNode("Input")) {


		if(ImGui::Button("Input")) {

		}

		ImGui::Spacing();

		static char name[256]{};
		ImGui::InputText("InputImageName", &name[0], sizeof(name));
		if(ImGui::IsItemEdited()) {
			inputImageName_ = name;
		}


		if(ImGui::Button("Import")) {

			///- 画像の読み込み
			inputImage_ = cv::imread(directionFilePath_ + inputImageName_);

			if(inputImage_.empty()) {
				///- 入力に失敗したときの処理

			} else {
				///- 入力に成功したら

				cv::imshow("ImportImage", inputImage_);

			}

		}


		ImGui::TreePop();
	}


	ImGui::Spacing();


	/// --------------------------------------
	/// 階層構造: 出力
	/// --------------------------------------
	if(ImGui::TreeNode("Output")) {

		if(ImGui::Button("SetOutputImage")) {

		}

		ImGui::Spacing();

		///- 出力する画像の名前設定
		static char name[256]{};
		ImGui::InputText("OutputImageName", &name[0], sizeof(name));
		if(ImGui::IsItemEdited()) {
			outputImageName_ = name;
		}

		if(ImGui::Button("Export")) {

			///- ここに画像の出力をいれる
			cv::imshow("outputImage", outputImage_);
			cv::imwrite(directionFilePath_ + outputImageName_, outputImage_);

		}

		ImGui::TreePop();
	}



	ImGui::Spacing();



	//ListFilesAndDirectories("./Resources/Images");
	if(ImGui::Button("SelectFilePath")) {

		GetSelectedFilePath(&filePath_);
	}


	ImGui::End();

#endif // _DEBUG


}


void InputImage::ListFilesAndDirectories(const std::filesystem::path& directory) {

	for(const auto& entry : std::filesystem::directory_iterator(directory)) {
		if(std::filesystem::is_directory(entry)) {
			// ディレクトリの場合はTreeNodeと再帰的な呼び出し
			if(ImGui::TreeNode(entry.path().filename().string().c_str())) {
				ListFilesAndDirectories(entry);
				ImGui::TreePop();
			}
		} else {
			// ファイルの場合はSelectable
			if(ImGui::Selectable(entry.path().filename().string().c_str())) {
				// ファイルが選択されたときの処理
			}
		}
	}

}


bool InputImage::GetSelectedFilePath(TCHAR* path) {
	// ファイルを開くダイアログを表示してファイルを選択する
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH] = { 0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = _T("All Files\0.\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if(GetOpenFileName(&ofn) == TRUE) {
		// ファイルパスをコピーする
		_tcscpy_s(path, MAX_PATH, szFile);
		return true;
	} else {
		// キャンセルされた場合、またはダイアログが閉じられた場合
		DWORD dwError = CommDlgExtendedError();

		if(dwError == CDERR_DIALOGFAILURE) {
			// ダイアログが閉じられたとみなす
			path[0] = '\0'; // 空の文字列をセット
			return false;
		}
	}
	return false;
}
