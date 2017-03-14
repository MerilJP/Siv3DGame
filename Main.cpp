#include <Siv3D.hpp>

class Siv3DGame 
{
public:
	Siv3DGame() : Timer(true)
		, m_ball(320, 400, 8)
		, m_bar(10, 80)
		, pos_y(Window::Height() / 2)
	{
		Graphics::SetBackground(Palette::Black);
		Window::SetTitle(GameTitle);
		Window::Resize(nClientWidth, nClientHeight);
		Window::Centering();
		m_ballSpeed = Vec2(0, -speed);
#ifdef _DEBUG
#else
		System::SetExitEvent(WindowEvent::CloseButton);
#endif
	}

	void ClientStatus() 
	{
		FontAsset::Register(L"status", 10, Typeface::Default);
		FontAsset(L"status")(L"FPS : ", Profiler::FPS(), L" \nTIMER : ", Timer.ms(),L"ms\n").draw();
	}

	void GameUpdate()
	{
		
		m_ball.moveBy(m_ballSpeed);

		m_bar.setCenter(20,pos_y);

		if (Input::KeyUp.pressed && pos_y >= 0) {
			pos_y -= 5;
		}
		else if (Input::KeyDown.pressed && pos_y <= Window::Height()) {
			pos_y += 5;
		}


		m_ball.draw();
		m_bar.draw();
		
	}

private:
	const Stopwatch Timer;


	Rect m_bar;
	Circle m_ball;
	int pos_y;
	Vec2 m_ballSpeed;

	const double speed = 4.0;

	const String GameTitle = L"Siv3D Application";
	const int nClientWidth = 640;
	const int nClientHeight = 480;


};

void Main()
{

	Siv3DGame *GamePlay = new Siv3DGame();

	while (System::Update())
	{
		GamePlay->GameUpdate();

		GamePlay->ClientStatus();
	}
}
