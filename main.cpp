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
struct Boss
{
	Vector2 pos;
	Vector2 attackPos;
	int isAlive;
	int attackCoolTimer;
	float speed;
	int isChange;
	int hpCount;
	float width;
	float height;
	float attackSpeed;
};

struct Sword
{
	Vector2 pos;
	float width;
	float height;
	float radius;
	int coolTime;
	int durationTime;
	int isAtk;
};

//スクリーン座標変換用関数
float ToScreen(float posY)
{
	const float kWorldToScreenTranslate = 620.0f;
	const float kWorldToScreenScale = -1.0f;
	return (posY * kWorldToScreenScale) + kWorldToScreenTranslate;
}

//矩形の当たり判定用関数
void IsHit(Vector2 leftTopA, float widthA, float heightA, Vector2 leftTopB, float widthB, float heightB, int& isHit)
{
	float ax1 = leftTopA.x;
	float ay1 = leftTopA.y;
	float ax2 = leftTopA.x + widthA;
	float ay2 = leftTopA.y - heightA;
	float bx1 = leftTopB.x;
	float by1 = leftTopB.y;
	float bx2 = leftTopB.x + widthB;
	float by2 = leftTopB.y - heightB;
	if (bx1 < ax2 && ax1 < bx2)//x座標が重なっているとき
	{
		if (by1 > ay2 && ay1 > by2)
		{
			isHit = true;
		}
	}
}

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
	player.speed = 15.0f; //移動速度
	player.jump = 15.0f; //ジャンプ速度
	player.gravity = 0.0f; //重力
	player.isJump = false; //ジャンプ状態か否か
	player.isAlive; //生存


	Sword shortSword;
	shortSword.pos.x = 100.0f; //ｘ座標
	shortSword.pos.y = 100.0f; //ｙ座標
	shortSword.width = 64.0f; //縦幅
	shortSword.height = 64.0f; //横幅
	shortSword.radius = 32.0f; //半径
	shortSword.coolTime = 0; //攻撃クールタイム
	shortSword.durationTime = 30; //攻撃の持続時間
	shortSword.isAtk = false; //攻撃しているか

	Sword longSword;
	longSword.pos.x = 100.0f; //ｘ座標
	longSword.pos.y = 100.0f; //ｙ座標
	longSword.width = 64.0f; //縦幅
	longSword.height = 64.0f; //横幅
	longSword.radius = 32.0f; //半径
	longSword.coolTime = 0; //攻撃クールタイム
	longSword.durationTime = 30; //攻撃の持続時間
	longSword.isAtk = false; //攻撃しているか


	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
	srand(currentTime);

	Boss boss;
	boss.pos = { 960.0f,100.0f };
	boss.speed = 10.0f;
	boss.attackCoolTimer = 60;
	boss.isAlive = true;
	boss.isChange = false;
	boss.hpCount = 100;
	boss.width = 300;
	boss.height = 200;
	//ボス攻撃
	boss.attackCoolTimer = 60;
	boss.attackPos = boss.pos;
	boss.attackSpeed = 10.0f;
  
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
		//プレイヤー
		//===========================================================

		//とりあえずキーボードで追加

		//左右移動
		if (keys[DIK_A])
		{
			player.pos.x -= player.speed;
		}

		if (keys[DIK_D])
		{
			player.pos.x += player.speed;
		}

		// ジャンプ
		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE])
		{
			player.isJump = true;
		}
		if (player.isJump)
		{
			player.pos.y += player.jump;
		}

		//攻撃

		if (keys[DIK_J] && !preKeys[DIK_J]) //短剣
		{
			if (!longSword.isAtk) //大剣攻撃時は使えない
			{
				if (shortSword.coolTime <= 0) //クールタイムが０以下の時のみ
				{
					shortSword.isAtk = true;
					shortSword.coolTime = 20;
				}
			}
		}

		if (keys[DIK_K] && !preKeys[DIK_K]) //大剣
		{
			if (!shortSword.isAtk) //短剣攻撃時は使えない
			{
				if (longSword.coolTime <= 0) //クールタイムが０以下の時のみ
				{
					longSword.isAtk = true;
					longSword.coolTime = 40;
				}
			}
		}

		//クールタイム
		if (shortSword.coolTime >= 0) //短剣
		{
			shortSword.coolTime--;
		}

		if (longSword.coolTime >= 0) //大剣
		{
			longSword.coolTime--;
		}

		//攻撃の持続時間
		if (shortSword.isAtk) //短剣
		{
			if (shortSword.durationTime >= 0)
			{
				shortSword.durationTime--;
			} 
			else
			{
				shortSword.isAtk = false;
				shortSword.durationTime = 30;
			}
		}

		if (longSword.isAtk) //大剣
		{
			if (longSword.durationTime >= 0)
			{
				longSword.durationTime--;
			} else
			{
				longSword.isAtk = false;
				longSword.durationTime = 30;
			}
		}

		//重力
		if (player.pos.y - player.width / 2.0f > 0.0f)
		{
			player.pos.y += player.gravity -= 0.7f;

		} 
		else
		{
			player.gravity = 0.0f;
		}

		//地面に着地した時
		if (player.pos.y - player.width / 2.0f <= 0.0f)
		{
			player.pos.y = player.width / 2.0f;
			player.isJump = false;
		}



		//===========================================================
		//ボスの移動
		//===========================================================

		//とりあえずキーボードで追加

		if (boss.hpCount <= 100)
		{
			if (keys[DIK_P] && !preKeys[DIK_P])
			{
				
			}
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		//地面
		Novice::DrawLine(0, 620, 1280, 620, RED);

		//プレイヤー
		Novice::DrawBox
		(
			static_cast<int>(player.pos.x - player.width / 2.0f),
			static_cast<int>(ToScreen(player.pos.y + player.height / 2.0f)),
			static_cast<int>(player.width),
			static_cast<int>(player.height),
			0.0f, WHITE, kFillModeSolid
		);

		//攻撃が出ているかテスト	

		if (shortSword.isAtk) //短剣(持続時)
		{
			Novice::DrawBox(0,0,60,60,0.0f, RED, kFillModeSolid);
		}

		if (longSword.isAtk) //大剣(持続時)
		{
			Novice::DrawBox(0, 0, 60, 60, 0.0f, BLUE, kFillModeSolid);
		}


		Novice::DrawEllipse(static_cast<int>(boss.pos.x), static_cast<int>(boss.pos.y), static_cast<int>(boss.radius), static_cast<int>(boss.radius), 0.0f, RED, kFillModeSolid);


		Novice::DrawBox(
			static_cast<int>(boss.pos.x),
			static_cast<int>(ToScreen(boss.pos.y+boss.height/2.0f)),
			static_cast<int>(boss.width),
			static_cast<int>(boss.height),
			0.0f, RED, kFillModeSolid);

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
