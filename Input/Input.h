#pragma once

#include "Vector2.h"



class Input final {
public:

	struct Mouse {
		bool leftButton_ = false;
		bool preLeftButton_ = false;
	};


	static Input* GetInstance();

	void Init();

	void Begin();

	/// <summary>
	/// winAppでセットする用; 他での使用禁止
	/// </summary>
	void SetMousePos(const Vec2f& position) {
		mousePos_ = position;
	}

	void SetMouseLeftButton(bool isPush) {
		mouse_.leftButton_ = isPush;
	}

	const Vec2f& GetMousePos() const { return mousePos_; }
	const Mouse& GetMouse() const { return mouse_; }

private:


	Vec2f mousePos_ = { 0.0f,0.0f };
	Mouse mouse_;

private:

	Input() = default;
	~Input() = default;

	Input& operator=(const Input&) = delete;
	Input(const Input&) = delete;
	Input(Input&&) = delete;

};


