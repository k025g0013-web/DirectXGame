#include "TunaEngine.h"
#include "Game.h"
#include <memory>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	auto engine = std::make_unique<Tuna::TunaEngine>();
	engine->Initialize(L"TunaEngine", 1280, 720);

	{
		Game game;
		game.Initialize(engine.get());

		while (engine->ProcessMessage()) {
			game.Update();
			game.Draw();

			if (game.IsEndRequested()) {
				break;
			}
		}
	}

	engine->Finalize();
	return 0;
}