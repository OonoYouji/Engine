#include "InputImage.h"


#include <ImGuiManager.h>





InputImage* InputImage::GetInstance() {
	static InputImage instance;
	return &instance;
}



void InputImage::Initialize() {
	directionFilePath_ = "./Resources/Images/";

	//inputName_ = const_cast<char*>("name");
	


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
		ImGui::InputText("OutputImageName", &outputImageName_[0], sizeof(outputImageName_));
		if(ImGui::Button("Export")) {

			///- ここに画像の出力をいれる

		}

		ImGui::TreePop();
	}



	ImGui::Spacing();



	ImGui::End();

#endif // _DEBUG


}
