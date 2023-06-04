#pragma once
#include "Application.h"

/// <summary>
/// カメラ座標を注視点を中心とした球体として管理するためのクラス
/// </summary>
class SphericalCoordinates
{
public:
	void SetRadius(float radius);										//半径を設定する関数
	void SetElevation(float elevation);									//仰角を設定する関数
	void SetAzimth(float azimth);										//方位角を設定する関数

	SphericalCoordinates();												//コンストラクタ

	SphericalCoordinates Rotate(float newAzimuth, float newElevation);	//カメラを回転させる関数
	SphericalCoordinates Scaling(float x);								//カメラを注視点に近付ける・遠ざける関数

	XMFLOAT3 ToCartesian();												//球面座標から通常座標に変換する関数

private:
	float _radius, _azimuth, _elevation;							//それぞれ半径・方位角・仰角

	float GetRadius()const;											//半径を取得する関数
	float GetElevation()const;										//仰角を取得する関数
};
