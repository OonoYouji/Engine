#pragma once

/// <summary>
/// 色々なクラスのメモリ解放を楽々できる関数
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="t"></param>
template <class T>
inline void SafeDelete(T*& t) {
	delete t;
	t = nullptr;
}