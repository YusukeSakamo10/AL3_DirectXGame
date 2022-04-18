﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

void SubView2Sight(XMFLOAT3& view_, XMFLOAT3& sight_) { 
	view_.x += sight_.x;
	view_.y += sight_.y;
	view_.z += sight_.z;
}

GameScene::GameScene() {}

GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	debugText_ = DebugText::GetInstance();

	
	//画像の読み込み
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデルの生成
	model_ = Model::Create();


	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	for (size_t i = 0; i < _countof(worldTransform_); i++) {
		//画像に対する
		//スケーリング
		worldTransform_[i].scale_ = {1.0f, 1.0f, 1.0f};
		//回転
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		//平行移動
		worldTransform_[i].translation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	//カメラ視点座標を設定
	viewProjection_.eye = {0, 0, -10};

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

}

void GameScene::Update() {

	XMFLOAT3 move = {0, 0, 0};
	const float kEyeSpeed = 0.2f;

	if (input_->PushKey(DIK_W)) {
		move = {0, 0, kEyeSpeed};
	} else if ( input_->PushKey(DIK_S)) {
		move = {0, 0, -kEyeSpeed};
	}

	SubView2Sight(viewProjection_.eye, move);
	viewProjection_.UpdateMatrix();

	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

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
	
	//3Dモデルの描画
	//複数の場合
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
