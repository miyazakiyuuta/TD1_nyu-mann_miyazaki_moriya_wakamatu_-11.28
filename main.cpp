#include <Novice.h>
#include <stdlib.h>
#include <time.h>

const char kWindowTitle[] = "LC1C_TD(~11/28)_タイトル";
const int kWindowWidth = 1280; // 画面の横幅
const int kWindowHeight = 720; // 画面の縦幅

struct Vector2
{
	float x;
	float y;
};

struct Player
{
	Vector2 pos;
	int isAlive;
};


struct Boss
{
	Vector2 pos;
	int isAlive;
	int AttackCoolTimer;
	float speed;
	float radius;
	int isChange;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
	srand(currentTime);

	Boss boss;
	boss.pos = { 640.0f, 620.0f };
	boss.isAlive = true;
	boss.AttackCoolTimer = 0;
	boss.speed = 4.0f;
	boss.radius = 125.0f;
	boss.isChange = false;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawEllipse(static_cast<int>(boss.pos.x), static_cast<int>(boss.pos.y), static_cast<int>(boss.radius), static_cast<int>(boss.radius), 0.0f, RED, kFillModeSolid);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
