#include <Siv3D.hpp>

class Siv3DGame
{
public:
	//コンストラクタ
	Siv3DGame() : Timer(true)
		, m_ball(320, 400, 8)
		, dSpeed(5.0)
		, bGameEnd(false)
	{
		Graphics::SetBackground(Palette::Black);
		Window::SetTitle(sGameTitle);
		Window::Resize(nClientWidth, nClientHeight);
		Window::Centering();
		m_ball.setPos(Window::Center().x, Window::Center().y);
		m_ballSpeed = Vec2(dSpeed, 0);

#ifdef _DEBUG
#else
		System::SetExitEvent(WindowEvent::CloseButton);
#endif
	}

	//デストラクタ
	~Siv3DGame()
	{
	}

	//蔵情報出力関数
	inline void ClientStatus() const throw()
	{
		FontAsset::Register(L"status", 10, Typeface::Default);
		FontAsset(L"status")(L"FPS : ", Profiler::FPS(), L" \nTIMER : ", Timer.ms(), L"ms\n").draw();
	}

	//ゲームタイトルの関数
	inline bool GameTitle() 
	{
		bool ret = true;
		//初期化
		m_ball.setPos(Window::Center().x, Window::Center().y);
		m_ballSpeed = Vec2(dSpeed, 0);

		FontAsset::Register(L"title", 30, Typeface::Default);
		FontAsset(L"title")(L"PONG\n\n").drawCenter(Window::Center().x,150);

		FontAsset(L"title")(L"[Enter]でスタート").drawCenter(Window::Center().x, 300);

		return ret;
	}

	//ゲームプレイ中の関数
	inline bool GamePlay()
	{
		const bool cP1 = Player.P1.intersects(m_ball);
		const bool cP2 = Player.P2.intersects(m_ball);

		bool ret = true;

		Player.P1.setCenter(20, Pos.P1);
		Player.P2.setCenter(Window::Width() - 20, Pos.P2);

		m_ball.moveBy(m_ballSpeed);

		Collision();

		Restart();
		m_ball.draw();
		Player.P1.draw(cP1 ? Palette::Red : Palette::White);
		Player.P2.draw(cP2 ? Palette::Red : Palette::White);

		if (!PlayerMovement() || m_Score.left >= 5 || m_Score.right >= 5) {
			ret = false;
		}
		return ret;

	}

	//ゲームオーバーシーンの関数
	inline void GameOver()
	{
		FontAsset::Register(L"gameover", 30, Typeface::Default);
		FontAsset(L"gameover")(L"左:", m_Score.left, L"		右:", m_Score.right).drawCenter(Window::Center().x, 150);
		FontAsset(L"gameover")(L"[Enter]でタイトルに戻る").drawCenter(Window::Center().x, 300);
		//ボールの位置を戻す
		m_ball.setPos(Window::Center().x, Window::Center().y);
	}

private:
	//入力関数
	inline bool PlayerMovement() {

		bool ret = true;

		const int32 t = Input::KeyEscape.pressedDuration;
		

		//プレイヤー1
		if (Input::KeyW.pressed && Player.P1.pos.y >= 0)
		{
			Pos.P1 -= 5;
		}
		else if (Input::KeyS.pressed && (Player.P1.pos.y + Player.P1.size.y) <= Window::Height())
		{
			Pos.P1 += 5;
		}
		//プレイヤー2		
		if (Input::KeyUp.pressed && Player.P2.pos.y >= 0)
		{
			Pos.P2 -= 5;
		}
		else if (Input::KeyDown.pressed && (Player.P2.pos.y + Player.P2.size.y) <= Window::Height())
		{
			Pos.P2 += 5;
		}
		//ゲーム終了	
		if (t > 0) {
			FontAsset::Register(L"end", 30, Typeface::Default);
			FontAsset(L"end")(L"Exit", String(Min(t / 1000, 3), L'.'))
				.drawCenter(Window::Center().x, Window::Center().y - 100, AlphaF(Min(t / 1000.0, 1.0)));
			if (t >= 3000)
			{
				ret = false;
			}
		}


		return ret;

	}

	//リスタート関数
	inline void Restart() {
		const int32 t = Input::KeyR.pressedDuration;

		if (t > 0)
		{
			FontAsset::Register(L"restart", 30, Typeface::Default);
			FontAsset(L"restart")(L"Ready", String(Min(t / 1000, 3), L'.'))
				.drawCenter(Window::Center().x, Window::Center().y - 100, AlphaF(Min(t / 1000.0, 1.0)));
			if (t >= 3000)
			{
				m_ball.setPos(Window::Center().x, Window::Center().y);
				dSpeed = 5.0;
				m_ballSpeed = Vec2(-dSpeed, 0);
				bGameEnd = false;
			}

		}
	}

	//音声関数
	inline void SoundPlay(int type) const throw()
	{
		SoundAsset::Register(L"wall", L"Example/WallCollision.mp3");
		SoundAsset::Register(L"bar", L"Example/BarCollision.mp3");
		SoundAsset::Register(L"end", L"Example/GameOver.mp3");


		switch (type) {
		case 0:
			SoundAsset(L"wall").playMulti();
			break;
		case 1:
			SoundAsset(L"bar").play();
			break;
		default:
			SoundAsset(L"end").play();
		}
	}

	//衝突関数
	inline void Collision() {

		// バーの判定 : プレイヤー1
		if (Player.P1.intersects(m_ball))
		{
			m_ballSpeed = Vec2(-m_ballSpeed.x, (m_ball.y - Player.P1.center.y) / 8).setLength(dSpeed);
			dSpeed += 0.2;
			SoundPlay(1);
		}

		// バーの判定 : プレイヤー2
		if (Player.P2.intersects(m_ball))
		{
			m_ballSpeed = Vec2(-m_ballSpeed.x, (m_ball.y - Player.P2.center.y) / 8).setLength(dSpeed);
			dSpeed += 0.2;
			SoundPlay(1);
		}

		//上下壁の判定
		if (m_ball.y < 0 && m_ballSpeed.y <  0 || (Window::Height() < m_ball.y && m_ballSpeed.y > 0))
		{
			m_ballSpeed.y *= -1;
			if (!bGameEnd) SoundPlay(0);
		}

		//左の壁
		if (m_ball.x < 0 && m_ballSpeed.x <  0)
		{
			if (!bGameEnd)
			{
				m_Score.right++;
				SoundPlay(3);
			}
			FontAsset::Register(L"score", 10, Typeface::Default);
			FontAsset(L"scre")(L"左:", m_Score.left, L"		右:", m_Score.right,
				L"\nR長押しでリスタート").drawCenter(Window::Center().x, 50);

			bGameEnd = true;
		}
		//右の壁
		else if ((Window::Width() <= m_ball.x && m_ballSpeed.x >= 0))
		{
			if (!bGameEnd)
			{
				m_Score.left++;
				SoundPlay(3);
			}

			FontAsset::Register(L"score", 10, Typeface::Default);
			FontAsset(L"scre")(L"左:", m_Score.left, L"		右:", m_Score.right,
				L"\nR長押しでリスタート").drawCenter(Window::Center().x, 50);

			bGameEnd = true;
		}
	}

	//得点
	struct Score
	{
		int left = 0;
		int right = 0;
	};

	//プレイヤー 座標
	struct Controller
	{
		Rect P1;
		Rect P2;
		Controller() :
			P1(10, 80)
			, P2(10, 80)
		{
		}
	};

	//座標
	struct Coordinate
	{
		int P1 = Window::Height() / 2;
		int P2 = Window::Height() / 2;
	};

	//時間
	const Stopwatch Timer;

	Controller Player;
	Circle m_ball;
	Vec2 m_ballSpeed;
	double dSpeed;
	Score m_Score;
	bool bGameEnd;
	Coordinate Pos;

	//ゲームタイトル
	const String sGameTitle = L"Siv3DGame PONG";
	//クライアントサイズ
	const int nClientWidth = 640;
	const int nClientHeight = 480;

};

enum class GameScene
{
	Title,
	Playing,
	GameOver
};



void Main()
{

	Siv3DGame *GameData = new Siv3DGame();
	GameScene gameScene = GameScene::Title;

	while (System::Update())
	{
		switch (gameScene)
		{
#pragma region タイトル
		case GameScene::Title:
			GameData->GameTitle();
			if (Input::KeyEnter.clicked) gameScene = GameScene::Playing;
			if (Input::KeyEscape.clicked) System::Exit();
			break;
#pragma endregion タイトルシーン
#pragma region プレイ中
		case GameScene::Playing:
			if (!GameData->GamePlay()) gameScene = GameScene::GameOver;
			break;
#pragma endregion ゲームプレイシーン
#pragma region ゲームオーバー
		case GameScene::GameOver:
			GameData->GameOver();	
			if(Input::KeyEnter.clicked) gameScene = GameScene::Title;
			break;
#pragma endregion ゲームオーバーシーン
		}
#ifdef _DEBUG
		GameData->ClientStatus();
#endif

	}
}
