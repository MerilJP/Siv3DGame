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

	//入力関数
	inline bool PlayerMovement() {

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

		if (Input::KeyUp.pressed && Player.P2.pos.y >= 0) 
		{
			Pos.P2 -= 5;
		}
		else if (Input::KeyDown.pressed && (Player.P2.pos.y + Player.P2.size.y) <= Window::Height()) 
		{
			Pos.P2 += 5;
		}

		if (t >= 3000) 
		{
			return false;
		}
		
		return true;

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

	//リスタート関数
	inline void Restart() {
		const int32 t = Input::KeyR.pressedDuration;

		if (t > 0) 
		{
			FontAsset::Register(L"restart", 30, Typeface::Default);
			FontAsset(L"restart")(L"Ready",String(Min(t / 1000,3),L'.'))
				.drawCenter(Window::Center().x,Window::Center().y - 100,AlphaF(Min(t/ 1000.0,1.0)));
			if (t >= 3000) 
			{
				m_ball.setPos(Window::Center().x,Window::Center().y);
				dSpeed = 5.0;
				m_ballSpeed = Vec2(-dSpeed, 0);
				bGameEnd = false;
			}

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

	//ゲーム終了関数
	inline bool GameOver() {
		if (m_Score.left >= 5 || m_Score.right >= 5)
		{
			return false;
		}
		return true;
	}

	//メイン関数
	inline bool GameUpdate() 
	{
		bool ret = true;

		Player.P1.setCenter(20, Pos.P1);
		Player.P2.setCenter(Window::Width() - 20, Pos.P2);

		m_ball.moveBy(m_ballSpeed);
	
		Collision();

		Restart();
		m_ball.draw();
		Player.P1.draw();
		Player.P2.draw();

		if (!GameOver() || !PlayerMovement()) {
			ret = false;
		}
		return ret;
		
	}

private:

	//得点
	struct Score 
	{
		int left = 0;
		int right = 0;
	};

	//プレイヤー
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
	const String sGameTitle = L"Siv3D Application";
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

	Siv3DGame *GamePlay = new Siv3DGame();
	GameScene gameScene = GameScene::Title;

	while (System::Update())
	{
		switch (gameScene)
		{
#pragma region タイトル
		case GameScene::Title:
			if (Input::KeyEnter.clicked) gameScene = GameScene::Playing;
			if (Input::KeyEscape.clicked) System::Exit();
			break;
#pragma endregion
#pragma region プレイ中
		case GameScene::Playing:
			if (!GamePlay->GameUpdate()) gameScene = GameScene::GameOver;
			break;
#pragma endregion
#pragma region ゲームオーバー
		case GameScene::GameOver:
			if (Input::KeyEnter.clicked) gameScene = GameScene::Title;
			break;
#pragma endregion
		}



#ifdef _DEBUG
		GamePlay->ClientStatus();		
#endif
	
	}
}