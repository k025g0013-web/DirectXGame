#pragma once
#include "KamataEngine.h"
#include <map>
#include <vector>

// マップチップ種別の定義
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

// マップチップデータ構造体
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

// マップチップテーブル
namespace {
std::map<std::string, MapChipType> mapChipTable = {
    {"0", MapChipType::kBlank},
    {"1", MapChipType::kBlock},
};
}

class MapChipField {
public:
	MapChipField();  // コンストラクタ
	~MapChipField(); // デストラクタ

	// リセット
	void ResetMapChipData();

	// 読み込み
	void LoadMapChipCsv(const std::string& filePath);

	// マップチップ種別の取得
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	
	// マップチップ座標の取得
	KamataEngine::Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex); 

	// 縦方向の個数を取得
	uint32_t GetNumBlockVirtical();

	// 横方向の個数を取得
	uint32_t GetNumBlockHorizontal();

private:
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 2.0f;
	static inline const float kBlockHeight = 2.0f;

	// ブロックの偶数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	// マップチップデータ構造体
	MapChipData mapChipData_;
};