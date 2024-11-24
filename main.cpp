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
	float width;
	float height;
	int color;
	float speed;
	float jump;
	float gravity;
	int isJump;
	int isAlive;
	int isDirections;
	int hpCount;
	int isWalk;
	int isNoDamage;
	int noDamageTime;
	int isFlying; 
	float flyDistance; 
	float flySpeed;

	//描画用
	Vector2 adjustment;
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
	int isFlying;
	int isInScreen;
	int direction;
	int form;
	int isfalling;
	int fallTimer; 
	float fallSpeed; 
	int isPlayerHit;
	int changedDirection;
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
};

//パーティクル
struct Particle
{
	Vector2 pos;
	float width;
	float height;
	float rotation;
	float circumference;
	float speed;
	int color;
	int isDisplay;
	int isSmallFire;
};

struct BackGround
{
	Vector2 pos;
	int color;
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

enum ATTACKFIRST
{
	MOVE,
	SLOWFIRE,
	FASTFIRE,
	MULTIPLEFIRE,
	GIANTFIRE,
	FLY
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

void BossMove(Boss* boss, float imageWidth)
{
	if (boss->isInScreen) // スクリーン上にいるときスクリーン外まで移動
	{
		if (boss->direction == LEFT)
		{
			if (boss->pos.x < 1280.0f + imageWidth)
			{
				boss->pos.x += boss->speed;
			}

			if (boss->pos.x >= 1280.0f + imageWidth)
			{
				boss->pos.x = -612.0f;
				boss->isInScreen = false;
				boss->direction = RIGHT;
				boss->changedDirection = true;
			}
		} else if (boss->direction == RIGHT)
		{
			if (boss->pos.x > 0 - imageWidth)
			{
				boss->pos.x -= boss->speed;

			}

			if (boss->pos.x <= 0.0f - imageWidth)
			{
				boss->pos.x = 1456.0f;
				boss->isInScreen = false;
				boss->direction = LEFT;
				boss->changedDirection = true;
			}
		}
	}

	if (!boss->isInScreen) // スクリーン外にいるとき反対側に移動して向きを反転し、画面内まで移動
	{
		if (boss->direction == RIGHT)
		{
			if (boss->pos.x < 160.0f)
			{
				boss->pos.x += boss->speed;
			}

			if (boss->pos.x >= 160.0f)
			{
				boss->pos.x = 160.0f;
				boss->isAttacking = false;
				boss->isInScreen = true;
				boss->attackCoolTimer = 20;
				boss->changedDirection = false;
			}
		} else if (boss->direction == LEFT)
		{
			if (boss->pos.x > 840.0f)
			{
				boss->pos.x -= boss->speed;
			}

			if (boss->pos.x <= 840.0f)
			{
				boss->pos.x = 840.0f;
				boss->isAttacking = false;
				boss->isInScreen = true;
				boss->attackCoolTimer = 20;
				boss->changedDirection = false;
			}
		}
	}
}

void SlowFire(const int kMax, Attack smallFire[], Boss* boss, int& shootCount, int& disappearCount)
{
	if (shootCount <= 7)
	{
		if (boss->fireCoolTimer <= 0)
		{
			for (int i = 0; i < kMax; ++i)
			{
				if (!smallFire[i].isShot)
				{
					smallFire[i].isShot = true;

					if (boss->direction == LEFT)
					{
						smallFire[i].pos.x = boss->pos.x;
					} else if (boss->direction == RIGHT)
					{
						smallFire[i].pos.x = boss->pos.x + 256.0f;
					}

					smallFire[i].pos.y = boss->pos.y - 120.0f;
					shootCount++;

					break;
				}
			}

			boss->fireCoolTimer = 40;

		}
	}

	if (boss->fireCoolTimer > 0)
	{
		boss->fireCoolTimer--;
	}

	for (int i = 0; i < kMax; ++i)
	{
		if (smallFire[i].isShot)
		{
			if (boss->direction == LEFT)
			{
				smallFire[i].pos.x -= smallFire[i].speed;
			} else if (boss->direction == RIGHT)
			{
				smallFire[i].pos.x += smallFire[i].speed;
			}

			//重力
			if (smallFire[i].pos.y - smallFire[i].width > 0.0f)
			{
				smallFire[i].pos.y += smallFire[i].gravity -= 0.8f;
			} else
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
				disappearCount++;
			}
		}
	}

	if (disappearCount == 8)
	{
		boss->isAttacking = false;
		boss->attackCoolTimer = 60;
		disappearCount = 0;
		shootCount = 0;
	}
}

void MultipleFire(const int kMax, Attack smallFire[], Boss* boss, int& shootCount, int& disappearCount)
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
					smallFire[i].isShot = true;
					smallFire[i].pos.y = boss->pos.y - 120.0f;
					if (boss->direction == LEFT)
					{
						smallFire[i].pos.x = boss->pos.x;
						smallFire[i].direction.x = cosf(((i % 3) + 4) / 6.0f * static_cast<float>(M_PI));
						smallFire[i].direction.y = sinf(((i % 3) + 4) / 6.0f * static_cast<float>(M_PI));
					} else if (boss->direction == RIGHT)
					{
						smallFire[i].pos.x = boss->pos.x + boss->width;
						smallFire[i].direction.x = cosf((i % 3) / 6.0f * static_cast<float>(M_PI));
						smallFire[i].direction.y = sinf((i % 3) / 6.0f * static_cast<float>(M_PI));
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
				smallFire[i].pos.y += smallFire[i].gravity -= 0.24f;
			} else
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
						boss->attackCoolTimer = 90;
						smallFire[i].gravity = 0.0f;
						disappearCount = 0;
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
					smallFire[i].pos.y = boss->pos.y - 120.0f;
					if (boss->direction == LEFT)
					{
						smallFire[i].pos.x = boss->pos.x;
					} else if (boss->direction == RIGHT)
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

void GiantFire(Attack* giantFire, Attack* explosion, Boss* boss, Player player, int& disappearCount)
{
	if (!boss->isHovering && disappearCount <= 0)//ボスが滞空していないとき&攻撃が1発も撃たれていないとき

	{
		if (!boss->isFlying)
		{
			boss->isFlying = true;
		}
		//ボスが上にいく処理
		if (boss->pos.y < 600.0f)
		{
			boss->pos.y += boss->speed;
		}
		//ボスが上に上がりきったときの処理
		if (boss->pos.y >= 600.0f)
		{
			boss->isHovering = true;//最高地点で飛んでいる
			boss->isCharging = true;//攻撃のためにはいる
			boss->chargeTimer = 120;

			if (boss->direction == LEFT)
			{
				giantFire->pos.x = boss->pos.x;
			}
			if (boss->direction == RIGHT)
			{
				giantFire->pos.x = boss->pos.x + boss->width;
			}

			giantFire->pos.y = boss->pos.y;
		}
	}

	if (boss->isHovering)//滞空しているとき
	{
		if (boss->isCharging)//攻撃をためているとき
		{
			if (boss->chargeTimer > 0)
			{
				boss->chargeTimer--;
			} else//チャージ完了
			{
				//この時点でのプレイヤーの位置に攻撃を飛ばすためのベクトルの計算
				float f2pDistance = sqrtf(powf(player.pos.x - giantFire->pos.x, 2) + powf(player.pos.y - giantFire->pos.y, 2));
				//正規化
				if (f2pDistance != 0.0f)
				{
					giantFire->direction.x = (player.pos.x - giantFire->pos.x) / f2pDistance;
					giantFire->direction.y = (player.pos.y - giantFire->pos.y + giantFire->height) / f2pDistance;

					boss->isCharging = false;
					giantFire->isShot = true;
				}
			}
		}

		if (giantFire->isShot) // 火球が撃たれた時
		{
			giantFire->pos.x += giantFire->speed * giantFire->direction.x;
			giantFire->pos.y += giantFire->speed * giantFire->direction.y;

			if (giantFire->pos.x <= 0.0f - giantFire->width ||
				giantFire->pos.y <= 0.0f + giantFire->height || giantFire->pos.x >= 1400.0f || giantFire->pos.y >= 800.0f)
			{
				if (giantFire->pos.y <= 0.0f + giantFire->height)
				{
					giantFire->pos.y = 0.0f + giantFire->height;
				}

				giantFire->isShot = false;
				explosion->pos.x = giantFire->pos.x - 192.0f;
				explosion->pos.y = giantFire->pos.y;
				explosion->isShot = true;
				explosion->duration = 30;
			}
		}

		if (explosion->isShot) // 爆発が起きたとき
		{
			explosion->duration--;

			if (explosion->duration <= 0)
			{
				explosion->isShot = false;
				explosion->width = 512.0f;
				explosion->height = 512.0f;
				disappearCount = 1;
			}
		}

		if (!giantFire->isShot && disappearCount == 1) // 攻撃し終わって、爆発も消えたとき
		{
			if (boss->pos.y > 320.0f)
			{
				boss->pos.y -= boss->speed;
			} else
			{
				boss->isHovering = false;
			}
		}
	}

	if (!boss->isHovering && disappearCount == 1) // 攻撃し終わって滞空していないとき
	{
		boss->isFlying = false;
		boss->isAttacking = false;
		boss->chargeTimer = 0;
		boss->attackCoolTimer = 180;
		disappearCount = 0;
	}
}

#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

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
	player.width = 40.0f; //横幅
	player.height = 90.0f; //縦幅
	player.color = 0xFFFFFFFF; //色
	player.speed = 10.0f; //移動速度
	player.jump = 15.0f; //ジャンプ速度
	player.gravity = 0.0f; //重力
	player.isJump = false; //ジャンプ状態か否か
	player.isAlive; //生存
	player.isDirections = false; //プレイヤーの向いている方向(false = 右,true = 左)
	player.hpCount = 10; //hp
	player.isWalk = false; //移動しているか(モーション切り替えに使う)
	player.isNoDamage = false;
	player.noDamageTime = 0; //無敵時間
	player.isFlying = false; //飛ばされているか
	player.flyDistance = 0.0f; //吹き飛ぶ距離
	player.flySpeed = 8.0f; //吹き飛ばされる時の速度

	//プレイヤー描画用
	Player playerDrawing;
	playerDrawing.pos.x = 0.0f; //ｘ座標
	playerDrawing.pos.y = 0.0f; //ｙ座標
	playerDrawing.adjustment.x = 44.0f; //調整幅ｘ
	playerDrawing.adjustment.y = 40.0f; //調整幅ｙ

	//短剣
	Sword shortSword;
	shortSword.pos.x = 100.0f; //ｘ座標
	shortSword.pos.y = 100.0f; //ｙ座標
	shortSword.width = 128.0f; //縦幅
	shortSword.height = 128.0f; //横幅
	shortSword.coolTime = 0; //攻撃クールタイム
	shortSword.durationTime = 30; //攻撃の持続時間
	shortSword.isAtk = false; //攻撃しているか
	shortSword.isBossHit = false; //攻撃が当たっているか(ボスに)
	shortSword.damage = 3; //攻撃力
	shortSword.isReaction = false; //硬直が起きているか(アニメーション切り替えにも使う)
	shortSword.reactionTime = 30; //硬直で動けない時間

	//大剣
	Sword longSword;
	longSword.pos.x = 100.0f; //ｘ座標
	longSword.pos.y = 100.0f; //ｙ座標
	longSword.width = 192.0f; //縦幅
	longSword.height = 192.0f; //横幅
	longSword.coolTime = 0; //攻撃クールタイム
	longSword.durationTime = 30; //攻撃の持続時間
	longSword.isAtk = false; //攻撃しているか
	longSword.isBossHit = false; //攻撃が当たっているか(ボスに)
	longSword.damage = 5; //攻撃力
	longSword.isReaction = false; //硬直が起きているか(アニメーション切り替えにも使う) 
	longSword.reactionTime = 60; //硬直で動けない時間 
	longSword.isBossHit = false; //攻撃が当たっているか(ボスに) 
	longSword.isSmallFireHit = false; //攻撃が当たっているか(smallFireに)

	//待機画像
	int ghPlayer[2] =
	{
		Novice::LoadTexture("./Resources/images/player_right.png"), // プレイヤー左向きの待機画像 
		Novice::LoadTexture("./Resources/images/player_left.png"), // プレイヤー右向きの待機画像
	};

	int ghPlayerFrameCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float ghPlayerMaxWidth = 768.0f; // プレイヤーの画像の最大横幅 
	float ghPlayerWidth = 128.0f; // プレイヤーの1フレームの画像横幅
	float ghPlayerHeight = 128.0f; //プレイヤーの画像の縦幅

	//移動画像
	int ghPlayerWalk[2] =
	{
		Novice::LoadTexture("./Resources/images/player_walk_R.png"), // プレイヤー移動画像
		Novice::LoadTexture("./Resources/images/player_walk_L.png"), // プレイヤー移動画像
	};

	int ghPlayerWalkFrameCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float ghPlayerWalkMaxWidth = 512.0f; // プレイヤーの画像の最大横幅 
	float ghPlayerWalkWidth = 128.0f; // プレイヤーの1フレームの画像横幅
	float ghPlayerWalkHeight = 128.0f; //プレイヤーの画像の縦幅

	//ジャンプ
	int ghPlayerJump[2] =
	{
		Novice::LoadTexture("./Resources/images/player_jump_R.png"), // プレイヤー移動画像
		Novice::LoadTexture("./Resources/images/player_jump_L.png"), // プレイヤー移動画像
	};

	int ghPlayerJumpFrameCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float ghPlayerJumpMaxWidth = 640.0f; // プレイヤーの画像の最大横幅 
	float ghPlayerJumpWidth = 128.0f; // プレイヤーの1フレームの画像横幅
	float ghPlayerJumpHeight = 128.0f; //プレイヤーの画像の縦幅

	//弱攻撃
	int ghPlayerAtkS[2] =
	{
		Novice::LoadTexture("./Resources/images/player_Attack_R.png"), // プレイヤー移動画像
		Novice::LoadTexture("./Resources/images/player_Attack_L.png"), // プレイヤー移動画像
	};

	int ghPlayerAtkSFrameCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float ghPlayerAtkSMaxWidth = 256.0f; // プレイヤーの画像の最大横幅 
	float ghPlayerAtkSWidth = 128.0f; // プレイヤーの1フレームの画像横幅
	float ghPlayerAtkSHeight = 128.0f; //プレイヤーの画像の縦幅

	//強攻撃
	int ghPlayerAtkL[2] =
	{
		Novice::LoadTexture("./Resources/images/Player_strong_attack_R.png"), // プレイヤー移動画像
		Novice::LoadTexture("./Resources/images/Player_strong_attack_L.png"), // プレイヤー移動画像
	};

	int ghPlayerAtkLFrameCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float ghPlayerAtkLMaxWidth = 800.0f; // プレイヤーの画像の最大横幅 
	float ghPlayerAtkLWidth = 160.0f; // プレイヤーの1フレームの画像横幅
	float ghPlayerAtkLHeight = 160.0f; //プレイヤーの画像の縦幅

	//ジャンプ強攻撃
	int ghPlayerJumpAtkL[2] =
	{
		Novice::LoadTexture("./Resources/images/Player_strong_attack_sky_R.png"), // プレイヤー移動画像
		Novice::LoadTexture("./Resources/images/Player_strong_attack_sky_L.png"), // プレイヤー移動画像
	};

	int ghPlayerJumpAtkLFrameCount = 0; // プレイヤーのアニメーションｎフレームカウント 
	float ghPlayerJumpAtkLMaxWidth = 800.0f; // プレイヤーの画像の最大横幅 
	float ghPlayerJumpAtkLWidth = 160.0f; // プレイヤーの1フレームの画像横幅
	float ghPlayerJumpAtkLHeight = 160.0f; //プレイヤーの画像の縦幅


#pragma endregion

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	unsigned int currentTime = static_cast<unsigned int>(time(nullptr));
	srand(currentTime);

#pragma region ボス
	Boss boss;
	boss.pos = { 840.0f, 320.0f }; // 左上座標
	boss.speed = 8.0f; // 移動速度
	boss.attackCoolTimer = 0; // 攻撃のクールタイム
	boss.isAlive = true; // 生きているか
	boss.isChange = false; // 形態変化用のフラグ
	boss.hpCount = 200; // 体力
	boss.width = 288.0f; // 横幅(当たり判定用)
	boss.height = 320.0f; // 縦幅(当たり判定用)
	boss.fallSpeed = 16.0f;


	//ボス攻撃
	boss.attackCoolTimer = 60; // クールタイム
	boss.fireCoolTimer = 0; // 小炎攻撃用のタイマー
	boss.isAttacking = false; // 攻撃しているか
	boss.isCharging = false; // 攻撃をチャージしているか
	boss.chargeTimer = 120; // チャージタイム
	boss.isHovering = false; // 滞空しているか
	boss.isFlying = false;
	boss.direction = LEFT; // 方向
	boss.form = 0; 
	boss.isfalling = false;
	boss.fallTimer = 0; 
	boss.isPlayerHit = false;
	boss.changedDirection = false;



	// ボス移動
	boss.isInScreen = true;

	const int kMaxSmallFire = 24; // 小炎の最大数
	const int kMaxSlowFire = 8; // 低速小炎の最大数
	const int kMaxFastFire = 4; // 高速小炎の最大数
	const int kMaxMultiple = 24; // 拡散小炎の最大数

	int fireShootCount = 0; // 炎を撃ったカウント
	int fireDisappearCount = 0; // 炎が消えたカウント

	Attack smallFire[24];

	for (int i = 0; i < kMaxSmallFire; i++)
	{
		smallFire[i].pos = { 0.0f }; // 座標
		smallFire[i].width = 32.0f; // 横幅
		smallFire[i].height = 32.0f; // 縦幅
		smallFire[i].speed = 0.0f; // 速度
		smallFire[i].isShot = false; // 撃たれたか
		smallFire[i].gravity = 0.0f; //重力
		smallFire[i].direction = { 0.0f };
		smallFire[i].isPlayerHit = false; //プレイヤーに当たったか
		smallFire[i].isBossHit = false; // 反射された攻撃がボスに当たったか 
		smallFire[i].isReflection = false; // 反射されたか 
		smallFire[i].reflectionDamage = 5; // 反射された攻撃がボスに当たった時のダメージ
	}

	float slowFireSpeed = 5.0f; // 低速小炎のスピード
	float fastFireSpeed = 20.0f; // 高速小炎のスピード
	float multipleFireSpeed = 10.0f; // 拡散小炎のスピード
	float dropSpeed = 8.0f; // 雫のスピード

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
	explosion.width = 512.0f; // 横幅
	explosion.height = 512.0f; // 縦幅
	explosion.duration = 0; // 持続時間
	explosion.isPlayerHit = false; // プレイヤーに当たったか
	explosion.isShot = false; // 撃たれたか
	
	Attack flash; 
	flash.pos = { 0.0f }; // 座標
	flash.width = 128.0f; // 横幅
	flash.height = 128.0f; // 縦幅
	flash.duration = 0; // 持続時間
	flash.isPlayerHit = false; // プレイヤーに当たったか
	flash.isShot = false; // 撃たれたか 
	flash.duration = 0; 

	int attackTypeFirst = 0; // 第一形態の技の種類

	int ghBoss1Left = Novice::LoadTexture("./Resources/images/Doragon_1.png"); // 第一形態のボスの画像
	int ghBoss1Right = Novice::LoadTexture("./Resources/images/Doragon_1_right.png"); // 第一形態のボスの画像
	int ghBoss1Sky = Novice::LoadTexture("./Resources/images/sky_Dragon.png"); // 画面中央で羽ばたくボスの画像(形態変化技)
	int ghBoss1FlyLeft = Novice::LoadTexture("./Resources/images/sky_Dragonx.png"); // 画面左で飛んでいる時のボスの画像
	int ghBoss1FlyRight = Novice::LoadTexture("./Resources/images/sky_Dragony.png"); // 画面右で飛んでいる時のボスの画像
	int ghBoss2Temp = Novice::LoadTexture("./Resources/images/boss_temporary.png");
	int ghBoss2Fall = Novice::LoadTexture("./Resources/images/boss_ground.png"); 
	int ghExplosion = Novice::LoadTexture("./Resources/images/flame_explosion.png");


	int bossAnimeCount = 0; // ボスのアニメーションｎフレームカウント
	float boss1MaxImageWidth = 5760.0f; // ボスの画像の最大横幅
	float boss1FrameImageWidth = 640.0f; // ボスの1フレームの画像横幅
	float boss1ImageHeight = 352.0f; //ボスの画像の縦幅

	int bossSkyAnimeCount = 0; // ボスのアニメーションｎフレームカウント
	float boss1SkyFrameWidth = 576.0f; // ボスの1フレームの画像横幅
	float boss1SkyImageHeight = 320.0f; //ボスの画像の縦幅

	int bossFlyAnimeCount = 0; // ボスのアニメーションｎフレームカウント
	float boss1FlyFrameWidth = 640.0f; // ボスの画像の最大横幅
	float boss1FlyImageHeight = 448.0f; // ボスの1フレームの画像横幅
	float boss1FlyMaxWidth = 5120.0f; //ボスの画像の縦幅

	float boss2TempFrameWidth = 384.0f; 
	float boss2TempMaxImageWidth = 3072.0f; 
	float boss2TempImageHeight = 240.0f;
	float boss2FallWidth = 336.0f; 
	float boss2FallHeight = 192.0f;
	int explosionAnimeCount = 0;
	float explosionMaxImageWidth = 2048.0f;
	float explosionFrameWidth = 512.0f;
	float explosionImageHeight = 512.0f; 

#pragma endregion

#pragma region パーティクル

	//----------------パーティクル----------------//
  
	//プレイヤーの軌跡
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

	//小炎の軌跡
	int smallFireLocusMax = 200; //最大表示数
	Particle smallFireLocus[24][200];
	for (int i = 0; i < kMaxSmallFire; i++)
	{
		for (int j = 0; j < smallFireLocusMax; j++)
		{
			smallFireLocus[i][j].pos.x = 0.0f; //ｘ座標
			smallFireLocus[i][j].pos.y = 0.0f; //ｙ座標
			smallFireLocus[i][j].width = 32.0f; //横幅
			smallFireLocus[i][j].height = 32.0f; //縦幅
			smallFireLocus[i][j].rotation = 0.0f; //回転角
			smallFireLocus[i][j].color = 0xFF0000FF; //色
			smallFireLocus[i][j].isDisplay = false; //表示されているか
		}
	}
	int smallFireLocusCoolTime = 240;//表示のクールタイム

	//巨大火球の軌跡
	int giantFireLocusMax = 300; //最大表示数
	Particle giantFireLocus[300];
	for (int i = 0; i < giantFireLocusMax; i++)
	{
		giantFireLocus[i].pos.x = 0.0f; //ｘ座標
		giantFireLocus[i].pos.y = 0.0f; //ｙ座標
		giantFireLocus[i].width = 128.0f; //横幅
		giantFireLocus[i].height = 128.0f; //縦幅
		giantFireLocus[i].rotation = 0.0f; //回転角
		giantFireLocus[i].color = 0xFF0000FF; //色
		giantFireLocus[i].isDisplay = false; //表示されているか
	}
	int giantFireLocusCoolTime = 240;//表示のクールタイム

	//地面から出るオーラ
	int powderAuraMax = 50; //最大表示数
	Particle powderAura[50];
	for (int i = 0; i < powderAuraMax; i++) {
		powderAura[i].pos.x = 0.0f; //ｘ座標
		powderAura[i].pos.y = 0.0f; //ｙ座標
		powderAura[i].width = 10.0f; //横幅
		powderAura[i].height = 10.0f; //縦幅
		powderAura[i].rotation = 0.0f; //回転角
		powderAura[i].color = 0xFF0000FF; //色
		powderAura[i].isDisplay = false; //表示されているか
	}
	int powderAuraCoolTime = 240;//表示のクールタイム

	//ヒットエフェクト
	int hitEffectMax = 100; //最大表示数
	Particle hitEffect[100];
	for (int i = 0; i < hitEffectMax; i++) {
		hitEffect[i].pos.x = 0.0f; //ｘ座標
		hitEffect[i].pos.y = 0.0f; //ｙ座標
		hitEffect[i].width = 5.0f; //横幅
		hitEffect[i].height = 32.0f; //縦幅
		hitEffect[i].speed = 0.0f; //速度
		hitEffect[i].rotation = 0.0f; //回転角
		hitEffect[i].circumference = 0.0f; //円周サイズ
		hitEffect[i].color = WHITE; //色
		hitEffect[i].isDisplay = false; //表示されているか
	}
	
	//小炎のヒットエフェクト
	int hitFireEffectMax = 200; //最大表示数
	Particle hitFireEffect[200];
	for (int i = 0; i < hitFireEffectMax; i++) {
		hitFireEffect[i].pos.x = 0.0f; //ｘ座標
		hitFireEffect[i].pos.y = 0.0f; //ｙ座標
		hitFireEffect[i].width = 64.0f; //横幅
		hitFireEffect[i].height = 64.0f; //縦幅
		hitFireEffect[i].speed = 0.0f; //速度
		hitFireEffect[i].rotation = 0.0f; //回転角
		hitFireEffect[i].circumference = 0.0f; //円周サイズ
		hitFireEffect[i].color = RED; //色
		hitFireEffect[i].isDisplay = false; //表示されているか
	}

	float effectTheta = 0; //エフェクトを円形に配置するためのθ

#pragma endregion

	//背景
	BackGround backGround;
	backGround.pos.x = 0.0f; //x座標
	backGround.pos.y = 0.0f; //y座標
	backGround.color = 0xFFFFFFFF; //色

	int ghBackGround1 = Novice::LoadTexture("./Resources/images/bg1.png");

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

#pragma region プレイヤー
			//===========================================================
			//プレイヤー
			//===========================================================

			if (phase == 0)
			{
				//攻撃時は動けない
				if (!shortSword.isAtk && !longSword.isAtk)
				{
					//左右移動(AD or 左スティック)
					if (keys[DIK_A] || padX <= -1)
					{
						player.pos.x -= player.speed;
						player.isDirections = true;

						//パーティクル軌跡
						for (int i = 0; i < playerLocusMax; i++) {
							if (playerLocusCoolTime >= 0) {
								playerLocusCoolTime--;
							} else {
								playerLocus[i].isDisplay = true;
								playerLocusCoolTime = 240;
							}
						}
					}


					if (keys[DIK_D] || padX >= 1)
					{
						player.pos.x += player.speed;
						player.isDirections = false;

						//パーティクル軌跡
						for (int i = 0; i < playerLocusMax; i++) {
							if (playerLocusCoolTime >= 0) {
								playerLocusCoolTime--;
							} else {
								playerLocus[i].isDisplay = true;
								playerLocusCoolTime = 240;
							}
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
							} else {
								playerLocus[i].isDisplay = true;
								playerLocusCoolTime = 240;
							}
						}
					}
				}

				//移動モーションと待機モーション切り替え
				if (keys[DIK_A] || keys[DIK_D] || padX >= 1 || padX <= -1)
				{
					player.isWalk = true;
				} else
				{
					player.isWalk = false;
				}

				//==============================================================
				//攻撃
				//==============================================================

				//攻撃時は動けない
				if (!shortSword.isAtk && !longSword.isAtk)
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
								longSword.coolTime = 20;
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
								shortSword.coolTime = 20;
								longSword.coolTime = 40;
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
					} else
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


				//------------攻撃時の硬直によって動けない時間------------//

				if (shortSword.isReaction) //短剣
				{
					if (shortSword.reactionTime >= 0)
					{
						shortSword.reactionTime--;
						player.gravity = 0.5f;
						player.speed = 0.0f;
						player.jump = 0.0f;
					} else
					{
						shortSword.reactionTime = 10;
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
					} else
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
				} else//左
				{
					shortSword.pos.x = player.pos.x + player.width / 2.0f - shortSword.width;
				}

				//大剣
				longSword.pos.y = player.pos.y - player.height + longSword.height;
				if (!player.isDirections)//右
				{
					longSword.pos.x = player.pos.x + player.width / 2.0f;
				} else//左
				{
					longSword.pos.x = player.pos.x + player.width / 2.0f - longSword.width;
				}

				//----------------重力------------------//

				if (player.pos.y - player.height > 0.0f)
				{
					player.pos.y += player.gravity -= 0.7f;
				} else
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

				if (player.hpCount <= 0)
				{
					player.isAlive = false;
				}

				//描画用変数の座標更新
				playerDrawing.pos.x = player.pos.x - playerDrawing.adjustment.x; //ｘ座標
				playerDrawing.pos.y = player.pos.y + playerDrawing.adjustment.y; //ｙ座標

#pragma endregion

#pragma region ボス
				//===========================================================
				//ボスの移動 
				//===========================================================

				if (phase == 0 || phase == 2)
				{
					if (!boss.isChange)
					{
						if (!boss.isAttacking)
						{
							if (boss.attackCoolTimer > 0)
							{
								boss.attackCoolTimer--;
							} else if (boss.attackCoolTimer <= 0)
							{
								//初期化
								for (int i = 0; i < kMaxSmallFire; ++i)
								{
									smallFire[i].isShot = false;
									smallFire[i].gravity = 0.0f;
									fireShootCount = 0;
								}

								boss.attackCoolTimer = 0;

								boss.isAttacking = true;

								// 使う技の種類をランダムで決める
								// 残り体力によって変わる

								if (phase == 0)
								{
									if (boss.hpCount <= 0)
									{
										boss.hpCount = 0;
										attackTypeFirst = 5;
									} 
									else if (boss.hpCount > 0)
									{
										if (boss.hpCount <= 150)
										{
											attackTypeFirst = rand() % 5;
										}
										else if (boss.hpCount <= 190)
										{
											attackTypeFirst = rand() % 4;
										} 
										else if (boss.hpCount <= 200)
										{
											attackTypeFirst = rand() % 3;
										}
									}
								}

								if (attackTypeFirst == 1)
								{
									for (int i = 0; i < kMaxSlowFire; i++) // 攻撃の種類によって炎のスピードを変える
									{
										smallFire[i].speed = slowFireSpeed;
									}
								} else if (attackTypeFirst == 2)
								{
									for (int i = 0; i < kMaxFastFire; i++) // 攻撃の種類によって炎のスピードを変える
									{
										smallFire[i].speed = fastFireSpeed;
									}
								} else if (attackTypeFirst == 3)
								{
									for (int i = 0; i < kMaxMultiple; i++) // 攻撃の種類によって炎のスピードを変える
									{
										smallFire[i].speed = multipleFireSpeed;
									}
								} else if (attackTypeFirst == 5)
								{
									smallFire[0].speed = multipleFireSpeed;
								}
							}
						}

						if (boss.isAttacking)
						{
							switch (attackTypeFirst)
							{
							case MOVE: BossMove(&boss, boss1FrameImageWidth);
								break;
							case SLOWFIRE:
								SlowFire(kMaxSlowFire, smallFire, &boss, fireShootCount, fireDisappearCount);
								break;
							case FASTFIRE: FastFire(kMaxFastFire, smallFire, &boss, player, fireShootCount, fireDisappearCount);
								break;
							case MULTIPLEFIRE: MultipleFire(kMaxMultiple, smallFire, &boss, fireShootCount, fireDisappearCount);
								break;
							case GIANTFIRE: GiantFire(&giantFire, &explosion, &boss, player, fireDisappearCount);
								break;
							case FLY:
								if (!boss.isHovering)
								{
									if (boss.isInScreen)
									{
										if (!boss.isFlying)
										{
											boss.isFlying = true;
										}

										if (boss.pos.y < 620.0f + boss1FrameImageWidth)
										{
											boss.pos.y += boss.speed;
										}

										if (boss.pos.y >= 620.0f + boss1FrameImageWidth)
										{
											boss.isInScreen = false;
										}
									}

									if (!boss.isInScreen)
									{
										if (boss.direction == LEFT)
										{
											if (boss.pos.x > 500)
											{
												boss.pos.x -= boss.pos.x;
											}

											if (boss.pos.x <= 500)
											{
												boss.pos.x = 500;
												boss.isHovering = true;
												boss.isFlying = false;
											}
										} else if (boss.direction == RIGHT)
										{
											if (boss.pos.x < 500)
											{
												boss.pos.x += boss.pos.x;
											}

											if (boss.pos.x >= 500)
											{
												boss.pos.x = 500;
												boss.isHovering = true;
												boss.isFlying = false;
											}
										}
									}
								}

								if (boss.isHovering)
								{
									if (!boss.isInScreen)
									{
										if (boss.pos.y > 600.0f)
										{
											boss.pos.y -= boss.speed;
										}

										if (boss.pos.y <= 600.0f)
										{
											boss.pos.y = 600.0f;
											boss.isInScreen = true;
										}
									}

									if (boss.isInScreen)
									{
										if (fireDisappearCount < 1)
										{
											if (!smallFire[0].isShot)
											{
												smallFire[0].pos.x = boss.pos.x + 128.0f;
												smallFire[0].pos.y = boss.pos.y - 96.0f;
												smallFire[0].isShot = true;
											}
										}

										if (smallFire[0].isShot)
										{
											smallFire[0].pos.y -= dropSpeed;

											if (smallFire[0].pos.y <= 0.0f + smallFire[0].width)
											{
												smallFire[0].isShot = false;
												fireDisappearCount = 1;
											}
										}

										if (fireDisappearCount == 1 && !flash.isShot)
										{
											flash.isShot = true;
											flash.duration = 90;
											flash.pos.x = smallFire[0].pos.x - 48.0f;
											flash.pos.y = smallFire[0].pos.y + 96.0f;
										}

										if (flash.isShot)
										{
											if (flash.width <= 1280.0f)
											{
												flash.width += 64.0f;
												flash.pos.x -= 32.0f;
											}

											if (flash.height <= 1280.0f)
											{
												flash.height += 64.0f;
												flash.pos.y += 32.0f;
											}

											if (flash.duration > 0)
											{
												flash.duration--;
											}

											if (flash.duration == 0)
											{
												flash.isShot = false;
												boss.isChange = true;
												boss.form = 1;
												boss.height = 200.0f;
												boss.fallTimer = 60;
												boss.isAttacking = false;
											}
										}
									}
								}
							}
						}
					}

					if (boss.form == 1)
					{
						if (!boss.isfalling)
						{
							if (boss.fallTimer > 0)
							{
								boss.fallTimer--;
							}

							if (boss.fallTimer <= 0)
							{
								boss.fallTimer = 0;
								boss.isfalling = true;
							}
						}

						if (boss.isfalling)
						{
							if (boss.pos.y > 0 - 120.0f)
							{
								boss.pos.y -= boss.fallSpeed;
							}

							if (boss.pos.y <= 0.0f - 120.0f)
							{
								boss.pos.y = -120.0f;
								boss.isfalling = false;
								boss.isAttacking = false;
								phase = 1;
							}
						}
					}
				}
			}

#pragma endregion
			
#pragma region アニメーション

			//アニメーション

			if (frameCount >= 59)
			{
				frameCount = 0;
				bossAnimeCount = 0;
				bossSkyAnimeCount = 0; 
				bossFlyAnimeCount = 0;
				bossFlyAnimeCount = 0;
				explosionAnimeCount = 0;

				//プレイヤー
				ghPlayerFrameCount = 0; //待機
				ghPlayerWalkFrameCount = 0; //移動
				ghPlayerJumpFrameCount = 0; //ジャンプ
				ghPlayerAtkSFrameCount = 0; //弱攻撃
				ghPlayerAtkLFrameCount = 0; //強攻撃
				ghPlayerJumpAtkLFrameCount = 0; //ジャンプ強攻撃
			}
			frameCount++;

			if (frameCount % (59 / 8) == 0)
			{
				bossAnimeCount++;
			}

			if (boss.isFlying || boss.form == 1) 
			{ 
				if (frameCount % (59 / 7) == 0)
				{ 
					bossFlyAnimeCount++; 
				} 
			} 
			if (attackTypeFirst == 5)
			{
				if (boss.isHovering) 
				{
					if (frameCount % (59 / 9) == 0)
					{ 
						bossSkyAnimeCount++; 
					} 
				} 
			}
			if (explosion.isShot)
			{
				if (frameCount % (59 / 4) == 0)
				{
					explosionAnimeCount++;
					
				}
			}

			if (!player.isWalk && !player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{ //止まっている時
				if (frameCount % (59 / 5) == 0)
				{
					ghPlayerFrameCount++;
				}
			} else if (player.isWalk && !player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{
				//移動してる時
				if (frameCount % (59 / 3) == 0)
				{
					ghPlayerWalkFrameCount++;
				}
			}
			else if (!player.isWalk && player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{ //ジャンプしてる時
				if (frameCount % (60 / 4) == 0)
				{
					ghPlayerJumpFrameCount++;
				}
			} else if (player.isWalk && player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{ 
				//移動中にジャンプしてる時
				if (frameCount % (59 / 4) == 0)
				{
					ghPlayerJumpFrameCount++;
				}
			}
			else if (!player.isWalk && !player.isJump && shortSword.isReaction && !longSword.isReaction)
			{ //弱攻撃をしてる時
				if (frameCount % (30 / 1) == 0)
				{
					ghPlayerAtkSFrameCount++;
				}
			} else if (player.isWalk && !player.isJump && shortSword.isReaction && !longSword.isReaction)
			{ //移動中に弱攻撃をしてる時
				if (frameCount % (30 / 1) == 0)
				{
					ghPlayerAtkSFrameCount++;
				}
			} else if (!player.isWalk && !player.isJump && !shortSword.isReaction && longSword.isReaction)
			{
				//強攻撃をしてる時
				if (frameCount % (29 / 4) == 0)
				{
					ghPlayerAtkLFrameCount++;
				}
			}
			else if (player.isWalk && !player.isJump && !shortSword.isReaction && longSword.isReaction)
			{
				//移動中に強攻撃をしてる時
				if (frameCount % (29 / 4) == 0)
				{
					ghPlayerAtkLFrameCount++;
				}
			}
			else if (!player.isWalk && player.isJump && shortSword.isReaction && !longSword.isReaction)
			{ 
				//ジャンプ弱攻撃をしてる時
				if (frameCount % (29 / 1) == 0)
				{
					ghPlayerAtkSFrameCount++;
				}
			} 
			else if (player.isWalk && player.isJump && shortSword.isReaction && !longSword.isReaction)
			{ 
				//移動中にジャンプ弱攻撃をしてる時
				if (frameCount % (29 / 1) == 0)
				{
					ghPlayerAtkSFrameCount++;
				}
			} 
			else if (!player.isWalk && player.isJump && !shortSword.isReaction && longSword.isReaction)
			{ 
				//ジャンプ強攻撃をしてる時
				if (frameCount % (59 / 4) == 0)
				{
					ghPlayerJumpAtkLFrameCount++;
				}
			} 
			else if (player.isWalk && player.isJump && !shortSword.isReaction && longSword.isReaction)
			{ 
				//移動中にジャンプ強攻撃をしてる時
				if (frameCount % (59 / 4) == 0)
				{
					ghPlayerJumpAtkLFrameCount++;
				}
			}

#pragma endregion

#pragma region

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

				//ヒットエフェクト
				for (int i = 0; i < hitEffectMax; i++)
				{
					hitEffect[i].isDisplay = true;
					hitEffect[i].speed = 2.0f;
				}

				shortSword.isBossHit = false;
			}

			//大剣の攻撃がボスに当たっている時
			if (longSword.isBossHit) {
				longSword.isAtk = false;
				longSword.durationTime = 30;
				boss.hpCount -= longSword.damage; //ボスのHPを攻撃力分減らす
				
				//ヒットエフェクト
				for (int i = 0; i < hitEffectMax; i++)
				{
					hitEffect[i].isDisplay = true;
					hitEffect[i].speed = 5.0f;
				}

				longSword.isBossHit = false;
			}

			//----------------攻撃を反射するときの当たり判定----------------//
			
			//形態変化技以外の時
			if (attackTypeFirst != 5) 
		    {
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


					//軌道修正
					if (attackTypeFirst == SLOWFIRE)
					{
						if (!smallFire[i].isShot)
						{
							if (!smallFire[i].isShot)
							{
								if (smallFire[i].speed <= 0.0f || smallFire[i].speed >= 6.0f)
								{
									smallFire[i].speed = 5.0f;
								}
							}
						}
					} 
					else if (attackTypeFirst == FASTFIRE)
					{
						if (!smallFire[i].isShot)
						{
							if (smallFire[i].speed <= 0.0f || smallFire[i].speed >= 21.0f)
							{
								if (smallFire[i].speed <= 0.0f || smallFire[i].speed >= 21.0f)
								{
									smallFire[i].speed = 20.0f;
								}
							}
						}
					}
					else if (attackTypeFirst == MULTIPLEFIRE)
					{
						if (!smallFire[i].isShot)
						{
							if (smallFire[i].speed <= 0.0f || smallFire[i].speed >= 13.0f)
							{
								smallFire[i].speed = 12.0f;
							}
						}
					}
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
					smallFire[i].isShot = false;
					smallFire[i].isBossHit = false;
					smallFire[i].isReflection = false;

					//小炎のヒットエフェクト
					for (int j = 0; j < hitFireEffectMax; j++)
					{
						hitFireEffect[j].isDisplay = true;
						hitFireEffect[j].speed = 5.0f;
					}

					fireDisappearCount++;
				}

			}

			// 無敵時間
			if (player.noDamageTime > 0)
			{
				player.noDamageTime--;

				if (player.noDamageTime % 2 == 1) {
					if (player.color == 0xFFFFFFFF)
					{
						player.color = 0xFFFFFF55;
					} 
					else
					{
						player.color = 0xFFFFFFFF;
					}
				}
			}
			else
			{
				player.noDamageTime = 0;
				player.isNoDamage = false;
				player.color = 0xFFFFFFFF;
			}

			//------------------小炎とプレイヤーの当たり判定----------------//

			// 小炎全体の当たり判定
			for (int i = 0; i < kMaxSmallFire; i++)
			{
				if (smallFire[i].isShot)
				{
					if (!player.isNoDamage)
					{
						IsHit(player.pos, player.width, player.height, smallFire[i].pos, smallFire[i].width, smallFire[i].height, smallFire[i].isPlayerHit);
					}
				}
			}

			// 巨大火球の当たり判定
			if (giantFire.isShot)
			{
				if (!player.isNoDamage)
				{
					IsHit(player.pos, player.width, player.height, giantFire.pos, giantFire.width, giantFire.height, giantFire.isPlayerHit);
				}
			}

			if (explosion.isShot)
			{
				if (!player.isNoDamage)
				{
					IsHit(player.pos, player.width, player.height, explosion.pos, explosion.width, explosion.height, explosion.isPlayerHit);
				}
			}

			if (attackTypeFirst == SLOWFIRE)
			{
				// 小炎(連続)の当たり判定
				for (int i = 0; i < kMaxSlowFire; i++)
				{
					if (smallFire[i].isPlayerHit)
					{
						player.hpCount -= 1;
						player.isNoDamage = true;
						player.noDamageTime = 30;
						smallFire[i].isShot = false;
						smallFire[i].isPlayerHit = false;
						fireDisappearCount++;
					}
				}
			}

			if (attackTypeFirst == FASTFIRE)
			{
				// 小炎(高速)の当たり判定
				for (int i = 0; i < kMaxFastFire; i++)
				{
					if (smallFire[i].isPlayerHit)
					{
						player.hpCount -= 1;
						player.isNoDamage = true;
						player.noDamageTime = 30;
						smallFire[i].isShot = false;
						smallFire[i].isPlayerHit = false;
						fireDisappearCount++;
					}
				}
			}

			if (attackTypeFirst == MULTIPLEFIRE)
			{
				// 小炎(拡散)の当たり判定
				for (int i = 0; i < kMaxMultiple; i++)
				{
					if (smallFire[i].isPlayerHit)
					{
						player.hpCount -= 1;
						player.isNoDamage = true;
						player.noDamageTime = 30;
						smallFire[i].isShot = false;
						smallFire[i].isPlayerHit = false;
						fireDisappearCount++;
					}
				}
			}

			if (giantFire.isPlayerHit)
			{
				player.hpCount -= 5;
				player.isNoDamage = true;
				player.noDamageTime = 30;
				giantFire.isShot = false;
				giantFire.isPlayerHit = false;
				fireDisappearCount++;
			}

			if (explosion.isPlayerHit)
			{
				player.hpCount -= 5;
				player.isNoDamage = true;
				player.noDamageTime = 30;
				explosion.isPlayerHit = false;
			}
		}
		IsHit(player.pos, player.width, player.height, boss.pos, boss.width, boss.height, boss.isPlayerHit); 
		if (boss.isPlayerHit)
		{ 
			if (!boss.changedDirection)
			{ 
				if (player.pos.x > boss.pos.x) 
				{ 
					player.pos.x = boss.pos.x + boss.width;
					boss.isPlayerHit = false; 
				} 
				else if (player.pos.x < boss.pos.x)
				{
					player.pos.x = boss.pos.x - player.width;
					boss.isPlayerHit = false;
				}
			} 
			else if (boss.changedDirection) 
			{
				if (!player.isFlying) 
				{
					if (boss.direction == RIGHT)
					{ 
						player.flySpeed = 32.0f;
					}
					else if (boss.direction == LEFT)
					{ 
						player.flySpeed = -32.0f;
					}
					player.hpCount -= 2;
					player.isNoDamage = true; 
					player.noDamageTime = 60;
					boss.isPlayerHit = false;
					player.isFlying = true;
				}
			} 
		} 
		if (player.isFlying)
		{
			if (boss.direction == RIGHT) 
			{
				if (player.flyDistance < 512.0f) 
				{ 
					player.pos.x += player.flySpeed;
					player.flyDistance += player.flySpeed;
				}
				else
				{ player.flySpeed = 0.0f;
				player.isFlying = false;
				player.flyDistance = 0.0f;
				} 
			} 
			else if (boss.direction == LEFT)
			{
		
		   if (player.flyDistance > -512.0f) { player.pos.x += player.flySpeed; player.flyDistance += player.flySpeed; } else { player.flySpeed = 0.0f; player.isFlying = false; player.flyDistance = 0.0f; } }
		} 
#pragma endregion

#pragma region パーティクル

		//==============================================================
		//パーティクル
		//==============================================================

		//--------------------プレイヤーの軌跡---------------------//
		for (int i = 0; i < playerLocusMax; i++)
		{

			if (playerLocus[i].isDisplay)
			{
				//段々小さくなる
				if (playerLocus[i].width >= 0.0f && playerLocus[i].height >= 0.0f) {
					playerLocus[i].width -= 0.5f;
					playerLocus[i].height -= 0.5f;
				}
				else
				{
					playerLocus[i].width = 16.0f;
					playerLocus[i].height = 16.0f;
					playerLocus[i].isDisplay = false;
				}

				//回転させる
				playerLocus[i].rotation += 0.02f;

				//移動させる
				if (!player.isDirections) 
				{
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

			if (!playerLocus[i].isDisplay)
			{
				//ランダムな位置に表示させる
				playerLocus[i].pos.x = rand() % 32 - 16 + player.pos.x + player.width / 2.0f;
				playerLocus[i].pos.y = rand() % 32 - 16 + ToScreen(player.pos.y) + player.height / 2.0f;
			}
		}

		//--------------------小炎の軌跡---------------------//
		for (int i = 0; i < kMaxSmallFire; i++)
		{
			for (int j = 0; j < smallFireLocusMax; j++)
			{
				if (smallFireLocus[i][j].isDisplay)
				{
					//段々小さくなる
					if (smallFireLocus[i][j].width >= 0.0f && smallFireLocus[i][j].height >= 0.0f) {
						smallFireLocus[i][j].width -= rand() % 5;
						smallFireLocus[i][j].height -= rand() % 5;
					}
					else
					{
						smallFireLocus[i][j].width = 32.0f;
						smallFireLocus[i][j].height = 32.0f;
						smallFireLocus[i][j].isDisplay = false;
					}

					//回転させる
					smallFireLocus[i][j].rotation += 0.02f;

					//移動させる
					if (player.pos.x <= smallFireLocus[i][j].pos.x) {
						//右
						playerLocus[i].pos.x -= 0.1f;
						playerLocus[i].pos.y += 0.01f;
					} 
					else
					{
						//左
						playerLocus[i].pos.x += 0.1f;
						playerLocus[i].pos.y += 0.01f;
					}
				}

				if (!smallFireLocus[i][j].isDisplay)
				{
					if (smallFire[i].isShot)
					{
						//ランダムな位置に表示させる
						smallFireLocus[i][j].pos.x = rand() % 16 - 8 + smallFire[i].pos.x + smallFire[i].width / 2.0f;
						smallFireLocus[i][j].pos.y = rand() % 16 - 8 + ToScreen(smallFire[i].pos.y) + smallFire[i].height / 2.0f;
					}
				}

				//クールタイム
				if (smallFire[i].isShot)
				{
					if (smallFireLocusCoolTime >= 0)
					{
						smallFireLocusCoolTime--;
					}
					else
					{
						smallFireLocus[i][j].isDisplay = true;
						smallFireLocusCoolTime = 240;
					}
				}

				attackTypeFirst = 5;

				//小さくなるにつれて色変化
				if (attackTypeFirst != 5) 
				{
					if (smallFireLocus[i][j].width <= 20.0f)
					{
						if (smallFireLocus[i][j].color <= 0xFF8800FF)
						{
							smallFireLocus[i][j].color += 0x00110000;
						}
					}
				}
				else //形態変化技の時
				{
					if (static_cast<int>(smallFireLocus[i][j].width) % 2 == 0) {
						if (smallFireLocus[i][j].color == 0xFFFFFFFF)
						{
							smallFireLocus[i][j].color = 0x0000FFFF;
						} 
						else
						{
							smallFireLocus[i][j].color = 0xFFFFFFFF;
						}
					}
				}

				//反射すると色が変わる
				if (smallFire[i].isReflection)
				{
					smallFireLocus[i][j].color = 0x0000FFFF;
				}

				//弾が消えるときにエフェクトも消える
				if (!smallFire[i].isShot)
				{
					smallFireLocus[i][j].isDisplay = false;
					smallFireLocus[i][j].color = 0xFF0000FF;
				}
			}
		}

		//--------------------巨大火球の軌跡---------------------//
		for (int i = 0; i < giantFireLocusMax; i++)
		{
			if (giantFireLocus[i].isDisplay)
			{
				//段々小さくなる
				if (giantFireLocus[i].width >= 0.0f && giantFireLocus[i].height >= 0.0f) 
				{
					giantFireLocus[i].width -= rand() % 10;
					giantFireLocus[i].height -= rand() % 10;
				}
				else
				{
					giantFireLocus[i].width = 128.0f;
					giantFireLocus[i].height = 128.0f;
					giantFireLocus[i].isDisplay = false;
				}

				//回転させる
				giantFireLocus[i].rotation += 0.02f;

				//移動させる
				if (player.pos.x <= giantFireLocus[i].pos.x)
				{
					//右
					playerLocus[i].pos.x -= 0.1f;
					playerLocus[i].pos.y += 0.1f;
				} 
				else
				{
					//左
					playerLocus[i].pos.x += 0.1f;
					playerLocus[i].pos.y += 0.01f;
				}
			}

			if (!giantFireLocus[i].isDisplay)
			{
				if (giantFire.isShot) 
				{
					//ランダムな位置に表示させる
					giantFireLocus[i].pos.x = rand() % 64 - 32 + giantFire.pos.x + giantFire.width / 2.0f;
					giantFireLocus[i].pos.y = rand() % 64 - 32 + ToScreen(giantFire.pos.y) + giantFire.height / 2.0f;
				}
			}

			//クールタイム
			if (giantFire.isShot)
			{
				if (giantFireLocusCoolTime >= 0)
				{
					giantFireLocusCoolTime--;
				}
				else
				{
					giantFireLocus[i].isDisplay = true;
					giantFireLocusCoolTime = 240;
				}
			}

			//小さくなるにつれて色変化
			if (giantFireLocus[i].width <= 96.0f)
			{
				if (giantFireLocus[i].color <= 0xFF8800FF)
				{
					giantFireLocus[i].color += 0x00110000;
				}
			}

			//弾が消えるときにエフェクトも消える
			if (!giantFire.isShot)
			{
				giantFireLocus[i].isDisplay = false;
				giantFireLocus[i].color = 0xFF0000FF;
			}
		}

		//--------------------地面から出るオーラ---------------------//
		for (int i = 0; i < powderAuraMax; i++) 
		{

			if (powderAura[i].isDisplay) 
			{
				//段々小さくなる
				if (powderAura[i].width >= 0.0f && powderAura[i].height >= 0.0f) {
					powderAura[i].width -= 0.05f;
					powderAura[i].height -= 0.05f;
				}
				else
				{
					powderAura[i].width = 10.0f;
					powderAura[i].height = 10.0f;
					powderAura[i].isDisplay = false;
				}

				//回転させる
				powderAura[i].rotation += 0.02f;

				//移動させる

				powderAura[i].pos.y -= 0.7f;

				if (powderAura[i].pos.y <= 0.0f)
				{
					powderAura[i].isDisplay = false;
				}

			}

			if (!powderAura[i].isDisplay)
			{
				//ランダムな位置に表示させる
				powderAura[i].pos.x = static_cast<float>(rand() % 1280);
				powderAura[i].pos.y = 600.0f;
			}

			if (powderAuraCoolTime >= 0) {
				powderAuraCoolTime--;
			} 
			else
			{
				powderAura[i].isDisplay = true;
				powderAuraCoolTime = 240;
			}

		}
		
		//---------------------弱、強攻撃のヒットエフェクト--------------------//
		for (int i = 0; i < hitEffectMax; i++)
		{

			if (hitEffect[i].isDisplay)
			{
				//段々小さくなる
				if (hitEffect[i].width >= 0.0f) {
					hitEffect[i].width -= 1.0f;
				} 
				else
				{
					if (i % 2 == 1) 
					{
						hitEffect[i].width = 5.0f;
						hitEffect[i].height = static_cast<float>(rand() % 128);
						hitEffect[i].isDisplay = false;
						hitEffect[i].circumference = 0.0f;
					}
					else
					{
						if (hitEffect[i].width >= -5.0f) {
							hitEffect[i].width -= 0.7f;
							hitEffect[i].height -= 5.0f;
						} 
						else
						{
							hitEffect[i].width = 5.0f;
							hitEffect[i].height = static_cast<float>(rand() % 128);
							hitEffect[i].isDisplay = false;
							hitEffect[i].circumference = 0.0f;
						}
					}
				}

				//回転させる
				hitEffect[i].rotation = effectTheta * i;

				//移動させる
				hitEffect[i].circumference += hitEffect[i].speed;

				hitEffect[i].pos.x = shortSword.pos.x + 50.0f - cosf(effectTheta * i) * hitEffect[i].circumference; //ｘ座標
				hitEffect[i].pos.y = shortSword.pos.y - 20.0f - sinf(effectTheta * i) * hitEffect[i].circumference; //ｙ座標
			}

			effectTheta = 1.0f / 32.0f * float(M_PI);

			if (!hitEffect[i].isDisplay)
			{
				//円状にに表示させる
				hitEffect[i].pos.x = shortSword.pos.x + 50.0f - cosf(effectTheta * i) * hitEffect[i].circumference; //ｘ座標
				hitEffect[i].pos.y = shortSword.pos.y - 20.0f - sinf(effectTheta * i) * hitEffect[i].circumference; //ｙ座標
				hitEffect[i].height = static_cast<float>(rand() % 64);
			}
		}

		//--------------------小炎のヒットエフェクト---------------------//
		for (int i = 0; i < hitFireEffectMax; i++)
		{
			if (hitFireEffect[i].isDisplay)
			{
				//段々小さくなる
				if (hitFireEffect[i].width >= 0.0f && hitFireEffect[i].height >= 0.0f) 
				{
					hitFireEffect[i].width -= rand() % 5;
					hitFireEffect[i].height -= rand() % 5;
				} 
				else
				{
					hitFireEffect[i].width = 64.0f;
					hitFireEffect[i].height = 64.0f;
					hitFireEffect[i].isDisplay = false;
					hitFireEffect[i].circumference = 0.0f;
				}

				//移動させる
				hitFireEffect[i].circumference += rand() % 10;
				
				hitFireEffect[i].pos.x = boss.pos.x + 150.0f - cosf(effectTheta * i) * hitFireEffect[i].circumference; //ｘ座標
				hitFireEffect[i].pos.y = boss.pos.y + 150.0f - sinf(effectTheta * i) * hitFireEffect[i].circumference; //ｙ座標
			}

			//小さくなるにつれて色変化
			if (hitFireEffect[i].width <= 30.0f)
			{
				if (hitFireEffect[i].color <= 0x000000FF)
				{
					hitFireEffect[i].color -= 0x33000011;
				}
			}

			if (!hitFireEffect[i].isDisplay)
			{
				//円状にに表示させる
				hitFireEffect[i].pos.x = boss.pos.x + 150.0f - cosf(effectTheta * i) * hitFireEffect[i].circumference; //ｘ座標
				hitFireEffect[i].pos.y = boss.pos.y + 150.0f - sinf(effectTheta * i) * hitFireEffect[i].circumference; //ｙ座標

				hitFireEffect[i].color = 0xFF0000FF;
			}
		}

#pragma endregion

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		if (scene == GAMEPLAY)
		{

			//背景
			Novice::DrawSprite
			(
				static_cast<int>(backGround.pos.x),
				static_cast<int>(backGround.pos.y),
				ghBackGround1, 1, 1, 0.0f, backGround.color
			);

			//---------------------パーティクル-----------------------//

			//地面から出るオーラ
			for (int i = 0; i < powderAuraMax; i++)
			{
				if (powderAura[i].isDisplay)
				{
					Novice::DrawBox(
						static_cast<int>(powderAura[i].pos.x),
						static_cast<int>(powderAura[i].pos.y),
						static_cast<int>(powderAura[i].width),
						static_cast<int>(powderAura[i].height),
						powderAura[i].rotation, powderAura[i].color, kFillModeSolid);
				}
			}

			//プレイヤーの軌跡
			for (int i = 0; i < playerLocusMax; i++)
			{
				if (playerLocus[i].isDisplay)
				{
					Novice::DrawBox
					(
						static_cast<int>(playerLocus[i].pos.x),
						static_cast<int>(playerLocus[i].pos.y),
						static_cast<int>(playerLocus[i].width),
						static_cast<int>(playerLocus[i].height),
						playerLocus[i].rotation, playerLocus[i].color, kFillModeWireFrame
					);
				}
			}


			//小炎の軌跡
			for (int i = 0; i < kMaxSmallFire; i++)
			{
				for (int j = 0; j < smallFireLocusMax; j++)
				{
					if (smallFireLocus[i][j].isDisplay)
					{
						Novice::DrawEllipse
						(
							static_cast<int>(smallFireLocus[i][j].pos.x),
							static_cast<int>(smallFireLocus[i][j].pos.y),
							static_cast<int>(smallFireLocus[i][j].width / 2.0f),
							static_cast<int>(smallFireLocus[i][j].height / 2.0f),
							smallFireLocus[i][j].rotation, smallFireLocus[i][j].color, kFillModeSolid
						);
					}
				}
			}

			//巨大火球の軌跡
			for (int i = 0; i < giantFireLocusMax; i++)
			{
				if (giantFireLocus[i].isDisplay)
				{
					Novice::DrawEllipse
					(
						static_cast<int>(giantFireLocus[i].pos.x),
						static_cast<int>(giantFireLocus[i].pos.y),
						static_cast<int>(giantFireLocus[i].width / 2.0f),
						static_cast<int>(giantFireLocus[i].height / 2.0f),
						giantFireLocus[i].rotation, giantFireLocus[i].color, kFillModeSolid
					);
				}
			}

			//地面 
			Novice::DrawLine(0, 620, 1280, 620, RED);

			if (boss.form == 0)
			{
				if (!boss.isFlying && !boss.isHovering)
				{
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
					} else if (boss.direction == RIGHT)
					{
						Novice::DrawSpriteRect
						(
							static_cast<int>(boss.pos.x - (boss1FrameImageWidth / 2.0f - boss.width / 2.0f)),
							static_cast<int>(ToScreen(boss.pos.y + (boss1ImageHeight - boss.height))),
							640 * bossAnimeCount,
							0,
							static_cast<int>(boss1FrameImageWidth),
							static_cast<int>(boss1ImageHeight),
							ghBoss1Right,
							boss1FrameImageWidth / boss1MaxImageWidth, 1,
							0, 0xFFFFFFFF
						);
					}
				}

				if (boss.isFlying)
				{
					if (boss.direction == LEFT)
					{
						Novice::DrawSpriteRect
						(
							static_cast<int>(boss.pos.x - (boss1FlyFrameWidth / 2.0f - boss.width / 2.0f)),
							static_cast<int>(ToScreen(boss.pos.y + (boss1FlyImageHeight - boss.height))),
							static_cast<int>(boss1FlyFrameWidth) * bossFlyAnimeCount,
							0,
							static_cast<int>(boss1FlyFrameWidth),
							static_cast<int>(boss1FlyImageHeight),
							ghBoss1FlyLeft,
							boss1FlyFrameWidth / boss1FlyMaxWidth, 1,
							0, 0xFFFFFFFF
						);
					} else if (boss.direction == RIGHT)
					{
						Novice::DrawSpriteRect
						(
							static_cast<int>(boss.pos.x - (boss1FlyFrameWidth / 2.0f - boss.width / 2.0f)),
							static_cast<int>(ToScreen(boss.pos.y + (boss1FlyImageHeight - boss.height))),
							static_cast<int>(boss1FlyFrameWidth) * bossFlyAnimeCount,
							0,
							static_cast<int>(boss1FlyFrameWidth),
							static_cast<int>(boss1FlyImageHeight),
							ghBoss1FlyRight,
							boss1FlyFrameWidth / boss1FlyMaxWidth, 1,
							0, 0xFFFFFFFF
						);
					}
				}

				if (attackTypeFirst == 5)
				{
					if (boss.isHovering)
					{
						Novice::DrawSpriteRect
						(
							static_cast<int>(boss.pos.x - (boss1SkyFrameWidth / 2.0f - boss.width / 2.0f)),
							static_cast<int>(ToScreen(boss.pos.y + (boss1SkyImageHeight - boss.height))),
							static_cast<int>(boss1SkyFrameWidth) * bossSkyAnimeCount,
							0,
							static_cast<int>(boss1SkyFrameWidth),
							static_cast<int>(boss1SkyImageHeight),
							ghBoss1Sky,
							boss1SkyFrameWidth / boss1MaxImageWidth, 1,
							0, 0xFFFFFFFF
						);
					}
				}
			} 
			else if (boss.form == 1)
			{
				if (!boss.isfalling)
				{
					Novice::DrawSpriteRect
					(
						static_cast<int>(boss.pos.x - (boss2TempFrameWidth / 2.0f - boss.width / 2.0f)),
						static_cast<int>(ToScreen(boss.pos.y + (boss2TempImageHeight - boss.height))),
						static_cast<int>(boss2TempFrameWidth) * bossFlyAnimeCount,
						0,
						static_cast<int>(boss2TempFrameWidth),
						static_cast<int>(boss2TempImageHeight),
						ghBoss2Temp,
						boss2TempFrameWidth / boss2TempMaxImageWidth, 1,
						0, 0xFFFFFFFF
					);
				} 
				else
				{
					Novice::DrawSprite
					(
						static_cast<int>(boss.pos.x - (boss2FallWidth / 2.0f - boss.width / 2.0f)),
						static_cast<int>(ToScreen(boss.pos.y + (boss2FallHeight - boss.height))),
						ghBoss2Fall,
						1, 1,
						0.0f, 0xFFFFFFFF
					);
				}
			}

			if (!player.isWalk && !player.isJump && !shortSword.isReaction && !longSword.isReaction) //止まっている時のみ
			{
				//止まっている時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerFrameCount,
						0,
						static_cast<int>(ghPlayerWidth),
						static_cast<int>(ghPlayerHeight),
						ghPlayer[0],
						ghPlayerWidth / ghPlayerMaxWidth, 1,
						0, player.color
					);
				} else if (player.isDirections)
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerFrameCount,
						0,
						static_cast<int>(ghPlayerWidth),
						static_cast<int>(ghPlayerHeight),
						ghPlayer[1],
						ghPlayerWidth / ghPlayerMaxWidth, 1,
						0, player.color
					);
				}
			} else if (player.isWalk && !player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{
				//移動している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerWalkFrameCount,
						0,
						static_cast<int>(ghPlayerWalkWidth),
						static_cast<int>(ghPlayerWalkHeight),
						ghPlayerWalk[0],
						ghPlayerWalkWidth / ghPlayerWalkMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerWalkFrameCount,
						0,
						static_cast<int>(ghPlayerWalkWidth),
						static_cast<int>(ghPlayerWalkHeight),
						ghPlayerWalk[1],
						ghPlayerWalkWidth / ghPlayerWalkMaxWidth, 1,
						0, player.color
					);
				}
			} else if (!player.isWalk && player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{
				//ジャンプしている時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerJumpFrameCount,
						0,
						static_cast<int>(ghPlayerJumpWidth),
						static_cast<int>(ghPlayerJumpHeight),
						ghPlayerJump[0],
						ghPlayerJumpWidth / ghPlayerJumpMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerJumpFrameCount,
						0,
						static_cast<int>(ghPlayerJumpWidth),
						static_cast<int>(ghPlayerJumpHeight),
						ghPlayerJump[1],
						ghPlayerJumpWidth / ghPlayerJumpMaxWidth, 1,
						0, player.color
					);
				}
			} else if (player.isWalk && player.isJump && !shortSword.isReaction && !longSword.isReaction)
			{
				//移動中にジャンプしている時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerJumpFrameCount,
						0,
						static_cast<int>(ghPlayerJumpWidth),
						static_cast<int>(ghPlayerJumpHeight),
						ghPlayerJump[0],
						ghPlayerJumpWidth / ghPlayerJumpMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerJumpFrameCount,
						0,
						static_cast<int>(ghPlayerJumpWidth),
						static_cast<int>(ghPlayerJumpHeight),
						ghPlayerJump[1],
						ghPlayerJumpWidth / ghPlayerJumpMaxWidth, 1,
						0, player.color
					);
				}
			} else if (!player.isWalk && !player.isJump && shortSword.isReaction && !longSword.isReaction)
			{
				//弱攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[0],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[1],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				}
			} else if (player.isWalk && !player.isJump && shortSword.isReaction && !longSword.isReaction)
			{
				//移動中に弱攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[0],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[1],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				}
			} else if (!player.isWalk && !player.isJump && !shortSword.isReaction && longSword.isReaction)
			{
				//強攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y) - 32.0f),
						160 * ghPlayerAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerAtkLWidth),
						static_cast<int>(ghPlayerAtkLHeight),
						ghPlayerAtkL[0],
						ghPlayerAtkLWidth / ghPlayerAtkLMaxWidth, 1,
						0, player.color
					);
				} 
				else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x - 32.0f),
						static_cast<int>(ToScreen(playerDrawing.pos.y) - 32.0f),
						160 * ghPlayerAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerAtkLWidth),
						static_cast<int>(ghPlayerAtkLHeight),
						ghPlayerAtkL[1],
						ghPlayerAtkLWidth / ghPlayerAtkLMaxWidth, 1,
						0, player.color
					);
				}
			} 
			else if (player.isWalk && !player.isJump && !shortSword.isReaction && longSword.isReaction)
			{
				//移動中に強攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y) - 32.0f),
						160 * ghPlayerAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerAtkLWidth),
						static_cast<int>(ghPlayerAtkLHeight),
						ghPlayerAtkL[0],
						ghPlayerAtkLWidth / ghPlayerAtkLMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x - 32.0f),
						static_cast<int>(ToScreen(playerDrawing.pos.y) - 32.0f),
						160 * ghPlayerAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerAtkLWidth),
						static_cast<int>(ghPlayerAtkLHeight),
						ghPlayerAtkL[1],
						ghPlayerAtkLWidth / ghPlayerAtkLMaxWidth, 1,
						0, player.color
					);
				}
			} else if (!player.isWalk && player.isJump && shortSword.isReaction && !longSword.isReaction)
			{
				//ジャンプ弱攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[0],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[1],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				}
			} else if (player.isWalk && player.isJump && shortSword.isReaction && !longSword.isReaction)
			{
				//移動中にジャンプ弱攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[0],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						128 * ghPlayerAtkSFrameCount,
						0,
						static_cast<int>(ghPlayerAtkSWidth),
						static_cast<int>(ghPlayerAtkSHeight),
						ghPlayerAtkS[1],
						ghPlayerAtkSWidth / ghPlayerAtkSMaxWidth, 1,
						0, player.color
					);
				}
			} else if (!player.isWalk && player.isJump && !shortSword.isReaction && longSword.isReaction)
			{
				//ジャンプ強攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						160 * ghPlayerJumpAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerJumpAtkLWidth),
						static_cast<int>(ghPlayerJumpAtkLHeight),
						ghPlayerJumpAtkL[0],
						ghPlayerJumpAtkLWidth / ghPlayerJumpAtkLMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x - 32.0f),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						160 * ghPlayerJumpAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerJumpAtkLWidth),
						static_cast<int>(ghPlayerJumpAtkLHeight),
						ghPlayerJumpAtkL[1],
						ghPlayerJumpAtkLWidth / ghPlayerJumpAtkLMaxWidth, 1,
						0, player.color
					);
				}
			} else if (player.isWalk && player.isJump && !shortSword.isReaction && longSword.isReaction)
			{
				//移動中にジャンプ強攻撃している時のみ
				if (!player.isDirections)
				{
					//プレイヤー右
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						160 * ghPlayerJumpAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerJumpAtkLWidth),
						static_cast<int>(ghPlayerJumpAtkLHeight),
						ghPlayerJumpAtkL[0],
						ghPlayerJumpAtkLWidth / ghPlayerJumpAtkLMaxWidth, 1,
						0, player.color
					);
				} else
				{
					//プレイヤー左
					Novice::DrawSpriteRect
					(
						static_cast<int>(playerDrawing.pos.x - 32.0f),
						static_cast<int>(ToScreen(playerDrawing.pos.y)),
						160 * ghPlayerJumpAtkLFrameCount,
						0,
						static_cast<int>(ghPlayerJumpAtkLWidth),
						static_cast<int>(ghPlayerJumpAtkLHeight),
						ghPlayerJumpAtkL[1],
						ghPlayerJumpAtkLWidth / ghPlayerJumpAtkLMaxWidth, 1,
						0, player.color
					);
				}
			}

			if (shortSword.isReaction) //短剣の判定(持続時)
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

			if (longSword.isReaction) //大剣の判定(持続時)
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
			
			//ヒットエフェクト
			for (int i = 0; i < hitEffectMax; i++)
			{
				if (hitEffect[i].isDisplay)
				{
					if (!player.isDirections)
					{
						Novice::DrawEllipse
						(
							static_cast<int>(hitEffect[i].pos.x + 50.0f),
							static_cast<int>(ToScreen(hitEffect[i].pos.y)),
							static_cast<int>(hitEffect[i].width / 2.0f),
							static_cast<int>(hitEffect[i].height / 2.0f),
							hitEffect[i].rotation, hitEffect[i].color, kFillModeSolid
						);
					} 
					else
					{
						Novice::DrawEllipse
						(
							static_cast<int>(hitEffect[i].pos.x - 50.0f),
							static_cast<int>(ToScreen(hitEffect[i].pos.y)),
							static_cast<int>(hitEffect[i].width / 2.0f),
							static_cast<int>(hitEffect[i].height / 2.0f),
							hitEffect[i].rotation, hitEffect[i].color, kFillModeSolid
						);
					}

				}
			}

			//小炎のヒットエフェクト
			for (int i = 0; i < hitFireEffectMax; i++)
			{
				if (hitFireEffect[i].isDisplay)
				{
					Novice::DrawEllipse
					(
						static_cast<int>(hitFireEffect[i].pos.x),
						static_cast<int>(hitFireEffect[i].pos.y),
						static_cast<int>(hitFireEffect[i].width / 2.0f),
						static_cast<int>(hitFireEffect[i].height / 2.0f),
						hitFireEffect[i].rotation, hitFireEffect[i].color, kFillModeSolid
					);
				}
			}


			Novice::ScreenPrintf(100, 100, "isAttacking: %d", boss.isAttacking);
			Novice::ScreenPrintf(100, 120, "attack coolTimer: %d", boss.attackCoolTimer);
			Novice::ScreenPrintf(100, 140, "attack type: %d", attackTypeFirst);
			Novice::ScreenPrintf(100, 160, "boss hp: %d", boss.hpCount);
			Novice::ScreenPrintf(100, 180, "player hp: %d", player.hpCount);
			Novice::ScreenPrintf(100, 200, "isShot: %d", explosion.isShot);
			Novice::ScreenPrintf(100, 220, "disappearcount: %d", fireDisappearCount);
			Novice::ScreenPrintf(100, 240, "smallFire : %d", smallFire[7].isShot);
			Novice::ScreenPrintf(100, 260, "player.isNodamage : %d", player.isNoDamage);
			Novice::ScreenPrintf(100, 280, "player.noDamageTime : %d", player.noDamageTime);

			Novice::DrawBox(static_cast<int>(boss.pos.x),
				static_cast<int>(ToScreen(boss.pos.y)),
				static_cast<int>(boss.width),
				static_cast<int>(boss.height),
				0.0f, 0xFFFFFFFF, kFillModeWireFrame);

			//当たり判定player
			Novice::DrawBox(static_cast<int>(player.pos.x),
				static_cast<int>(ToScreen(player.pos.y)),
				static_cast<int>(player.width),
				static_cast<int>(player.height),
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
