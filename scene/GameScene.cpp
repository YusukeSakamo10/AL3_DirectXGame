﻿#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <random>

using namespace DirectX;

void SubXMFloat3A4B(XMFLOAT3& A_, XMFLOAT3& B_) { 
	A_.x += B_.x;
	A_.y += B_.y;
	A_.z += B_.z;
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
		worldTransform_[i].scale_ ={ 1.0f, 1.0f, 1.0f};
		//回転
		worldTransform_[i].rotation_ = {rotDist(engine), rotDist(engine), rotDist(engine)};
		//平行移動
		worldTransform_[i].translation_ = {posDist(engine), posDist(engine), posDist(engine)};

		//ワールドトランスフォームの初期化
		worldTransform_[i].Initialize();
	}

	viewProjection_.fovAngleY = XMConvertToRadians(45.0f);

	//アスペクト比
	viewProjection_.aspectRatio = 1.0f;

	//ニアクリップ
	viewProjection_.nearZ = 52.0f;
	//ファークリップ
	viewProjection_.farZ = 53.0f;

	//ビュープロジェクションの初期化
	viewProjection_.Initialize();

}

void GameScene::Update() {

	//XMFLOAT3 move = {0, 0, 0};
	//const float kEyeSpeed = 0.2f;

	//if (input_->PushKey(DIK_W)) {
	//	move = {0, 0, kEyeSpeed};
	//} else if ( input_->PushKey(DIK_S)) {
	//	move = {0, 0, -kEyeSpeed};
	//}

	//SubXMFloat3A4B(viewProjection_.eye, move);

	//viewProjection_.UpdateMatrix();

	//カメラの変更
	/*
	//注視店移動
	const float kTargetSpeed = 0.2f;

	//押した方向で移動ベクトルを変更
	if (input_->PushKey(DIK_LEFT)) {
		move = { -kTargetSpeed, 0,0 };
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		move = { kTargetSpeed, 0,0 };
	}

	SubXMFloat3A4B(viewProjection_.target, move);
//	viewProjection_.UpdateMatrix();

	//上方向回転処理

	const float kUpRotSpeed = 0.05f;

	if (input_->PushKey(DIK_SPACE)) {
		viewAngle += kUpRotSpeed;
		//2πを超えたら0に戻す
		viewAngle = fmodf(viewAngle, XM_2PI);
	}

	viewProjection_.up = { cosf(viewAngle), sinf(viewAngle), 0.0f };

	//行列再計算
	viewProjection_.UpdateMatrix();
	

	debugText_->SetPos(50, 50);
	debugText_->Printf(
	  "eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
		"up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

	*/


	//Fov変更処理
	
	if (input_->PushKey(DIK_W)) {
		viewProjection_.fovAngleY += 0.01f;
		viewProjection_.fovAngleY = min(viewProjection_.fovAngleY, XM_PI);
	}
	else if (input_->PushKey(DIK_S)) {
		viewProjection_.fovAngleY -= 0.01f;
		viewProjection_.fovAngleY = max(viewProjection_.fovAngleY, 0.01f);
	}
	//行列の再計算::下で更新
//	viewProjection_.UpdateMatrix();

	if (input_->PushKey(DIK_UP)) {
		viewProjection_.nearZ += 0.1f;
	}
	else if (input_->PushKey(DIK_DOWN) && viewProjection_.nearZ > 0.001f) {
		viewProjection_.nearZ -= 0.1f;
	}

	viewProjection_.UpdateMatrix();


	debugText_->SetPos(50, 110);
	debugText_->Printf("fovAngleY(Degree):%f", XMConvertToDegrees(viewProjection_.fovAngleY));

	debugText_->SetPos(50, 130);
	debugText_->Printf("nearZ:%f", viewProjection_.nearZ);



	debugText_->SetPos(50, 50);
	debugText_->Printf(
		"eye:(%f,%f,%f)", viewProjection_.eye.x, viewProjection_.eye.y, viewProjection_.eye.z);

	debugText_->SetPos(50, 70);
	debugText_->Printf(
		"target:(%f,%f,%f)", viewProjection_.target.x, viewProjection_.target.y, viewProjection_.target.z);

	debugText_->SetPos(50, 90);
	debugText_->Printf(
		"up:(%f,%f,%f)", viewProjection_.up.x, viewProjection_.up.y, viewProjection_.up.z);

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
