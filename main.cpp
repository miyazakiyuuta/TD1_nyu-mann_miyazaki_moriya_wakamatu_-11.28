#include <Novice.h>

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
	Vector2 posW;
	float width;
	float height;
	float radius;
	float speed;
	float jump;
	float gravity;
	int isJump;
	int isAlive;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	//==============================================
	//変数初期化
	//==============================================

	//プレイヤー
	Player player;
	player.pos.x = 100.0f; //ｘ座標
	player.pos.y = 100.0f; //ｙ座標
	player.posW.x = 100.0f; //ｘ座標(ワールド)
	player.posW.y = 100.0f; //ｙ座標(ワールド)
	player.width = 32.0f; //縦幅
	player.height = 32.0f; //横幅
	player.radius = 16.0f; //半径
	player.speed = 5.0f; //移動速度
	player.jump = 15.0f; //ジャンプ速度
	player.gravity = 0.0f; //重力
	player.isJump = false; //ジャンプ状態か否か
	player.isAlive; //生存

	float scaffold = 620.0f; //地面
	float scrollX = 0.0f; //スクリーンｘ座標
	float scrollY = 0.0f; //スクリーンｙ座標
	float monitorX = 0.0f; //モニターｘ座標
	float monitorY = 0.0f; //モニターｙ座標

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

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

		//===========================================================
		//移動
		//===========================================================

		//とりあえずキーボードで追加
		if (keys[DIK_A]) {
			player.pos.x -= player.speed;
		}

		if (keys[DIK_D]) {
			player.pos.x += player.speed;
		}

		// ジャンプ
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			player.isJump = true;
		}
		if (player.isJump) {
			player.pos.y -= player.jump;
		}

		//重力
		if (player.pos.y + player.radius < scaffold) {
			player.pos.y += player.gravity += 0.7f;
		} else {
			player.gravity = 0.0f;
		}

		//地面に着地した時
		if (player.pos.y + player.radius >= scaffold) {
			player.pos.y = scaffold - player.radius;
			player.isJump = false;
		}

		//モニター座標
		monitorX = player.pos.x - scrollX;
		monitorY = player.pos.y - scrollY;
		player.posW.x = monitorX;
		player.posW.y = monitorY;


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawLine(0, 620, 1280, 620, RED);

		Novice::DrawBox
		(
			static_cast<int>(player.posW.x - player.width / 2.0f),
			static_cast<int>(player.posW.y - player.height / 2.0f),
			static_cast<int>(player.width),
			static_cast<int>(player.height),
			0.0f, WHITE, kFillModeSolid
		);

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
