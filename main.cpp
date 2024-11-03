#include <Novice.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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
	int isDirections;
};
struct Boss
{
	Vector2 pos;
	int isAlive;
	int attackCoolTimer;
	float speed;
	int isChange;
	int hpCount;
	float width;
	float height;
	int fireCoolTimer;
	int isAttacking;
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
	int isBossHit;
	int damage;
};

struct Attack
{
	Vector2 pos;
	float width;
	float height;
	float speed;
	int isShot;
	float gravity;
	Vector2 direction;
};

enum SCENE
{
	GAMETITLE,
	GAMEPLAY,
	GAMEOVER,
	GAMECLEAR
};

enum ATTACK
{
	SMALLFIRE,
	BIGFIRE
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

	int scene = GAMEPLAY;

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
	player.isDirections = false; //プレイヤーの向いている方向(false = 右,true = 左)


	Sword shortSword;
	shortSword.pos.x = 100.0f; //ｘ座標
	shortSword.pos.y = 100.0f; //ｙ座標
	shortSword.width = 64.0f; //縦幅
	shortSword.height = 64.0f; //横幅
	shortSword.radius = 32.0f; //半径
	shortSword.coolTime = 0; //攻撃クールタイム
	shortSword.durationTime = 30; //攻撃の持続時間
	shortSword.isAtk = false; //攻撃しているか
	shortSword.isBossHit = false; //攻撃が当たっているか(ボスに)
	shortSword.damage = 3; //攻撃力

	Sword longSword;
	longSword.pos.x = 100.0f; //ｘ座標
	longSword.pos.y = 100.0f; //ｙ座標
	longSword.width = 64.0f; //縦幅
	longSword.height = 64.0f; //横幅
	longSword.radius = 32.0f; //半径
	longSword.coolTime = 0; //攻撃クールタイム
	longSword.durationTime = 30; //攻撃の持続時間
	longSword.isAtk = false; //攻撃しているか
	longSword.isBossHit = false; //攻撃が当たっているか(ボスに)
	longSword.damage = 5; //攻撃力


	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
	srand(currentTime);

	Boss boss;
	boss.pos = { 1000.0f, 160.0f }; // 座標
	boss.speed = 10.0f; // 移動速度
	boss.attackCoolTimer = 0; // 攻撃のクールタイム
	boss.isAlive = true; // 生きているか
	boss.isChange = false; // 形態変化用のフラグ
	boss.hpCount = 100; // 体力
	boss.width = 120.0f; // 横幅
	boss.height = 162.0f; // 縦幅
	//ボス攻撃
	boss.attackCoolTimer = 60;
	boss.fireCoolTimer = 0; // 小炎攻撃用のタイマー
	boss.isAttacking = false;

	const int smallFireMax = 8; // 小炎の最大数

	int fireShootCount = 0; // 炎を撃ったカウント
	int fireDisappearCount = 0; // 炎が消えたカウント

	Attack smallFire[9];

	for (int i = 0; i < smallFireMax; i++)
	{
		smallFire[i].pos = { 0.0f, 0.0f }; // 座標
		smallFire[i].width = 32.0f; // 横幅
		smallFire[i].height = 32.0f; // 縦幅
		smallFire[i].speed = 5.0f; // 速度
		smallFire[i].isShot = false; // 撃たれたか
		smallFire[i].gravity = 0.0f;
	}

	smallFire[8].pos = { 0.0f, 0.0f }; // 座標
	smallFire[8].width = 32.0f; // 横幅
	smallFire[8].height = 32.0f; // 縦幅
	smallFire[8].speed = 15.0f; // 速度
	smallFire[8].isShot = false; // 撃たれたか
	smallFire[8].direction = { 0.0f };

	float f2pDistance = 0.0f; // 炎とプレイヤーの距離

	int attackTypeFirst = 0; // 第一形態の技の種類

	int ghBoss1 = ghBoss1 = Novice::LoadTexture("./Resources/images/TD1.png"); // 第一形態のボスの画像
	int ghBoss1Move = 0; // ボスの画像の左上の座標

	int frameCount = 0;
	int bossAnimCount = 0;
	int bossCountChange = 0;
	float ghBoss1TotalWidth = 1920.0f;
	float ghBoss1Width = 296.0f;

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
        
		/*シーン切り替え*/
		switch (scene)
		{
		case GAMETITLE:
			break;
		case GAMEPLAY:
			break;
		case GAMEOVER:
			break;
		case GAMECLEAR:
			break;
		}
		if (scene == GAMEPLAY)
		{

			//===========================================================
			//プレイヤー
			//===========================================================

			//とりあえずキーボードで追加

			//左右移動
			if (keys[DIK_A])
			{
				player.pos.x -= player.speed;
				player.isDirections = true;
			}

			if (keys[DIK_D])
			{
				player.pos.x += player.speed;
				player.isDirections = false;
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

			//==============================================================
			//攻撃
			//==============================================================

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
					player.gravity = -15.0f;
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
					player.gravity = -15.0f;
				} 
                else
				{
					longSword.isAtk = false;
					longSword.durationTime = 30;
				}
			}

			//攻撃の座標
			 //短剣
			shortSword.pos.x = player.pos.x;
			shortSword.pos.y = player.pos.y + player.radius + shortSword.radius;
			//大剣
			longSword.pos.x = player.pos.x;
			longSword.pos.y = player.pos.y + player.radius + longSword.radius;

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
				if (!boss.isAttacking)
				{
					if (boss.attackCoolTimer > 0)
					{
						boss.attackCoolTimer--;
					}
					if (boss.attackCoolTimer <= 0)
					{
						boss.attackCoolTimer = 0;

						attackTypeFirst = rand() % 2;
						boss.isAttacking = true;
					}
				}
			}

			if (boss.isAttacking)
			{
				switch (attackTypeFirst)
				{
				case SMALLFIRE:
					if (fireShootCount <= 7)
					{
						if (boss.fireCoolTimer <= 0)
						{
							for (int i = 0; i < smallFireMax; i++)
							{
								if (!smallFire[i].isShot)
								{
									smallFire[i].isShot = true;
									smallFire[i].pos.x = boss.pos.x + 16.0f;
									smallFire[i].pos.y = boss.pos.y - 64.0f;
									fireShootCount++;

									break;
								}
							}

							boss.fireCoolTimer = 40;
						}
					}

					if (boss.fireCoolTimer > 0)
					{
						boss.fireCoolTimer--;
					}

					for (int i = 0; i < smallFireMax; i++)
					{
						if (smallFire[i].isShot)
						{
							smallFire[i].pos.x -= smallFire[i].speed;

							//重力
							if (smallFire[i].pos.y - smallFire[i].width / 2.0f > 0.0f)
							{
								smallFire[i].pos.y += smallFire[i].gravity -= 0.8f;
							}
							else
							{
								smallFire[i].gravity = 0.0f;
							}

							//地面に着地した時
							if (smallFire[i].pos.y - smallFire[i].width / 2.0f <= 0.0f)
							{
								smallFire[i].pos.y = smallFire[i].width / 2.0f;
							}

							if (smallFire[i].pos.x <= 0.0f - smallFire[i].width)
							{
								smallFire[i].isShot = false;
								fireDisappearCount++;

								break;
							}
						}
					}

					if (fireDisappearCount == 8)
					{
						boss.isAttacking = false;
						boss.attackCoolTimer = 90;
						fireDisappearCount = 0;
						fireShootCount = 0;

						for (int i = 0; i < smallFireMax; i++)
						{
							smallFire[i].isShot = false;
						}

						break;
					}

					break;

				case BIGFIRE:
					if (!smallFire[8].isShot)
					{
						smallFire[8].pos.x = boss.pos.x + 16.0f;
						smallFire[8].pos.y = boss.pos.y - 64.0f;
						f2pDistance = sqrtf(powf(player.pos.x - smallFire[8].pos.x, 2) + powf(player.pos.y - smallFire[8].pos.y, 2));
						
						if (f2pDistance != 0.0f)
						{
							smallFire[8].direction.x = (player.pos.x - smallFire[8].pos.x) / f2pDistance;
							smallFire[8].direction.y = (player.pos.y - smallFire[8].pos.y) / f2pDistance;
						}

						smallFire[8].isShot = true;
					}

					if (smallFire[8].isShot)
					{
						smallFire[8].pos.x += smallFire[8].direction.x * smallFire[8].speed;
						smallFire[8].pos.y += smallFire[8].direction.y * smallFire[8].speed;

						if (smallFire[8].pos.y <= 0.0f + smallFire[8].height / 2.0f ||
							smallFire[8].pos.x <= 0.0f - smallFire[8].width)
						{
							smallFire[8].isShot = false;
							fireDisappearCount = 1;
						}
					}

					if (fireDisappearCount == 1)
					{
						boss.isAttacking = false;
						boss.attackCoolTimer = 90;
						fireDisappearCount = 0;

						break;
					}

					break;
				}
			}

			frameCount++;

			if (frameCount >= 120)
			{
				frameCount = 0;
			}

			bossCountChange = bossAnimCount;
			bossAnimCount = frameCount / 15;

			if (bossAnimCount > bossCountChange)
			{
				ghBoss1Move += 296;
			}

			if (ghBoss1Move > 2072)
			{
				ghBoss1Move = 0;
			}

			//===========================================================
			//当たり判定
			//===========================================================

			//短剣とボス
			if (shortSword.isAtk)
			{
				if (!player.isDirections) { //右を向いている時、右に攻撃をする
					IsHit(shortSword.pos, shortSword.width, shortSword.height, boss.pos, boss.width, boss.height, shortSword.isBossHit);
				}
				if (player.isDirections) { //左を向いている時、左に攻撃をする
					IsHit(shortSword.pos, -shortSword.width, shortSword.height, boss.pos, boss.width, boss.height, shortSword.isBossHit);
				}
			}

			//大剣とボス
			if (longSword.isAtk)
			{
				if (!player.isDirections) { //右を向いている時、右に攻撃をする
					IsHit(longSword.pos, longSword.width, longSword.height, boss.pos, boss.width, boss.height, longSword.isBossHit);
				}
				if (player.isDirections) { //左を向いている時、左に攻撃をする
					IsHit(longSword.pos, -longSword.width, longSword.height, boss.pos, boss.width, boss.height, longSword.isBossHit);
				}
			}

			//短剣の攻撃がボスに当たっている時
			if (shortSword.isBossHit) {
				shortSword.isAtk = false;
				shortSword.durationTime = 30;
				boss.hpCount -= shortSword.damage; //ボスのHPを攻撃力分減らす
				shortSword.isBossHit = false;
			}

			//大剣の攻撃がボスに当たっている時
			if (longSword.isBossHit) {
				longSword.isAtk = false;
				longSword.durationTime = 30;
				boss.hpCount -= longSword.damage; //ボスのHPを攻撃力分減らす
				longSword.isBossHit = false;
			}
		}

		///
		/// ↑更新処理ここまで
		///
    
		///
		/// ↓描画処理ここから
		///

		if (scene == GAMEPLAY)
		{
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

			if (shortSword.isAtk) //短剣の判定(持続時)
			{
				if (!player.isDirections) //右を向いている時
				{
					Novice::DrawBox
					(
						static_cast<int>(shortSword.pos.x),
						static_cast<int>(ToScreen(shortSword.pos.y)),
						static_cast<int>(shortSword.width),
						static_cast<int>(shortSword.height),
						0.0f, 0xFF000055, kFillModeSolid
					);
				}

				if (player.isDirections) //左を向いている時
				{
					Novice::DrawBox
					(
						static_cast<int>(shortSword.pos.x),
						static_cast<int>(ToScreen(shortSword.pos.y)),
						static_cast<int>(-shortSword.width),
						static_cast<int>(shortSword.height),
						0.0f, 0xFF000055, kFillModeSolid
					);
				}

			}

			if (longSword.isAtk) //大剣の判定(持続時)
			{
				if (!player.isDirections) //右を向いている時
				{
					Novice::DrawBox
					(
						static_cast<int>(longSword.pos.x),
						static_cast<int>(ToScreen(longSword.pos.y)),
						static_cast<int>(longSword.width),
						static_cast<int>(longSword.height),
						0.0f, 0x0000FF55, kFillModeSolid
					);
				}

				if (player.isDirections) //左を向いている時
				{
					Novice::DrawBox
					(
						static_cast<int>(longSword.pos.x),
						static_cast<int>(ToScreen(longSword.pos.y)),
						static_cast<int>(-longSword.width),
						static_cast<int>(longSword.height),
						0.0f, 0x0000FF55, kFillModeSolid
					);
				}
			}

			// 小炎攻撃(連射)
			for (int i = 0; i < smallFireMax; i++)
			{
				if (smallFire[i].isShot)
				{
					Novice::DrawBox(
						static_cast<int>(smallFire[i].pos.x - smallFire[i].width / 2),
						static_cast<int>(ToScreen(smallFire[i].pos.y + smallFire[i].height / 2.0f)),
						static_cast<int>(smallFire[i].width),
						static_cast<int>(smallFire[i].height),
						0.0f, RED, kFillModeSolid);
				}
			}

			// 小炎攻撃(高速)
			if (smallFire[8].isShot)
			{
				Novice::DrawBox(
					static_cast<int>(smallFire[8].pos.x - smallFire[8].width / 2),
					static_cast<int>(ToScreen(smallFire[8].pos.y + smallFire[8].height / 2.0f)),
					static_cast<int>(smallFire[8].width),
					static_cast<int>(smallFire[8].height),
					0.0f, RED, kFillModeSolid);
			}

			// ボス
			Novice::DrawSpriteRect(
				static_cast<int>(boss.pos.x - 48.0f),
				static_cast<int>(ToScreen(boss.pos.y)),
				ghBoss1Move,
				0,
				static_cast<int>(ghBoss1Width),
				static_cast<int>(boss.height),
				ghBoss1,
				boss.height / ghBoss1TotalWidth, 1,
				0, 0xFFFFFFFF);

			Novice::ScreenPrintf(100, 100, "isAttacking: %d", boss.isAttacking);
			Novice::ScreenPrintf(100, 120, "attack coolTimer: %d", boss.attackCoolTimer);
			Novice::ScreenPrintf(100, 140, "attack type: %d", attackTypeFirst);
			Novice::ScreenPrintf(100, 160, "boss hp: %d", boss.hpCount);

			Novice::DrawBox(static_cast<int>(boss.pos.x),
				static_cast<int>(ToScreen(boss.pos.y)),
				static_cast<int>(boss.width),
				static_cast<int>(boss.height),
				0.0f, 0xFFFFFFFF, kFillModeWireFrame);
		}
    
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
