#define NOMINMAX
#include "Fade.h"
#include <algorithm>

using namespace KamataEngine;

// コンストラクタ
Fade::Fade() {}

// デストラクタ
Fade::~Fade() {}

void Fade::Initialize() {
	textureHandle_ = TextureManager::Load("white1x1.png");

	// スプライトを持たせる
	sprite_ = Sprite::Create(textureHandle_, {0.0f, 0.0f});
	sprite_->SetSize(Vector2(1280.0f, 720.0f));
	sprite_->SetColor(Vector4(0, 0, 0, 1));
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		// 何もしない
		break;
	case Status::FadeIn:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);

		// 0.0fから1.0fまでの間で、経過時間がフェード継続時間に近づくほどアルファ値を小さくする
		sprite_->SetColor(Vector4(0, 0, 0, 1.0f - std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	case Status::FadeOut:
		// 1フレーム分の秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		counter_ = std::min(counter_, duration_);

		// 0.0fから1.0fまでの間で、経過時間がフェード継続時間に近づくほどアルファ値を大きくする
		sprite_->SetColor(Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)));
		break;
	}
}

void Fade::Draw() { 
	if (status_ == Status::None) return;

	Sprite::PreDraw();
	sprite_->Draw();
	Sprite::PostDraw();
}

bool Fade::IsFinished() const {
	// フェード状態による分岐
	switch (status_) { 
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}

	return true;
};