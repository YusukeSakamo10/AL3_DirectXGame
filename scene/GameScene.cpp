#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

void DegOntheCircle(XMFLOAT3& a, int r, float degree) {
	float radius = 2 * XM_PI * degree / 360;
	a.x = r * cos(radius);
	a.y = r * sin(radius);
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

		XMFLOAT3 position = {0, 0, 0};
		float radius = 40 * i;
		DegOntheCircle(position, 10, radius);

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
	float deg = 0;
	deg += 15;
	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		
		float r = 40;
		float radius =2 * XM_PI * deg / 360;
		
		worldTransform_[i].translation_.x = r* cos(radius);
		worldTransform_[i].translation_.y = r* sin(radius);

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
