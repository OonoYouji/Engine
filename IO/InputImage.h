#pragma once


#include <opencv.hpp>
#include <imgui.h>


#include <string>
#include <iostream>
#include <filesystem>
#include <tchar.h>


class InputImage final {
public:

	static InputImage* GetInstance();

	void Initialize();

	void Update();

	/// <summary>
	/// 入力した画像 getter
	/// </summary>
	const cv::Mat& GetInputImage() const { return inputImage_; }

	void SetOutputImage(const cv::Mat& mat) {
		outputImage_ = mat;
	}

private:


	///- 画像を保存するフォルダへのファイルパス  "./Resources/Images/"
	std::string directionFilePath_;

	std::string outputImageName_;
	std::string inputImageName_;

	cv::Mat inputImage_;
	cv::Mat outputImage_;

	TCHAR filePath_;

	void ListFilesAndDirectories(const std::filesystem::path& directory);
	bool GetSelectedFilePath(TCHAR* path);

private:

	InputImage() = default;
	~InputImage() = default;

	InputImage& operator=(const InputImage&) = delete;
	InputImage(const InputImage&) = delete;
	InputImage(InputImage&&) = delete;

};