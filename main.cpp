#include <Novice.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES
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
};

struct Sword
{
	Vector2 pos;
	float width;
	float height;
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
	int isPlayerHit;
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
	FASTFIRE,
	MULTIPLEFIRE,
	GIANTFIRE
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

	//シーン
	int scene = GAMEPLAY;

	//コントローラー
	int padX = 0;	//左スティックの左右値
	int padY = 0;	//左スティックの上下値

	//プレイヤー
	Player player;
	player.pos.x = 100.0f; //ｘ座標
	player.pos.y = 100.0f; //ｙ座標
	player.posW.x = 100.0f; //ｘ座標(ワールド)
	player.posW.y = 100.0f; //ｙ座標(ワールド)
	player.width = 32.0f; //縦幅
	player.height = 32.0f; //横幅
	player.speed = 10.0f; //移動速度
	player.jump = 15.0f; //ジャンプ速度
	player.gravity = 0.0f; //重力
	player.isJump = false; //ジャンプ状態か否か
	player.isAlive; //生存
	player.isDirections = false; //プレイヤーの向いている方向(false = 右,true = 左)
	player.hpCount = 10;


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

	Sword longSword;
	longSword.pos.x = 100.0f; //ｘ座標
	longSword.pos.y = 100.0f; //ｙ座標
	longSword.width = 64.0f; //縦幅
	longSword.height = 64.0f; //横幅
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
	boss.isCharging = false;
	boss.chargeTimer = 120;
	boss.isHovering = false;

	const int slowFireMax = 8; // 低速小炎の最大数
	const int fastFireMax = 12; // 高速小炎の最大数
	const int multipleFireMax = 36; // 拡散小炎の最大数
	const int multiple1Max = 20; // 上段
	const int multiple2Max = 28; // 中段
	const int multiple3Max = 36; // 下段
	const int smallFireMax = 36;

	int fireShootCount = 0; // 炎を撃ったカウント
	int fireDisappearCount = 0; // 炎が消えたカウント

	Attack smallFire[36];

	for (int i = 0; i < slowFireMax; i++)
	{
		smallFire[i].pos = { 0.0f }; // 座標
		smallFire[i].width = 32.0f; // 横幅
		smallFire[i].height = 32.0f; // 縦幅
		smallFire[i].speed = 5.0f; // 速度
		smallFire[i].isShot = false; // 撃たれたか
		smallFire[i].gravity = 0.0f;
	}

	for (int i = 8; i < fastFireMax; i++)
	{
		smallFire[i].pos = { 0.0f }; // 座標
		smallFire[i].width = 32.0f; // 横幅
		smallFire[i].height = 32.0f; // 縦幅
		smallFire[i].speed = 20.0f; // 速度
		smallFire[i].isShot = false; // 撃たれたか
		smallFire[i].direction = { 0.0f };
	}

	for (int i = 12; i < multipleFireMax; i++)
	{
		smallFire[i].pos = { 0.0f }; // 座標
		smallFire[i].width = 32.0f; // 横幅
		smallFire[i].height = 32.0f; // 縦幅
		smallFire[i].speed = 12.0f; // 速度
		smallFire[i].isShot = false; // 撃たれたか
		smallFire[i].direction = { 0.0f };
		smallFire[i].gravity = 0.0f;
	}

	for (int i = 0; i < smallFireMax; i++)
	{
		smallFire[i].isPlayerHit = false;
	}

	Attack giantFire;

	giantFire.pos = { 0.0f };
	giantFire.width = 128.0f;
	giantFire.height = 128.0f;
	giantFire.speed = 10.0f;
	giantFire.isShot = false;
	giantFire.direction = { 0.0f };

	float f2pDistance = 0.0f; // 炎とプレイヤーの距離

	int attackTypeFirst = 0; // 第一形態の技の種類

	int ghBoss1 = Novice::LoadTexture("./Resources/images/Doragon_1.png"); // 第一形態のボスの画像
	int ghBoss1Move = 0; // ボスの画像の左上の座標

	int frameCount = 0; // フレーム
	int bossAnimCount = 0; // ボスのアニメカウント
	int bossCountChange = 0; // ボスのカウントが変わったかチェックする変数
	float ghBoss1TotalWidth = 1920.0f; // ボスの画像の最大幅
	float ghBoss1Width = 296.0f; // ボスの一枚ずつの画像の幅

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

			//スティックの値を取得する
			Novice::GetAnalogInputLeft(0, &padX, &padY);

			//左右移動(AD or 左スティック)
			if (keys[DIK_A] || padX <= -1)
			{
				player.pos.x -= player.speed;
				if (!shortSword.isAtk && !longSword.isAtk)
				{
					player.isDirections = true;
				}
			}

			if (keys[DIK_D] || padX >= 1)
			{
				player.pos.x += player.speed;
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
			}

			//==============================================================
			//攻撃
			//==============================================================

			//短剣(J or X)
			if (keys[DIK_J] && !preKeys[DIK_J] || Novice::IsPressButton(0, PadButton::kPadButton12))
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

			//大剣(K or Y)
			if (keys[DIK_K] && !preKeys[DIK_K] || Novice::IsPressButton(0, PadButton::kPadButton13))
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
			shortSword.pos.y = player.pos.y + player.height + shortSword.height;
			if (!player.isDirections)//右
			{
				shortSword.pos.x = player.pos.x;
			}
			else//左
			{
				shortSword.pos.x = player.pos.x - shortSword.width;
			}
			//大剣
			longSword.pos.y = player.pos.y + player.height + longSword.height;
			if (!player.isDirections)//右
			{
				longSword.pos.x = player.pos.x;
			}
			else//左
			{
				longSword.pos.x = player.pos.x - longSword.width;
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

			if (!boss.isAttacking)
			{
				if (boss.attackCoolTimer > 0)
				{
					boss.attackCoolTimer--;
				}
				else if (boss.attackCoolTimer <= 0)
				{
					boss.attackCoolTimer = 0;

					boss.isAttacking = true;

					if (boss.hpCount <= 50)
					{
						attackTypeFirst = rand() % 4;
					}
					else if (boss.hpCount <= 90)
					{
						attackTypeFirst = rand() % 3;
					}
					else if (boss.hpCount <= 100)
					{
						attackTypeFirst = rand() % 2;
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
							for (int i = 0; i < slowFireMax; i++)
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

					for (int i = 0; i < slowFireMax; i++)
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
							}
						}
					}

					if (fireDisappearCount == 8)
					{
						boss.isAttacking = false;
						boss.attackCoolTimer = 90;
						fireDisappearCount = 0;
						fireShootCount = 0;

						break;
					}

					break;

				case FASTFIRE:
					if (fireShootCount <= 3)
					{
						if (boss.fireCoolTimer <= 0)
						{
							for (int i = 8; i < fastFireMax; i++)
							{
								if (!smallFire[i].isShot)
								{
									smallFire[i].pos.x = boss.pos.x + 16.0f;
									smallFire[i].pos.y = boss.pos.y - 64.0f;
									f2pDistance = sqrtf(powf(player.pos.x - smallFire[i].pos.x, 2) + powf(player.pos.y - smallFire[i].pos.y, 2));

									if (f2pDistance != 0.0f)
									{
										smallFire[i].direction.x = (player.pos.x - smallFire[i].pos.x) / f2pDistance;
										smallFire[i].direction.y = (player.pos.y - smallFire[i].pos.y) / f2pDistance;
									}

									smallFire[i].isShot = true;
									fireShootCount++;

									break;
								}
							}

							boss.fireCoolTimer = 30;
						}
					}

					if (boss.fireCoolTimer > 0)
					{
						boss.fireCoolTimer--;
					}

					for (int i = 8; i < fastFireMax; i++)
					{
						if (smallFire[i].isShot)
						{
							smallFire[i].pos.x += smallFire[i].direction.x * smallFire[i].speed;
							smallFire[i].pos.y += smallFire[i].direction.y * smallFire[i].speed;

							if (smallFire[i].pos.y <= 0.0f + smallFire[i].height / 2.0f ||
								smallFire[i].pos.x <= 0.0f - smallFire[i].width)
							{
								smallFire[i].isShot = false;
								fireDisappearCount++;
							}
						}
					}

					if (fireDisappearCount == 4)
					{
						boss.isAttacking = false;
						boss.attackCoolTimer = 90;
						fireDisappearCount = 0;
						fireShootCount = 0;
						f2pDistance = 0.0f;

						break;
					}

					break;

				case MULTIPLEFIRE:
					if (fireShootCount <= 21)
					{
						if (boss.fireCoolTimer <= 0)
						{
							smallFire[8].pos.x = boss.pos.x + 16.0f;
							smallFire[8].pos.y = boss.pos.y - 64.0f;
							f2pDistance = sqrtf(powf(player.pos.x - smallFire[8].pos.x, 2) + powf(player.pos.y - smallFire[8].pos.y, 2));
							if (f2pDistance != 0.0f)
							{
								for (int i = 12; i < multiple1Max; i++)
								{
									if (!smallFire[i].isShot)
									{
										smallFire[i].isShot = true;
										smallFire[i].pos.x = boss.pos.x + 16.0f;
										smallFire[i].pos.y = boss.pos.y - 64.0f;
										smallFire[i].direction.x = cosf(2.0f / 3.0f * static_cast<float>(M_PI));
										smallFire[i].direction.y = sinf(2.0f / 3.0f * static_cast<float>(M_PI));
										fireShootCount++;

										break;
									}
								}
								for (int i = 20; i < multiple2Max; i++)
								{
									if (!smallFire[i].isShot)
									{
										smallFire[i].isShot = true;
										smallFire[i].pos.x = boss.pos.x + 16.0f;
										smallFire[i].pos.y = boss.pos.y - 64.0f;
										smallFire[i].direction.x = cosf(5.0f / 6.0f * static_cast<float>(M_PI));
										smallFire[i].direction.y = sinf(5.0f / 6.0f * static_cast<float>(M_PI));
										fireShootCount++;

										break;
									}
								}
								for (int i = 28; i < multiple3Max; i++)
								{
									if (!smallFire[i].isShot)
									{
										smallFire[i].isShot = true;
										smallFire[i].pos.x = boss.pos.x + 16.0f;
										smallFire[i].pos.y = boss.pos.y - 64.0f;
										smallFire[i].direction.x = cosf(static_cast<float>(M_PI));
										smallFire[i].direction.y = sinf(static_cast<float>(M_PI));
										fireShootCount++;

										break;
									}
								}

								boss.fireCoolTimer = 30;
							}
						}
					}

					if (boss.fireCoolTimer > 0)
					{
						boss.fireCoolTimer--;
					}

					for (int i = 12; i < multipleFireMax; i++)
					{
						if (smallFire[i].isShot)
						{
							smallFire[i].pos.x += smallFire[i].speed * smallFire[i].direction.x;

							//重力
							if (smallFire[i].pos.y - smallFire[i].width / 2.0f > 0.0f)
							{
								smallFire[i].pos.y += smallFire[i].speed * smallFire[i].direction.y;
								smallFire[i].pos.y += smallFire[i].gravity -= 0.16f;
							}
							else
							{
								smallFire[i].gravity = 0.0f;
							}

							if (smallFire[i].pos.x <= 0.0f - smallFire[i].width ||
								smallFire[i].pos.y <= 0.0f + smallFire[i].height / 2.0f)
							{
								smallFire[i].isShot = false;
								fireDisappearCount++;
							}
						}
					}

					if (fireDisappearCount == 24)
					{
						boss.isAttacking = false;
						boss.attackCoolTimer = 90;
						fireDisappearCount = 0;
						fireShootCount = 0;

						for (int i = 12; i < multipleFireMax; i++)
						{
							smallFire[i].gravity = 0.0f;
						}

						break;
					}

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
								giantFire.pos.x = boss.pos.x;
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
										giantFire.direction.y = (player.pos.y - giantFire.pos.y) / f2pDistance;
									}

									boss.isCharging = false;
									giantFire.isShot = true;
								}
							}

							if (giantFire.isShot)
							{
								giantFire.pos.x += giantFire.speed * giantFire.direction.x;
								giantFire.pos.y += giantFire.speed * giantFire.direction.y;

								if (giantFire.pos.x <= 0.0f - giantFire.width ||
									giantFire.pos.y <= 0.0f + giantFire.height / 2.0f)
								{
									giantFire.isShot = false;
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
						}

						if (!boss.isHovering && fireDisappearCount == 1)
						{
							boss.isAttacking = false;
							boss.chargeTimer = 0;
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

			// 小炎全体の当たり判定
			for (int i = 0; i < smallFireMax; i++)
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

			// 小炎攻撃(連射)
			for (int i = 0; i < slowFireMax; i++)
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

			for (int i = 8; i < fastFireMax; i++)
			{
				// 小炎攻撃(高速)
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

			for (int i = 12; i < multipleFireMax; i++)
			{
				// 小炎攻撃(高速)
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

			if (giantFire.isShot)
			{
				Novice::DrawBox(
					static_cast<int>(giantFire.pos.x - giantFire.width / 2),
					static_cast<int>(ToScreen(giantFire.pos.y + giantFire.height / 2.0f)),
					static_cast<int>(giantFire.width),
					static_cast<int>(giantFire.height),
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
			Novice::ScreenPrintf(100, 180, "player hp: %d", player.hpCount);

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
