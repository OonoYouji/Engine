#pragma once

#include "Vector2.h"


class Input final {
public:

	static Input* GetInstance();

	void Init();

	void Begin();

	/// <summary>
	/// winAppでセットする用; 他での使用禁止
	/// </summary>
	void SetMousePos(const Vec2f& position) {
		mousePos_ = position;
	}

	const Vec2f& GetMousePos() const { return mousePos_; }

private:

	Vec2f mousePos_ = { 0.0f,0.0f };


private:

	Input() = default;
	~Input() = default;

	Input& operator=(const Input&) = delete;
	Input(const Input&) = delete;
	Input(Input&&) = delete;

};


