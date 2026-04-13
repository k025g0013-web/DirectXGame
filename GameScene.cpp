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

	delete player_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	delete skydome_;

	if (deathParticles_ != nullptr) {
		delete deathParticles_;
	}

	delete mapChipField_;

	delete cameraController_;
}

void GameScene::Initialize() {
	// カメラの初期化
	camera_.farZ = 2000.0f;
	camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 3Dモデルデータの生成
	modelBlock_ = Model::Create();

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// キューブの生成
	GenerateBlocks();

	// プレイヤー
	modelPlayer_ = Model::CreateFromOBJ("player", true);
	player_ = new Player;
	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
	player_->Initialize(modelPlayer_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	// エネミー
	modelEnemy_ = Model::CreateFromOBJ("enemy", true);
	for (int32_t i = 0; i < 3; ++i) {
		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(20, 18 - i);
		newEnemy->Initialize(modelEnemy_, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}

	// スカイドーム
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &camera_);

	// カメラコントローラー
	cameraController_ = new CameraController;
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	cameraController_->SetMovableArea({20.35f, 177.6f, 11.0f, 100.0f});
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

	switch (phase_) {
	case Phase::kPlay:
#pragma region ゲームプレイフェーズ
		// スカイドーム
		skydome_->Update();

		// プレイヤー
		player_->Update();

		// プレイヤーの位置：初期化
		if (Input::GetInstance()->TriggerKey(DIK_R)) {
			// 座標をマップチップ番号で指定
			Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);
			player_->Initialize(modelPlayer_, &camera_, playerPosition);
			player_->SetMapChipField(mapChipField_);
		}

		// エネミー
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// カメラコントローラー
		cameraController_->Update();

		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			const Camera& debugCamera = debugCamera_->GetCamera();

			camera_.matView = debugCamera.matView;             // デバッグカメラのビュー行列
			camera_.matProjection = debugCamera.matProjection; // デバッグカメラのプロジェクション行列

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// カメラコントローラーのトランスフォームを参照
			camera_.translation_ = cameraController_->GetCameraTranslation();

			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				// ワールド行列更新
				UpdateWorldTransform(*worldTransformBlock);
			}
		}

		// 全ての当たり判定を行う
		CheckAllCollision();

		ChangePhase();
#pragma endregion
		break;
	case Phase::kDeath:
#pragma region デス演出フェーズ
		// スカイドーム
		skydome_->Update();

		// エネミー
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// 死亡時パーティクル
		if (deathParticles_ != nullptr) {
			deathParticles_->Update();
		}
		
		// カメラコントローラー
		cameraController_->Update();

		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			const Camera& debugCamera = debugCamera_->GetCamera();

			camera_.matView = debugCamera.matView;             // デバッグカメラのビュー行列
			camera_.matProjection = debugCamera.matProjection; // デバッグカメラのプロジェクション行列

			// ビュープロジェクション行列の転送
			camera_.TransferMatrix();
		} else {
			// カメラコントローラーのトランスフォームを参照
			camera_.translation_ = cameraController_->GetCameraTranslation();

			// ビュープロジェクション行列の更新と転送
			camera_.UpdateMatrix();
		}

		// ブロックの更新
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				// ワールド行列更新
				UpdateWorldTransform(*worldTransformBlock);
			}
		}

		// ゲームシーンの終了
		if (deathParticles_->IsFinished()) {
			finished_ = true;
		}
#pragma endregion
		break;
	}
}

void GameScene::Draw() {
	Model::PreDraw(); // 開始

	switch (phase_) {
	case Phase::kPlay:
#pragma region ゲームプレイフェーズ
		// スカイドーム
		skydome_->Draw();
		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				modelBlock_->Draw(*worldTransformBlock, camera_);
			}
		}

		// プレイヤー
		player_->Draw();

		// エネミー
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}
#pragma endregion
		break;
	case Phase::kDeath:
#pragma region デス演出フェーズ
		// スカイドーム
		skydome_->Draw();

		// ブロックの描画
		for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
				if (!worldTransformBlock)
					continue;

				modelBlock_->Draw(*worldTransformBlock, camera_);
			}
		}

		// エネミー
		for (Enemy* enemy : enemies_) {
			enemy->Draw();
		}

		// 死亡時パーティクル
		if (deathParticles_ != nullptr) {
			deathParticles_->Draw();
		}
#pragma endregion
		break;
	};

	Model::PostDraw(); // 終了
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t kNumBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t kNumBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(kNumBlockVirtical);
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		// 1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollision() {
#pragma region 自キャラと敵キャラの当たり判定
	// 判定対象1と2の座標
	AABB aabb1, aabb2;

	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵弾全ての当たり判定
	for (Enemy* enemy : enemies_) {
		// 敵弾の座標
		aabb2 = enemy->GetAABB();

		// AABB同士の交差判定
		if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && (aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && (aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
			// 自キャラの衝突時関数を呼び出す
			player_->OnCollision(enemy);
			// 敵の衝突時関数を呼び出す
			enemy->OnCollision(player_);
		}
	}
#pragma endregion
};

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kPlay:
		//自キャラがデス状態
		if (player_->IsDead()) {
			// 死亡演出フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();

			// 自キャラの座標にデスパーティクルを発生、初期化
			modelDeathParticles_ = Model::CreateFromOBJ("deathParticle", true);
			deathParticles_ = new DeathParticles;
			deathParticles_->Initialize(modelDeathParticles_, &camera_, deathParticlesPosition);
		}
		break;
	case Phase::kDeath:
		break;
	}
};