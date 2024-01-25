#pragma once

/// <summary>
/// シーン遷移用基底クラス
/// </summary>
class ITransition {
public: // member method

	// 仮想デストラクタ
	virtual ~ITransition() = default;

	// default method
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

protected: // member object

	// 遷移の折り返し地点で true にする
	bool isReturn_;
	bool preIsReturn_;

	// 遷移が終了したら true にする
	bool isEnd_;

public: // accessor

	bool GetIsEnd(void) const { return isEnd_; }
	bool GetTriggerIsReturn(void) const { return isReturn_ && !preIsReturn_; }
};

