#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

void SetOntheCircle(XMFLOAT3& a, int length, int degree) {
	if (degree > 360) degree -= 360;
	float radius = 2 * XM_PI * degree / 360;
	a.x = length * cos(radius);
	a.y = length * sin(radius);
}
void SetOntheCircle(XMFLOAT3& a, int length, float radius) {
	
	a.x = length * cos(radius);
	a.y = length * sin(radius);
}

float Degree2Radius(int degree) {
	return (XM_PI / 180) * degree;
}
int Radius2Degree(float radius) {
	return radius * 180 / XM_PI;
}


GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	debugText_ = DebugText::GetInstance();

	//ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	model_ = Model::Create();

	for (size_t i = 0; i < _countof(worldTransform_); i++) {

		Degree[i] = 36 * i;
		XMFLOAT3 position = {0, 0, 0};
		SetOntheCircle(position, length, Degree[i]);

		//スケーリング
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		//回転		  i
		worldTransform_[i].rotation_ = {0.0f, 0.0f, 0.0f};
		//平行移動		i
		worldTransform_[i].translation_ = position;

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}
	viewProjection_.Initialize();
}

void GameScene::Update() {

		static int ChangeDeg = 0;
		if (ChangeDeg > 360) ChangeDeg = 0;
		else ChangeDeg += 3;


	for (size_t i = 0; i < _countof(worldTransform_); i++) {


		XMFLOAT3 position = { 0,0,0 };
		SetOntheCircle(position, length, Degree[i] + ChangeDeg);

		worldTransform_[i].translation_ = position;
		worldTransform_[i].UpdateMatrix();
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		model_->Draw(worldTransform_[i], viewProjection_, textureHandle_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// デバッグテキストの描画
	debugText_->DrawAll(commandList);
	//
	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
