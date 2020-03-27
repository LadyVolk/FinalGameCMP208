#pragma once

class GameData
{
public:

	enum Difficulty { easy, medium, hard };

	GameData(Difficulty difficulty);

	Difficulty difficulty_;

};

