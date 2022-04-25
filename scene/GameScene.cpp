#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

using namespace DirectX;

void SumXMFloat3(XMFLOAT3& a, XMFLOAT3& b) {
	a.x += b.x;
	a.y += b.y;
	a.z += b.z;
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

	// 3Dモデルの生成
	model_ = Model::Create();

	//画像に対する

	//キャラクターの大元
	//スケーリング
	worldTransform_[PartId::ROOT].scale_ = {1.0f, 1.0f, 1.0f};
	//回転
	worldTransform_[PartId::ROOT].rotation_ = {0.0f, 0.0f, 0.0f};
	//平行移動
	worldTransform_[PartId::ROOT].translation_ = {0.0f, 0.0f, 0.0f};

	//ワールドトランスフォームの初期化
	worldTransform_[0].Initialize();

	//脊椎
	worldTransform_[PartId::SPINE].translation_ = { 0, 4.5f,0 };
	worldTransform_[PartId::SPINE].parent_ = &worldTransform_[PartId::ROOT];
	worldTransform_[PartId::SPINE].Initialize();

	//上半身
	worldTransform_[PartId::CHEST].translation_ = { 0, -4.5f,0 };
	worldTransform_[PartId::CHEST].parent_ = &worldTransform_[PartId::SPINE];
	worldTransform_[PartId::CHEST].Initialize();

	worldTransform_[PartId::HEAD].translation_ = { 0, 4.5f,0 };
	worldTransform_[PartId::HEAD].parent_ = &worldTransform_[PartId::CHEST];
	worldTransform_[PartId::HEAD].Initialize();

	worldTransform_[PartId::ARML].translation_ = { 5.0f, 0,0 };
	worldTransform_[PartId::ARML].parent_ = &worldTransform_[PartId::CHEST];
	worldTransform_[PartId::ARML].Initialize();

	worldTransform_[PartId::ARMR].translation_ = { -5.0, 0,0 };
	worldTransform_[PartId::ARMR].parent_ = &worldTransform_[PartId::CHEST];
	worldTransform_[PartId::ARMR].Initialize();

	//下半身

	worldTransform_[PartId::HIP].translation_ = { 0, 8.0f,0 };
	worldTransform_[PartId::HIP].parent_ = &worldTransform_[PartId::SPINE];
	worldTransform_[PartId::HIP].Initialize();

	worldTransform_[PartId::LEGL].translation_ = { 5.0f, -4.5f,0 };
	worldTransform_[PartId::LEGL].parent_ = &worldTransform_[PartId::HIP];
	worldTransform_[PartId::LEGL].Initialize();

	worldTransform_[PartId::LEGR].translation_ = { -5.0f, -4.5f,0 };
	worldTransform_[PartId::LEGR].parent_ = &worldTransform_[PartId::HIP];
	worldTransform_[PartId::LEGR].Initialize();


	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void GameScene::Update() {

	XMFLOAT3 move = { 0,0,0 };

	const float kCharacterSpeed = 0.2f;

	if (input_->PushKey(DIK_LEFT)) {
		move = { -kCharacterSpeed, 0,0 };
	}
	else if (input_->PushKey(DIK_RIGHT)) {
		move = { kCharacterSpeed, 0,0 };
	}

	SumXMFloat3(worldTransform_[PartId::ROOT].translation_, move);

	debugText_->SetPos(50, 150);
	debugText_->Printf("Root:(%f,%f,%f)",
		worldTransform_[PartId::ROOT].translation_.x,
		worldTransform_[PartId::ROOT].translation_.y,
		worldTransform_[PartId::ROOT].translation_.z
	);

	worldTransform_[0].UpdateMatrix();
	//子どもアップデート
	for (size_t i = 1; i < PartId::PARTIDEND; i++) {
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
	// 3Dモデルの描画
	model_->Draw(worldTransform_[0], viewProjection_, textureHandle_);
	for (size_t i = 1; i < _countof(worldTransform_); i++) {
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
