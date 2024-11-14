#include <Novice.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
#include <math.h>

const char kWindowTitle[] = "LC1C_TD(~11/28)_タイトル";
const int kWindowWidth = 1280; // 画面の横幅
const int kWindowHeight = 720; // 画面の縦幅

#pragma region 構造体
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
	float speed;
	float jump;
	float gravity;
	int isJump;
	int isAlive;
	int isDirections;
	int hpCount;
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
	int isCharging;
	int chargeTimer;
	int isHovering;
	int isInScreen;
	int direction;
};

struct Sword
{
	Vector2 pos;
	float width;
	float height;
	int coolTime;
	int durationTime;
	int isAtk;
	int damage;
	int isReaction;
	int reactionTime;
	int isBossHit;
	int isSmallFireHit;
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
	int isReflection;
	int reflectionDamage;
	int isPlayerHit;
	int isBossHit;
	int duration;
	float playerMove;
};

//パーティクル
struct Particle
{
	Vector2 pos;
	float width;
	float height;
	float rotation;
	int color;
	int isDisplay;
};

#pragma endregion

enum DIRECTION
{
	LEFT,
	RIGHT
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
	MOVE,
	SLOWFIRE,
	FASTFIRE,
	MULTIPLEFIRE,
	GIANTFIRE
};

#pragma region 自作関数
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

void MultipleFire(const int kMax, Attack smallFire[], Boss* boss, int& shootCount, float speed)
{
	if (boss->fireCoolTimer > 0)
	{
		boss->fireCoolTimer--;
	}
	if (boss->fireCoolTimer <= 0)
	{
		for (int i = 0; i < kMax; ++i)
		{
			if (i >= shootCount)
			{
				if (!smallFire[i].isShot)//falseのとき
				{
					smallFire[i].speed = speed;
					smallFire[i].isShot = true;
					smallFire[i].pos.y = boss->pos.y;
					if (boss->direction == LEFT)
					{
						smallFire[i].pos.x = boss->pos.x;
						smallFire[i].direction.x = cosf(((i % 3) + 4) / 6.0f * static_cast<float>(M_PI));
						smallFire[i].direction.y = sinf(((i % 3) + 4) / 6.0f * static_cast<float>(M_PI));
					}
					else if (boss->direction == RIGHT)
					{
						smallFire[i].pos.x = boss->pos.x + boss->width;
						smallFire[i].direction.x = cosf((i % 3) / 6 * static_cast<float>(M_PI));
						smallFire[i].direction.y = sinf((i % 3) / 6 * static_cast<float>(M_PI));
					}
					if (i % 3 == 2)
					{
						boss->fireCoolTimer = 30;
						shootCount += 3;
						break;
					}
				}
			}
		}
	}
	for (int i = 0; i < kMax; ++i)
	{
		if (smallFire[i].isShot)
		{
			smallFire[i].pos.x += smallFire[i].direction.x * smallFire[i].speed;
			smallFire[i].pos.y += smallFire[i].direction.y * smallFire[i].speed;

			//重力
			if (smallFire[i].pos.y - smallFire[i].height > 0.0f)
			{
				smallFire[i].pos.y += smallFire[i].direction.y * smallFire[i].speed;
				smallFire[i].pos.y += smallFire[i].gravity -= 0.16f;
			}
			else
			{
				smallFire[i].gravity = 0.0f;
			}

			if (smallFire[i].pos.x + smallFire[i].width <= 0.0f ||
				smallFire[i].pos.x >= 1280.0f ||
				smallFire[i].pos.y - smallFire[i].height <= 0.0f)
			{
				smallFire[i].isShot = false;
				if (i == kMax - 1 || i == kMax - 2 || i == kMax - 3)
				{
					if (!smallFire[kMax - 1].isShot &&
						!smallFire[kMax - 2].isShot &&
						!smallFire[kMax - 3].isShot)//すべての弾を打ち終わり、最後の玉がfalseになったとき
					{
						boss->isAttacking = false;
						boss->attackCoolTimer = 120;
						smallFire[i].gravity = 0.0f;
						shootCount = 0;
						//smallFireを反射した場合の軌道を修正
						if (smallFire[i].speed <= 0)
						{
							smallFire[i].speed *= -0.5f;
						}
					}
				}
			}
		}
	}
}

void FastFire(const int kMax, Attack smallFire[], Boss* boss, Player player, int& shootCount, int& disappearCount)
{
	if (shootCount < 4)
	{
		if (boss->fireCoolTimer > 0)
		{
			boss->fireCoolTimer--;
		}
		if (boss->fireCoolTimer <= 0)
		{
			for (int i = 0; i < kMax; ++i)
			{
				if (!smallFire[i].isShot)
				{
					smallFire[i].isShot = true;
					smallFire[i].pos.y = boss->pos.y + 120.0f;
					if (boss->direction == LEFT)
					{
						smallFire[i].pos.x = boss->pos.x;
					}
					else if (boss->direction == RIGHT)
					{
						smallFire[i].pos.x = boss->pos.x + boss->width;
					}
					float fireToPlayerV = sqrtf(powf(player.pos.x - smallFire[i].pos.x, 2) + powf(player.pos.y - smallFire[i].pos.y, 2));
					if (fireToPlayerV != 0.0f)
					{
						smallFire[i].direction.x = (player.pos.x - smallFire[i].pos.x) / fireToPlayerV;
						smallFire[i].direction.y = (player.pos.y - smallFire[i].pos.y) / fireToPlayerV;
					}
					shootCount++;
					boss->fireCoolTimer = 30;
					break;
				}
			}
		}
	}
	for (int i = 0; i < kMax; ++i)
	{
		if (smallFire[i].isShot)
		{
			smallFire[i].pos.x += smallFire[i].direction.x * smallFire[i].speed;
			smallFire[i].pos.y += smallFire[i].direction.y * smallFire[i].speed;
			//画面外にでたとき
			if (smallFire[i].pos.x + smallFire[i].width < 0.0f ||
				smallFire[i].pos.x>1280.0f ||
				smallFire[i].pos.y - smallFire[i].height < 0.0f)
			{
				smallFire[i].isShot = false;
				disappearCount++;
			}
		}
	}

	if (disappearCount == 4)
	{
		boss->isAttacking = false;
		boss->attackCoolTimer = 120;
		disappearCount = 0;
	}

}

#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	//==============================================
	//変数初期化
	//==============================================

	//シーン
	int scene = GAMEPLAY;

	int phase = 0;

	//コントローラー
	int padX = 0;	//左スティックの左右値
	int padY = 0;	//左スティックの上下値

#pragma region プレイヤー
	//プレイヤー
	Player player;
	player.pos.x = 100.0f; //ｘ座標
	player.pos.y = 100.0f; //ｙ座標
	player.posW.x = 100.0f; //ｘ座標(ワールド)
	player.posW.y = 100.0f; //ｙ座標(ワールド)
	player.width = 32.0f; //縦幅
	player.height = 64.0f; //横幅
	player.speed = 10.0f; //移動速度
	player.jump = 15.0f; //ジャンプ速度
	player.gravity = 0.0f; //重力
	player.isJump = false; //ジャンプ状態か否か
	player.isAlive; //生存
	player.isDirections = false; //プレイヤーの向いている方向(false = 右,true = 左)
	player.hpCount = 10; //hp

	//短剣
	Sword shortSword;
	shortSword.pos.x = 100.0f; //ｘ座標
	shortSword.pos.y = 100.0f; //ｙ座標
	shortSword.width = 64.0f; //縦幅
	shortSword.height = 64.0f; //横幅
	shortSword.coolTime = 0; //攻撃クールタイム
	shortSword.durationTime = 30; //攻撃の持続時間
	shortSword.isAtk = false; //攻撃しているか
	shortSword.isBossHit = false; //攻撃が当たっているか(ボスに)
	shortSword.damage = 3; //攻撃力
	shortSword.isReaction = false; //硬直が起きているか 
	shortSword.reactionTime = 30; //硬直で動けない時間

	//大剣
	Sword longSword;
	longSword.pos.x = 100.0f; //ｘ座標
	longSword.pos.y = 100.0f; //ｙ座標
	longSword.width = 96.0f; //縦幅
	longSword.height = 96.0f; //横幅
	longSword.coolTime = 0; //攻撃クールタイム
	longSword.durationTime = 30; //攻撃の持続時間
	longSword.isAtk = false; //攻撃しているか
	longSword.isBossHit = false; //攻撃が当たっているか(ボスに)
	longSword.damage = 5; //攻撃力
	longSword.isReaction = false; //硬直が起きているか 
	longSword.reactionTime = 30; //硬直で動けない時間 
	longSword.isBossHit = false; //攻撃が当たっているか(ボスに) 
	longSword.isSmallFireHit = false; //攻撃が当たっているか(smallFireに)


	int ghPlayerLeft = Novice::LoadTexture("./Resources/images/player_left.png"); // プレイヤー左向きの待機画像 
	int ghPlayerRight = Novice::LoadTexture("./Resources/images/player_right.png"); // プレイヤー右向きの待機画像

	int playerAnimeCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float playerMaxImageWidth = 320.0f; // プレイヤーの画像の最大横幅 
	float playerFrameImageWidth = 64.0f; // プレイヤーの1フレームの画像横幅
	float playerImageHeight = 64.0f; //プレイヤーの画像の縦幅



#pragma endregion

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
	srand(currentTime);

#pragma region ボス
	Boss boss;
	boss.pos = { 840.0f, 320.0f }; // 左上座標
	boss.speed = 10.0f; // 移動速度
	boss.attackCoolTimer = 0; // 攻撃のクールタイム
	boss.isAlive = true; // 生きているか
	boss.isChange = false; // 形態変化用のフラグ
	boss.hpCount = 200; // 体力
	boss.width = 288.0f; // 横幅(当たり判定用)
	boss.height = 320.0f; // 縦幅(当たり判定用)

	//ボス攻撃
	boss.attackCoolTimer = 60;
	boss.fireCoolTimer = 0; // 小炎攻撃用のタイマー
	boss.isAttacking = false;
	boss.isCharging = false;
	boss.chargeTimer = 120;
	boss.isHovering = false;
	boss.direction = LEFT;

	// ボス移動
	boss.isInScreen = true;

	const int slowFireMax = 8; // 低速小炎の最大数
	const int fastFireMax = 12; // 高速小炎の最大数
	const int multipleFireMax = 36; // 拡散小炎の最大数
	const int kMaxSmallFire = 36;
	const int kMaxMultiple = 24;

	int fireShootCount = 0; // 炎を撃ったカウント
	int fireDisappearCount = 0; // 炎が消えたカウント

	Attack smallFire[36];

	for (int i = 0; i < kMaxSmallFire; i++)
	{
		smallFire[i].pos = { 0.0f }; // 座標
		smallFire[i].width = 32.0f; // 横幅
		smallFire[i].height = 32.0f; // 縦幅
		smallFire[i].speed = 5.0f; // 速度
		smallFire[i].isShot = false; // 撃たれたか
		smallFire[i].gravity = 0.0f; //重力
		smallFire[i].direction = { 0.0f };
		smallFire[i].isPlayerHit = false; //プレイヤーに当たったか
		smallFire[i].isBossHit = false; // 反射された攻撃がボスに当たったか 
		smallFire[i].isReflection = false; // 反射されたか 
		smallFire[i].reflectionDamage = 5; // 反射された攻撃がボスに当たった時のダメージ
	}

	float slowFireSpeed = 5.0f;
	//float fastFireSpeed = 20.0f;
	float multipleFireSpeed = 12.0f;

	Attack giantFire;

	giantFire.pos = { 0.0f }; // 座標
	giantFire.width = 128.0f; // 横幅
	giantFire.height = 128.0f; // 縦幅
	giantFire.speed = 15.0f; // 速度
	giantFire.isShot = false; // 撃たれたか
	giantFire.direction = { 0.0f }; // 方向
	giantFire.isPlayerHit = false; // プレイヤーに当たったか

	Attack explosion;

	explosion.pos = { 0.0f }; // 座標
	explosion.width = 128.0f; // 横幅
	explosion.height = 128.0f; // 縦幅
	explosion.duration = 0; // 持続時間
	explosion.isPlayerHit = false; // プレイヤーに当たったか
	explosion.isShot = false; // 撃たれたか
	explosion.playerMove = 256.0f; // プレイヤーに当たったか

	float f2pDistance = 0.0f; // 炎とプレイヤーの距離

	int attackTypeFirst = 0; // 第一形態の技の種類

	int ghBoss1Left = Novice::LoadTexture("./Resources/images/Doragon_1.png"); // 第一形態のボスの画像
	int ghBoss1Right = Novice::LoadTexture("./Resources/images/Doragon_1_right.png"); // 第一形態のボスの画像

	int bossAnimeCount = 0; // ボスのアニメーションｎフレームカウント
	float boss1MaxImageWidth = 5760.0f; // ボスの画像の最大横幅
	float boss1FrameImageWidth = 640.0f; // ボスの1フレームの画像横幅
	float boss1ImageHeight = 352.0f; //ボスの画像の縦幅


	//パーティクル
	int playerLocusMax = 50; //最大表示数
	Particle playerLocus[50];
	for (int i = 0; i < playerLocusMax; i++) {
		playerLocus[i].pos.x = 0.0f; //ｘ座標
		playerLocus[i].pos.y = 0.0f; //ｙ座標
		playerLocus[i].width = 16.0f; //横幅
		playerLocus[i].height = 16.0f; //縦幅
		playerLocus[i].rotation = 0.0f; //回転角
		playerLocus[i].color = WHITE; //色
		playerLocus[i].isDisplay = false; //表示されているか
	}
	int playerLocusCoolTime = 240;//表示のクールタイム

#pragma endregion

	int frameCount = 0; // フレーム
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



			//スティックの値を取得する
			Novice::GetAnalogInputLeft(0, &padX, &padY);

			if (phase == 0)
			{
#pragma region プレイヤー
				//===========================================================
				//プレイヤー
				//===========================================================

				//攻撃時は動けない
				if (!shortSword.isAtk && !longSword.isAtk)
				{
					// 吹き飛ばされている間は動けない
					if (!explosion.isPlayerHit)
					{
						//左右移動(AD or 左スティック)
						if (keys[DIK_A] || padX <= -1)
						{
							player.pos.x -= player.speed;

							//パーティクル軌跡
							for (int i = 0; i < playerLocusMax; i++) {
								if (playerLocusCoolTime >= 0) {
									playerLocusCoolTime--;
								}
								else {
									playerLocus[i].isDisplay = true;
									playerLocusCoolTime = 240;
								}
							}

							if (!shortSword.isAtk && !longSword.isAtk)
							{
								player.isDirections = true;
							}
						}


						if (keys[DIK_D] || padX >= 1)
						{
							player.pos.x += player.speed;

							//パーティクル軌跡
							for (int i = 0; i < playerLocusMax; i++) {
								if (playerLocusCoolTime >= 0) {
									playerLocusCoolTime--;
								}
								else {
									playerLocus[i].isDisplay = true;
									playerLocusCoolTime = 240;
								}
							}

							if (!shortSword.isAtk && !longSword.isAtk)
							{
								player.isDirections = false;
							}
						}


						//ジャンプ(SPACE or A)
						if (keys[DIK_SPACE] && !preKeys[DIK_SPACE] || Novice::IsPressButton(0, PadButton::kPadButton10))
						{
							player.isJump = true;
						}

						if (player.isJump)
						{
							player.pos.y += player.jump;

							//パーティクル軌跡
							for (int i = 0; i < playerLocusMax; i++) {
								if (playerLocusCoolTime >= 0) {
									playerLocusCoolTime--;
								}
								else {
									playerLocus[i].isDisplay = true;
									playerLocusCoolTime = 240;
								}
							}
						}
					}
				}

				//==============================================================
				//攻撃
				//==============================================================

				//攻撃時は動けない
				if (!shortSword.isAtk && !longSword.isAtk)
				{
					// 吹き飛ばされている間は攻撃できない
					if (!explosion.isPlayerHit)
					{
						//短剣(J or X)
						if (keys[DIK_J] && !preKeys[DIK_J] || Novice::IsPressButton(0, PadButton::kPadButton12))
						{
							if (!longSword.isAtk) //大剣攻撃時は使えない
							{
								if (shortSword.coolTime <= 0) //クールタイムが０以下の時のみ
								{
									shortSword.isAtk = true;
									shortSword.isReaction = true;
									shortSword.coolTime = 20;
								}
							}
						}

						//大剣(K or Y)
						if (keys[DIK_K] && !preKeys[DIK_K] || Novice::IsPressButton(0, PadButton::kPadButton13))
						{
							if (!shortSword.isAtk) //短剣攻撃時は使えない
							{
								if (longSword.coolTime <= 0) //クールタイムが０以下の時のみ
								{
									longSword.isAtk = true;
									longSword.isReaction = true;
									longSword.coolTime = 40;
								}
							}
						}
					}
				}

				//--------------クールタイム----------------//

				if (shortSword.coolTime >= 0) //短剣
				{
					shortSword.coolTime--;
				}

				if (longSword.coolTime >= 0) //大剣
				{
					longSword.coolTime--;
				}

				//-------------攻撃の持続時間----------------//

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
					}
					else
					{
						longSword.isAtk = false;
						longSword.durationTime = 30;
					}
				}


				//------------攻撃時の硬直によって動けない時間------------//

				if (shortSword.isReaction) //短剣
				{
					if (shortSword.reactionTime >= 0)
					{
						shortSword.reactionTime--;
						player.gravity = 0.5f;
						player.speed = 0.0f;
						player.jump = 0.0f;
					}
					else
					{
						shortSword.reactionTime = 30;
						player.speed = 10.0f;
						shortSword.isReaction = false;
					}
				}

				if (longSword.isReaction) //大剣
				{
					if (longSword.reactionTime >= 0)
					{
						longSword.reactionTime--;
						player.gravity = 0.5f;
						player.speed = 0.0f;
						player.jump = 0.0f;
					}
					else
					{
						longSword.reactionTime = 30;
						player.speed = 10.0f;
						longSword.isReaction = false;
					}
				}

				//-----------攻撃の座標--------------//

				//短剣
				shortSword.pos.y = player.pos.y - player.height + shortSword.height;
				if (!player.isDirections)//右
				{
					shortSword.pos.x = player.pos.x + player.width / 2.0f;
				}
				else//左
				{
					shortSword.pos.x = player.pos.x + player.width / 2.0f - shortSword.width;
				}

				//大剣
				longSword.pos.y = player.pos.y - player.height + longSword.height;
				if (!player.isDirections)//右
				{
					longSword.pos.x = player.pos.x + player.width / 2.0f;
				}
				else//左
				{
					longSword.pos.x = player.pos.x + player.width / 2.0f - longSword.width;
				}

				//----------------重力------------------//

				if (player.pos.y - player.height > 0.0f)
				{
					player.pos.y += player.gravity -= 0.7f;
				}
				else
				{
					player.gravity = 0.0f;
				}

				//地面に着地した時
				if (player.pos.y - player.height <= 0.0f)
				{
					player.pos.y = player.height;
					player.jump = 15.0f;
					player.isJump = false;
				}

				//-----------HP--------------//

				if (player.hpCount <= 0) {
					player.isAlive = false;
				}
#pragma endregion

#pragma region ボス
				//===========================================================
				//ボスの移動
				//===========================================================

				if (!boss.isAttacking)
				{
					if (boss.attackCoolTimer > 0)
					{
						boss.attackCoolTimer--;
					}
					else if (boss.attackCoolTimer <= 0)
					{
						//初期化
						for (int i = 0; i < kMaxSmallFire; ++i)
						{
							smallFire[i].isShot = false;
							smallFire[i].speed = 5.0f;
							smallFire[i].gravity = 0.0f;
							fireShootCount = 0;
						}

						boss.attackCoolTimer = 0;

						boss.isAttacking = true;

						if (boss.hpCount <= 100)
						{
							attackTypeFirst = rand() % 5;
						}
						else if (boss.hpCount <= 180)
						{
							attackTypeFirst = rand() % 4;
						}
						else if (boss.hpCount <= 200)
						{
							attackTypeFirst = 2;// rand() % 3;
						}
					}
				}

				if (boss.isAttacking)
				{
					switch (attackTypeFirst)
					{
					case MOVE:
						if (boss.isInScreen)
						{
							if (boss.direction == LEFT)
							{
								if (boss.pos.x < 1280.0f + boss1FrameImageWidth)
								{
									boss.pos.x += boss.speed;
								}

								if (boss.pos.x >= 1280.0f + boss1FrameImageWidth)
								{
									boss.pos.x = -200;
									boss.isInScreen = false;
									boss.direction = RIGHT;
								}
							}
							else if (boss.direction == RIGHT)
							{
								if (boss.pos.x > 0 - boss1FrameImageWidth)
								{
									boss.pos.x -= boss.speed;

								}

								if (boss.pos.x <= 0.0f - boss1FrameImageWidth)
								{
									boss.pos.x = 1300;
									boss.isInScreen = false;
									boss.direction = LEFT;
								}
							}
						}

						if (!boss.isInScreen)
						{
							if (boss.direction == RIGHT)
							{
								if (boss.pos.x < 152.0f)
								{
									boss.pos.x += boss.speed;
								}

								if (boss.pos.x >= 152.0f)
								{
									boss.pos.x = 152.0f;
									boss.isAttacking = false;
									boss.isInScreen = true;
									boss.attackCoolTimer = 20;
								}
							}
							else if (boss.direction == LEFT)
							{
								if (boss.pos.x > 840.0f)
								{
									boss.pos.x -= boss.speed;
								}

								if (boss.pos.x <= 840.0f)
								{
									boss.pos.x = 840.0f;
									boss.isAttacking = false;
									boss.isInScreen = true;
									boss.attackCoolTimer = 20;
								}
							}
						}

						break;
					case SLOWFIRE:
						if (fireShootCount <= 7)
						{
							if (boss.fireCoolTimer <= 0)
							{
								for (int i = 0; i < slowFireMax; i++)
								{
									if (!smallFire[i].isShot)
									{
										smallFire[i].isShot = true;

										if (boss.direction == LEFT)
										{
											smallFire[i].pos.x = boss.pos.x;
										}
										else if (boss.direction == RIGHT)
										{
											smallFire[i].pos.x = boss.pos.x + 256.0f;
										}

										smallFire[i].pos.y = boss.pos.y - 120.0f;
										smallFire[i].speed = slowFireSpeed;
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

						for (int i = 0; i < slowFireMax; i++)
						{
							if (smallFire[i].isShot)
							{
								if (boss.direction == LEFT)
								{
									smallFire[i].pos.x -= smallFire[i].speed;
								}
								else if (boss.direction == RIGHT)
								{
									smallFire[i].pos.x += smallFire[i].speed;
								}

								//重力
								if (smallFire[i].pos.y - smallFire[i].width > 0.0f)
								{
									smallFire[i].pos.y += smallFire[i].gravity -= 0.8f;
								}
								else
								{
									smallFire[i].gravity = 0.0f;
								}

								//地面に着地した時
								if (smallFire[i].pos.y - smallFire[i].width <= 0.0f)
								{
									smallFire[i].pos.y = smallFire[i].width;
								}

								if (smallFire[i].pos.x <= 0.0f - smallFire[i].width || smallFire[i].pos.x >= 1400.0f || smallFire[i].pos.y >= 800.0f)
								{
									smallFire[i].isShot = false;
									fireDisappearCount++;

								}
							}
						}

						if (fireDisappearCount == 8)
						{
							boss.isAttacking = false;
							boss.attackCoolTimer = 60;
							fireDisappearCount = 0;
							fireShootCount = 0;

							break;
						}

						break;

					case FASTFIRE:

						FastFire(kMaxSmallFire, smallFire, &boss, player, fireShootCount, fireDisappearCount);

						break;

					case MULTIPLEFIRE:

						MultipleFire(kMaxMultiple, smallFire, &boss, fireShootCount, multipleFireSpeed);

						break;

					case GIANTFIRE:
						if (!boss.isHovering && fireDisappearCount <= 0)//ボスが滞空していないとき&攻撃が1発も撃たれていないとき

						{
							//ボスが上にいく処理
							if (boss.pos.y < 500.0f)
							{
								boss.pos.y += boss.speed;
							}
							//ボスが上に上がりきったときの処理
							if (boss.pos.y >= 500.0f)
							{
								boss.isHovering = true;//最高地点で飛んでいる
								boss.isCharging = true;//攻撃のためにはいる
								boss.chargeTimer = 120;

								if (boss.direction == LEFT)
								{
									giantFire.pos.x = boss.pos.x;
								}
								if (boss.direction == RIGHT)
								{
									giantFire.pos.x = boss.pos.x + boss.width;
								}

								giantFire.pos.y = boss.pos.y;
							}
						}

						if (boss.isHovering)//滞空しているとき
						{
							if (boss.isCharging)//攻撃をためているとき
							{
								if (boss.chargeTimer > 0)
								{
									boss.chargeTimer--;
								}
								else//チャージ完了
								{
									//この時点でのプレイヤーの位置に攻撃を飛ばすためのベクトルの計算
									f2pDistance = sqrtf(powf(player.pos.x - giantFire.pos.x, 2) + powf(player.pos.y - giantFire.pos.y, 2));
									//正規化
									if (f2pDistance != 0.0f)
									{
										giantFire.direction.x = (player.pos.x - giantFire.pos.x) / f2pDistance;
										giantFire.direction.y = (player.pos.y - giantFire.pos.y + giantFire.height) / f2pDistance;

										boss.isCharging = false;
										giantFire.isShot = true;
									}
								}
							}

							if (giantFire.isShot)
							{
								giantFire.pos.x += giantFire.speed * giantFire.direction.x;
								giantFire.pos.y += giantFire.speed * giantFire.direction.y;

								if (giantFire.pos.x <= 0.0f - giantFire.width ||
									giantFire.pos.y <= 0.0f + giantFire.height || giantFire.pos.x >= 1400.0f || giantFire.pos.y >= 800.0f)
								{
									giantFire.isShot = false;

									explosion.pos.x = giantFire.pos.x - 192.0f;
									explosion.pos.y = giantFire.pos.y;
									explosion.isShot = true;
									explosion.duration = 30;
								}
							}

							if (explosion.isShot)
							{
								explosion.duration--;

								if (explosion.width < 512.0f)
								{
									explosion.width += 128;
								}

								if (explosion.height < 256.0f)
								{
									explosion.height += 64;
									explosion.pos.y += 64;
								}

								if (explosion.duration <= 0)
								{
									explosion.isShot = false;
									explosion.width = 128.0f;
									explosion.height = 128.0f;
									fireDisappearCount = 1;
								}
							}

							if (!giantFire.isShot && fireDisappearCount == 1)
							{
								if (boss.pos.y > 160.0f)
								{
									boss.pos.y -= boss.speed;
								}
								else
								{
									boss.isHovering = false;
								}
							}

							if (!boss.isHovering && fireDisappearCount == 1)
							{
								boss.isAttacking = false;
								boss.chargeTimer = 0;
								boss.attackCoolTimer = 180;
								fireDisappearCount = 0;

								break;
							}

							break;
						}
					}
				}
#pragma endregion
			}
			//ボスのアニメーション

			if (frameCount >= 60)
			{
				frameCount = 0;
				bossAnimeCount = 0;
				playerAnimeCount = 0;
			}
			frameCount++;

			if (frameCount % (60 / 8) == 0)
			{
				bossAnimeCount++;
			}

			if (frameCount % (60 / 4) == 0)
			{
				playerAnimeCount++;
			}



			//===========================================================
			//当たり判定
			//===========================================================

			//--------------ボスと攻撃の当たり判定---------------//

			//短剣とボス
			if (shortSword.isAtk)
			{
				IsHit(shortSword.pos, shortSword.width, shortSword.height, boss.pos, boss.width, boss.height, shortSword.isBossHit);
			}

			//大剣とボス
			if (longSword.isAtk)
			{
				IsHit(longSword.pos, longSword.width, longSword.height, boss.pos, boss.width, boss.height, longSword.isBossHit);
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

			//----------------攻撃を反射するときの当たり判定----------------//

			for (int i = 0; i < kMaxSmallFire; i++)
			{
				if (longSword.isAtk)
				{
					IsHit(longSword.pos, longSword.width, longSword.height, smallFire[i].pos, smallFire[i].width, smallFire[i].height, longSword.isSmallFireHit);
				}

				if (longSword.isSmallFireHit)
				{
					longSword.isAtk = false;
					longSword.durationTime = 30;
					longSword.isSmallFireHit = false;

					smallFire[i].speed *= -2.0f; //反射
					smallFire[i].isReflection = true;
				}
			}

			//小炎軌道修正
			for (int i = 0; i < slowFireMax; i++) {
				if (!smallFire[i].isShot) {
					smallFire[i].speed = slowFireSpeed;
				}
			}

			for (int i = 8; i < fastFireMax; i++) {
				if (!smallFire[i].isShot) {
					//smallFire[i].speed = 20.0f;
				}
			}

			for (int i = 12; i < multipleFireMax; i++) {
				if (!smallFire[i].isShot) {
					//smallFire[i].speed = 12.0f;
				}
			}

			//------------------反射した攻撃とボスの当たり判定-----------------//

			for (int i = 0; i < kMaxSmallFire; i++)
			{
				if (smallFire[i].isReflection)
				{
					IsHit(smallFire[i].pos, smallFire[i].width, smallFire[i].height, boss.pos, boss.width, boss.height, smallFire[i].isBossHit);
				}

				if (smallFire[i].isBossHit)
				{
					boss.hpCount -= smallFire[i].reflectionDamage; //反射した小炎がボスに当たった時にHPを減らす
					smallFire[i].isBossHit = false;
					smallFire[i].isReflection = false;
				}

			}

			//------------------小炎とプレイヤーの当たり判定----------------//

			// 小炎全体の当たり判定
			for (int i = 0; i < kMaxSmallFire; i++)
			{
				if (smallFire[i].isShot)
				{
					IsHit(player.pos, player.width, player.height, smallFire[i].pos, smallFire[i].width, smallFire[i].height, smallFire[i].isPlayerHit);
				}
			}

			// 小炎(連続)の当たり判定
			for (int i = 0; i < slowFireMax; i++)
			{
				if (smallFire[i].isPlayerHit)
				{
					player.hpCount -= 1;
					smallFire[i].isShot = false;
					smallFire[i].isPlayerHit = false;
					fireDisappearCount++;
				}
			}

			// 小炎(高速)の当たり判定
			for (int i = 8; i < fastFireMax; i++)
			{
				if (smallFire[i].isPlayerHit)
				{
					player.hpCount -= 1;
					smallFire[i].isShot = false;
					smallFire[i].isPlayerHit = false;
					fireDisappearCount++;
				}
			}

			// 小炎(拡散)の当たり判定
			for (int i = 12; i < multipleFireMax; i++)
			{
				if (smallFire[i].isPlayerHit)
				{
					player.hpCount -= 1;
					smallFire[i].isShot = false;
					smallFire[i].isPlayerHit = false;
					fireDisappearCount++;
				}
			}

			if (giantFire.isShot)
			{
				IsHit(player.pos, player.width, player.height, giantFire.pos, giantFire.width, giantFire.height, giantFire.isPlayerHit);
			}

			if (giantFire.isPlayerHit)
			{
				player.hpCount -= 5;
				giantFire.isShot = false;
				giantFire.isPlayerHit = false;
				fireDisappearCount++;
			}

			if (explosion.isShot)
			{
				IsHit(player.pos, player.width, player.height, explosion.pos, explosion.width, explosion.height, explosion.isPlayerHit);
			}

			if (explosion.isPlayerHit)
			{
				player.hpCount -= 5;

				if (player.pos.x <= explosion.pos.x + explosion.width / 2.0f)
				{
					player.pos.x -= explosion.playerMove;
				}
				if (player.pos.x > explosion.pos.x + explosion.width / 2.0f)
				{
					player.pos.x += explosion.playerMove;
				}

				explosion.isPlayerHit = false;
			}
		}


		//==============================================================
		//パーティクル
		//==============================================================

		//--------------------プレイヤーの軌跡---------------------//
		for (int i = 0; i < playerLocusMax; i++) {

			if (playerLocus[i].isDisplay) {
				//段々小さくなる
				if (playerLocus[i].width >= 0.0f && playerLocus[i].height >= 0.0f) {
					playerLocus[i].width -= 0.5f;
					playerLocus[i].height -= 0.5f;
				}
				else {
					playerLocus[i].width = 16.0f;
					playerLocus[i].height = 16.0f;
					playerLocus[i].isDisplay = false;
				}

				//回転させる
				playerLocus[i].rotation += 0.02f;

				//移動させる
				if (!player.isDirections) {
					//右
					playerLocus[i].pos.x -= 0.5f;
					playerLocus[i].pos.y += 0.01f;
				}
				else
				{
					//左
					playerLocus[i].pos.x += 0.5f;
					playerLocus[i].pos.y += 0.01f;
				}
			}

			if (!playerLocus[i].isDisplay) {
				//ランダムな位置に表示させる
				playerLocus[i].pos.x = rand() % 32 - 16 + player.pos.x + player.width / 2.0f;
				playerLocus[i].pos.y = rand() % 32 - 16 + ToScreen(player.pos.y) + player.height / 2.0f;
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

			//---------------------パーティクル-----------------------//

			//プレイヤーの軌跡
			for (int i = 0; i < playerLocusMax; i++) {
				if (playerLocus[i].isDisplay) {
					Novice::DrawBox(
						static_cast<int>(playerLocus[i].pos.x),
						static_cast<int>(playerLocus[i].pos.y),
						static_cast<int>(playerLocus[i].width),
						static_cast<int>(playerLocus[i].height),
						playerLocus[i].rotation, playerLocus[i].color, kFillModeWireFrame);
				}
			}

			//地面 
			Novice::DrawLine(0, 620, 1280, 620, RED);

			if (!player.isDirections)
			{
				//プレイヤー
				Novice::DrawSpriteRect
				(
					static_cast<int>(player.pos.x),
					static_cast<int>(ToScreen(player.pos.y)),
					64 * playerAnimeCount,
					0,
					static_cast<int>(playerFrameImageWidth),
					static_cast<int>(playerImageHeight),
					ghPlayerRight,
					playerFrameImageWidth / playerMaxImageWidth, 1,
					0, 0xFFFFFFFF
				);
			}
			else if (player.isDirections)
			{
				//プレイヤー
				Novice::DrawSpriteRect
				(
					static_cast<int>(player.pos.x),
					static_cast<int>(ToScreen(player.pos.y)),
					256 - 64 * playerAnimeCount,
					0,
					static_cast<int>(playerFrameImageWidth),
					static_cast<int>(playerImageHeight),
					ghPlayerLeft,
					playerFrameImageWidth / playerMaxImageWidth, 1,
					0, 0xFFFFFFFF
				);
			}

			if (shortSword.isAtk) //短剣の判定(持続時)
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

			if (longSword.isAtk) //大剣の判定(持続時)
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

			// ボス
			if (boss.direction == LEFT)
			{
				Novice::DrawSpriteRect
				(
					static_cast<int>(boss.pos.x - (boss1FrameImageWidth / 2.0f - boss.width / 2.0f)),
					static_cast<int>(ToScreen(boss.pos.y + (boss1ImageHeight - boss.height))),
					640 * bossAnimeCount,
					0,
					static_cast<int>(boss1FrameImageWidth),
					static_cast<int>(boss1ImageHeight),
					ghBoss1Left,
					boss1FrameImageWidth / boss1MaxImageWidth, 1,
					0, 0xFFFFFFFF
				);
			}
			else if (boss.direction == RIGHT)
			{
				Novice::DrawSpriteRect
				(
					static_cast<int>(boss.pos.x - (boss1FrameImageWidth / 2.0f - boss.width / 2.0f)),
					static_cast<int>(ToScreen(boss.pos.y + (boss1ImageHeight - boss.height))),
					5120 - 640 * bossAnimeCount,
					0,
					static_cast<int>(boss1FrameImageWidth),
					static_cast<int>(boss1ImageHeight),
					ghBoss1Right,
					boss1FrameImageWidth / boss1MaxImageWidth, 1,
					0, 0xFFFFFFFF
				);
			}

			// 小炎攻撃(連射)
			for (int i = 0; i < slowFireMax; i++)
			{
				if (smallFire[i].isShot)
				{
					Novice::DrawBox(
						static_cast<int>(smallFire[i].pos.x),
						static_cast<int>(ToScreen(smallFire[i].pos.y)),
						static_cast<int>(smallFire[i].width),
						static_cast<int>(smallFire[i].height),
						0.0f, RED, kFillModeSolid);
				}
			}

			for (int i = 8; i < fastFireMax; i++)
			{
				// 小炎攻撃(高速)
				if (smallFire[i].isShot)
				{
					Novice::DrawBox(
						static_cast<int>(smallFire[i].pos.x),
						static_cast<int>(ToScreen(smallFire[i].pos.y)),
						static_cast<int>(smallFire[i].width),
						static_cast<int>(smallFire[i].height),
						0.0f, RED, kFillModeSolid);
				}
			}

			for (int i = 12; i < multipleFireMax; i++)
			{
				// 小炎攻撃(高速)
				if (smallFire[i].isShot)
				{
					Novice::DrawBox(
						static_cast<int>(smallFire[i].pos.x),
						static_cast<int>(ToScreen(smallFire[i].pos.y)),
						static_cast<int>(smallFire[i].width),
						static_cast<int>(smallFire[i].height),
						0.0f, RED, kFillModeSolid);
				}
			}

			// 巨大火球
			if (giantFire.isShot)
			{
				Novice::DrawBox(
					static_cast<int>(giantFire.pos.x),
					static_cast<int>(ToScreen(giantFire.pos.y)),
					static_cast<int>(giantFire.width),
					static_cast<int>(giantFire.height),
					0.0f, RED, kFillModeSolid);
			}

			// 爆発
			if (explosion.isShot)
			{
				Novice::DrawBox(
					static_cast<int>(explosion.pos.x),
					static_cast<int>(ToScreen(explosion.pos.y)),
					static_cast<int>(explosion.width),
					static_cast<int>(explosion.height),
					0.0f, 0XFF000055, kFillModeSolid);
			}

			Novice::ScreenPrintf(100, 100, "isAttacking: %d", boss.isAttacking);
			Novice::ScreenPrintf(100, 120, "attack coolTimer: %d", boss.attackCoolTimer);
			Novice::ScreenPrintf(100, 140, "attack type: %d", attackTypeFirst);
			Novice::ScreenPrintf(100, 160, "boss hp: %d", boss.hpCount);
			Novice::ScreenPrintf(100, 180, "player hp: %d", player.hpCount);
			Novice::ScreenPrintf(100, 200, "isShot: %d", explosion.isShot);
			Novice::ScreenPrintf(100, 220, "disappearcount: %d", fireDisappearCount);
			Novice::ScreenPrintf(100, 240, "smallFire : %d", smallFire[7].isShot);

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
