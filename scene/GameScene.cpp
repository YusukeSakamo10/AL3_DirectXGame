#include "GameScene.h"
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
	debugText_ = DebugText::GetInstance();
	//画像の読み込み
	textureHandle_ = TextureManager::Load("mario.jpg");

	// 3Dモデルの生成
	model_ = Model::Create();

	std::random_device seed_gen;
	std::mt19937_64 engine(seed_gen());
	std::uniform_real_distribution<float> rotDist(0.0f, XM_2PI);
	std::uniform_real_distribution<float> posDist(-10.0f, 10.0f);

	//画像に対する
	//スケーリング
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};
	//回転
	worldTransform_.rotation_ = {0, 0, 0};
	//平行移動
	worldTransform_.translation_ = {0, 0, 0};

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	for (size_t i = 0; i < 3; i++) {
		//カメラ視点座標を設定
		viewProjection_[i].eye = {posDist(engine), posDist(engine), posDist(engine)};

		//注視店
		viewProjection_[i].target = {0, 0, 0};

		//カメラ上方向ベクトルを設定
		viewProjection_[i].up = {0, 1.0f, 0.0f};

		//ビュープロジェクションの初期化
		viewProjection_[i].Initialize();
	}
}

void GameScene::Update() {

	if (input_->TriggerKey(DIK_SPACE)) {
		if (CameraNum == 2) {
			CameraNum = 0;
		} else
			CameraNum++;
	}

	viewProjection_[CameraNum].UpdateMatrix();

	for (size_t i = 0; i < 3; i++) {
		int debugPosX = 50;
		int debugPosY = 50 + (100*i);
		int debugSpace = 20;
		debugText_->SetPos(debugPosX, debugPosY);
		debugText_->Printf("Camera%d", i + 1);

		debugPosY += debugSpace;
		debugText_->SetPos(debugPosX, debugPosY);
		debugText_->Printf(
		  "eye:(%f,%f,%f)", viewProjection_[i].eye.x, viewProjection_[i].eye.y, viewProjection_[i].eye.z);

		debugPosY += debugSpace;
		debugText_->SetPos(debugPosX, debugPosY);
		debugText_->Printf(
		  "target:(%f,%f,%f)", viewProjection_[i].target.x, viewProjection_[i].target.y,
		  viewProjection_[i].target.z);

		debugPosY += debugSpace;

		debugText_->SetPos(debugPosX, debugPosY);
		debugText_->Printf(
		  "up:(%f,%f,%f)", viewProjection_[i].up.x, viewProjection_[i].up.y, viewProjection_[i].up.z);
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

	model_->Draw(worldTransform_, viewProjection_[CameraNum], textureHandle_);

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
