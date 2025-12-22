#include "GameScene.h"

using namespace KamataEngine;

// コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete modelBlock_;
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	delete debugCamera_;
}

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 3Dモデルデータの生成
	modelBlock_ = Model::Create();

	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;
	// ブロック1個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// キューブの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if ((i + j) % 2 == 1) {
				worldTransformBlocks_[i][j] = nullptr;
				continue;
			}

			worldTransformBlocks_[i][j] = new WorldTransform();
			worldTransformBlocks_[i][j]->Initialize();
			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
		}
	}
}

void GameScene::Update() {
	// デバッグカメラの更新
	debugCamera_->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// デバッグカメラ有効フラグをトグル
		isDebugCameraActive_ = !isDebugCameraActive_;
	}
#endif

	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		const Camera& debugCamera = debugCamera_->GetCamera();

		camera_.matView = debugCamera.matView;             // デバッグカメラのビュー行列
		camera_.matProjection = debugCamera.matProjection; // デバッグカメラのプロジェクション行列
							
		// ビュープロジェクション行列の転送
		camera_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		camera_.UpdateMatrix();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			// アフィン変換行列の作成
			Matrix4x4 affineMatrix = {};

			for (int i = 0; i < 4; i++) {
				affineMatrix.m[i][i] = 1.0f;
			}

			// アフィン変換行列
			affineMatrix.m[0][0] = worldTransformBlock->scale_.x * cosf(worldTransformBlock->rotation_.y) * cosf(worldTransformBlock->rotation_.z);
			affineMatrix.m[0][1] = worldTransformBlock->scale_.x * cosf(worldTransformBlock->rotation_.y) * sinf(worldTransformBlock->rotation_.z);
			affineMatrix.m[0][2] = worldTransformBlock->scale_.x * -sinf(worldTransformBlock->rotation_.y);

			affineMatrix.m[1][0] = worldTransformBlock->scale_.y * sinf(worldTransformBlock->rotation_.x) * sinf(worldTransformBlock->rotation_.y) * cosf(worldTransformBlock->rotation_.z) -
			                       cosf(worldTransformBlock->rotation_.y) * sinf(worldTransformBlock->rotation_.z);
			affineMatrix.m[1][1] = worldTransformBlock->scale_.y * sinf(worldTransformBlock->rotation_.x) * sinf(worldTransformBlock->rotation_.y) * sinf(worldTransformBlock->rotation_.z) +
			                       cosf(worldTransformBlock->rotation_.y) * cosf(worldTransformBlock->rotation_.z);
			affineMatrix.m[1][2] = worldTransformBlock->scale_.y * sinf(worldTransformBlock->rotation_.x) * cosf(worldTransformBlock->rotation_.y);

			affineMatrix.m[2][0] = worldTransformBlock->scale_.z * cosf(worldTransformBlock->rotation_.y) * sinf(worldTransformBlock->rotation_.y) * cosf(worldTransformBlock->rotation_.z) +
			                       sinf(worldTransformBlock->rotation_.x) * sinf(worldTransformBlock->rotation_.z);
			affineMatrix.m[2][1] = worldTransformBlock->scale_.z * cosf(worldTransformBlock->rotation_.y) * sinf(worldTransformBlock->rotation_.y) * sinf(worldTransformBlock->rotation_.z) -
			                       sinf(worldTransformBlock->rotation_.x) * cosf(worldTransformBlock->rotation_.z);
			affineMatrix.m[2][2] = worldTransformBlock->scale_.z * cosf(worldTransformBlock->rotation_.y) * cosf(worldTransformBlock->rotation_.y);

			affineMatrix.m[3][0] = worldTransformBlock->translation_.x;
			affineMatrix.m[3][1] = worldTransformBlock->translation_.y;
			affineMatrix.m[3][2] = worldTransformBlock->translation_.z;

			// アフィン変換行列の代入
			worldTransformBlock->matWorld_ = affineMatrix;

			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}
}

void GameScene::Draw() {
	Model::PreDraw(); // 開始

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	Model::PostDraw(); // 終了
}
