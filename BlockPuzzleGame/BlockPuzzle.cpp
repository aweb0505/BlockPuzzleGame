#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

constexpr int NORTH = 0;
constexpr int EAST = 1;
constexpr int SOUTH = 2;
constexpr int WEST = 3;

#define PRINTER(name) printer(#name, (name))

class BlockPuzzle : public olc::PixelGameEngine
{
public:
	BlockPuzzle()
	{
		sAppName = "Block Puzzle Game";
	}

	//#s are solid blocks, .s are empty space
	//TODO Add a separate floor string for the level so we can specify special tiles in the floor, and then put blocks on them, etc
	std::vector<std::string> levels;

	std::string sLevel1 =
		"################"
		"#..............#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#....P.+.......#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#............@.#"
		"#..............#"
		"#..............#"
		"################";

	std::string sLevel2 =
		"################"
		"#..............#"
		"#..P........@..#"
		"#..............#"
		"#..............#"
		"#......+.......#"
		"#......+.......#"
		"#......+.......#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#...........@@.#"
		"#..............#"
		"################";

	std::string sLevel3 =
		"################"
		"#..............#"
		"#..P........@..#"
		"#..............#"
		"#..............#"
		"#......+.......#"
		"#......+.......#"
		"#......+.......#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#...........##.#"
		"#...........@@##"
		"#..............#"
		"################";

	std::string sLevel4 =
		"################"
		"#..............#"
		"#..P........@..#"
		"#..............#"
		"#..............#"
		"#......+.......#"
		"#......+.......#"
		"#......+.......#"
		"#..............#"
		"#..............#"
		"#..............#"
		"#...........##.#"
		"#.........--@@##"
		"#..............#"
		"################";

	olc::vf2d vLevelSize = { 16,15 };
	olc::vf2d vBlockSize = { 16,16 };

	std::vector<olc::vi2d> vGoals;

	int level = 0;

	olc::Renderable gfxTiles;


	struct block {
		block() {

		}

		virtual void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) {

		}

		virtual bool Push(const int from) {
			return true;
		}

		virtual void Move() {

		}
	};

	struct block_solid : public block{
		void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) override{
			pge->FillRect(pos * size, size, olc::BLUE);
		}
		bool Push(const int from) override{
			return false;
		}
	};

	struct block_player : public block {
		void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) override {
			//pge->FillRect(pos * size, size, olc::WHITE);
			pge->DrawPartialSprite(pos * size, skin.Sprite(), olc::vi2d(2, 0) * size, size);
		}
		bool Push(const int from) override {
			return true;
		}
	};

	struct block_simple : public block {
		void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) override {
			//pge->FillRect(pos * size, size, olc::RED);
			pge->DrawPartialSprite(pos * size, skin.Sprite(), olc::vi2d(1, 0) * size, size);
		}
		bool Push(const int from) override {
			return true;
		}
	};

	struct block_horizontal : public block {
		void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) override {
			pge->FillRect(pos * size, size, olc::GREEN);
		}
		bool Push(const int from) override {
			return from == EAST || from == WEST;
		}
	};

	struct block_vertical : public block {
		void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) override {
			pge->FillRect(pos * size, size, olc::YELLOW);
		}
		bool Push(const int from) override {
			return from == NORTH || from == SOUTH;
		}
	};

	struct block_countdown : public block {
		int nMoves = 0;

		block_countdown(int m) {
			nMoves = m;
		}

		void DrawSelf(olc::PixelGameEngine* pge, const olc::vi2d& pos, const olc::vi2d& size, const olc::Renderable& skin) override {
			pge->FillRect(pos * size, size, olc::CYAN);
			pge->DrawString(pos * size + olc::vi2d(4, 4), std::to_string(nMoves), olc::BLACK);
		}

		bool Push(const int from) override {
			return nMoves > 0;
		}

		void Move() override {
			nMoves--;
		}
	};

	std::vector<std::unique_ptr<block>> vLevel;
	olc::vi2d vPlayer;

public:

	void LoadLevel(int n) {

		vLevel.clear();
		vGoals.clear();

		for (int y = 0; y < vLevelSize.y; y++) {
			for (int x = 0; x < vLevelSize.x; x++) {
				switch (levels[n][y * vLevelSize.x + x]) {
				case '#':
					vLevel.emplace_back(std::make_unique<block_solid>());
					break;
				case '-':
					vLevel.emplace_back(std::make_unique<block_horizontal>());
					break;
				case '|':
					vLevel.emplace_back(std::make_unique<block_vertical>());
					break;
				case 'P':
					vLevel.emplace_back(std::make_unique<block_player>());
					vPlayer = { x,y };
					break;
				case '+':
					vLevel.emplace_back(std::make_unique<block_simple>());
					break;
				case '5':
					vLevel.emplace_back(std::make_unique<block_countdown>(5));
					break;
				case '@':
					vGoals.push_back({ x, y });
					vLevel.emplace_back(nullptr);
					break;
				default:
					vLevel.emplace_back(nullptr);
				}

			}
		}
	}

	bool OnUserCreate() override
	{
		levels.push_back(sLevel1);
		levels.push_back(sLevel2);
		levels.push_back(sLevel3);
		levels.push_back(sLevel4);
		gfxTiles.Load("./Blocks.png");
		LoadLevel(0);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		bool bPushing = false;
		int dirPush = 0;

		if (GetKey(olc::Key::W).bPressed) {
			dirPush = NORTH;
			bPushing = true;
		}

		if (GetKey(olc::Key::S).bPressed) {
			dirPush = SOUTH;
			bPushing = true;
		}

		if (GetKey(olc::Key::A).bPressed) {
			dirPush = WEST;
			bPushing = true;
		}

		if (GetKey(olc::Key::D).bPressed) {
			dirPush = EAST;
			bPushing = true;
		}

		if (GetKey(olc::Key::R).bPressed) {
			LoadLevel(0);
		}

		auto id = [&](olc::vi2d& pos) {return pos.y * vLevelSize.x + pos.x; };

		if (bPushing) {
			olc::vi2d vBlock = vPlayer;

			bool bAllowPush = false;
			bool bTest = true;

			while (bTest) 
			{
				if (vLevel[id(vBlock)] != nullptr)
				{
					if (vLevel[id(vBlock)]->Push((dirPush + 2) % 4))
					{
						//Block is allowed to be pushed
						switch (dirPush) 
						{
						case NORTH:
							vBlock.y--;
							break;
						case SOUTH:
							vBlock.y++;
							break;
						case EAST:
							vBlock.x++;
							break;
						case WEST:
							vBlock.x--;
							break;
						}
					}
					else 
					{
						bTest = false;
					}
				}
				else 
				{
					bAllowPush = true;
					bTest = false;
				}
			}

			if (bAllowPush)
			{
				while (vBlock != vPlayer) 
				{
					olc::vi2d vSource = vBlock;
					switch (dirPush)
					{
					case NORTH:
						vSource.y++;
						break;
					case SOUTH:
						vSource.y--;
						break;
					case EAST:
						vSource.x--;
						break;
					case WEST:
						vSource.x++;
						break;
					}

					if (vLevel[id(vSource)] != nullptr) {
						vLevel[id(vSource)]->Move();
					}
					std:swap(vLevel[id(vSource)], vLevel[id(vBlock)]);

					vBlock = vSource;
				}

				switch (dirPush)
				{
				case NORTH:
					vPlayer.y--;
					break;
				case SOUTH:
					vPlayer.y++;
					break;
				case EAST:
					vPlayer.x++;
					break;
				case WEST:
					vPlayer.x--;
					break;
				}

			}

		}


		int nGoals = 0;
		for (auto& g : vGoals) {
			if (vLevel[id(g)]) {
				nGoals++;
				if (nGoals == vGoals.size()) {
					level+=1;
					if (level >= levels.size()) {
						//std::cout << levels.size() << std::endl;
						break;
					}
					else {
						LoadLevel(level);
					}
					std::cout << level << std::endl;
				}
			}
		}

		Clear(olc::BLACK);

		for (auto& g : vGoals) {
			FillCircle(g * vBlockSize + vBlockSize / 2, vBlockSize.x / 2 - 2, olc::DARK_YELLOW);
		}

		olc::vi2d vTilePos = { 0,0 };

		for (vTilePos.y = 0; vTilePos.y < vLevelSize.y; vTilePos.y++) {
			for (vTilePos.x = 0; vTilePos.x < vLevelSize.x; vTilePos.x++) {

				auto& b = vLevel[id(vTilePos)];

				if (b) {
					b->DrawSelf(this, vTilePos, vBlockSize, gfxTiles);
				}


			}
		}
		
		if (level >= levels.size()) {
			DrawString(4, 4, "You win, press r to play again", olc::WHITE);
			if (GetKey(olc::Key::R).bPressed) {
				level = 0;
				LoadLevel(0);
			}
		}
		else {
			DrawString(128, 4, "Level:" + std::to_string(level + 1), olc::WHITE);
			DrawString(4, 4, "Goals: " + std::to_string(nGoals) + "/" + std::to_string(vGoals.size()), olc::WHITE);
		}


		
		return true;
	}
};



int main()
{
	BlockPuzzle demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}

