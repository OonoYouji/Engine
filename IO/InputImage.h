#pragma once


#include <opencv.hpp>
#include <imgui.h>


#include <string>


class InputImage final {
public:

	static InputImage* GetInstance();

	void Initialize();

	void Update();

	/// <summary>
	/// 入力した画像 getter
	/// </summary>
	const cv::Mat& GetInputImage() const { return inputImage_; }


private:


	///- 画像を保存するフォルダへのファイルパス  "./Resources/Images/"
	std::string directionFilePath_;

	std::string outputImageName_;
	std::string inputImageName_;

	cv::Mat inputImage_;





private:

	InputImage() = default;
	~InputImage() = default;

	InputImage& operator=(const InputImage&) = delete;
	InputImage(const InputImage&) = delete;
	InputImage(InputImage&&) = delete;

};