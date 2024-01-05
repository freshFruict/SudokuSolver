#include <iostream>
#include <cstdint>
#include <set>
#include <vector>
#include <string>
#include <utility>

class Game
{
private:
	std::vector<std::set<char>> candidates;
	constexpr inline int c(int x, int y) { return 9 * y + x; }

	void exclude_candidate_horizontal(char ca, int y, int x_ignore_min, int x_ignore_max)
	{
		for (int x = 0; x < 9; ++x)
			if (x_ignore_min > x || x_ignore_max <= x)
				candidates[c(x, y)].erase(ca);
	}
	void exclude_candidate_vertical(char ca, int x, int y_ignore_min, int y_ignore_max)
	{
		for (int y = 0; y < 9; ++y)
			if (y_ignore_min > y || y_ignore_max <= y)
				candidates[c(x, y)].erase(ca);
	}
	void exclude_square(char ca, int square_id, int x_ignore, int y_ignore)
	{
		for (int x = 3 * (square_id % 3); x < 3 * (square_id % 3) + 3; ++x)
			for (int y = 3 * (square_id / 3); y < 3 * (square_id / 3) + 3; ++y)
				if (x != x_ignore || y != y_ignore)
					candidates[c(x, y)].erase(ca);
	}
	int getSquareId(int x, int y)
	{
		return 3 * (y / 3) + x / 3;
	}
	int squareGetCandidateX(const std::vector<std::set<char>>& src, char ca, int square_id)
	{
		int candidate_x = -1;
		for (int x = 3 * (square_id % 3); x < 3 * (square_id % 3) + 3; ++x)
			for (int y = 3 * (square_id / 3); y < 3 * (square_id / 3) + 3; ++y)
				if (src[c(x, y)].contains(ca))
				{
					if (candidate_x == -1)
						candidate_x = x;
					else if (candidate_x != x)
						return -1;
				}
		return candidate_x;
	}
	int squareGetCandidateY(const std::vector<std::set<char>>& src, char ca, int square_id)
	{
		int candidate_y = -1;
		for (int x = 3 * (square_id % 3); x < 3 * (square_id % 3) + 3; ++x)
			for (int y = 3 * (square_id / 3); y < 3 * (square_id / 3) + 3; ++y)
				if (src[c(x, y)].contains(ca))
				{
					if (candidate_y == -1)
						candidate_y = y;
					else if (candidate_y != y)
						return -1;
				}
		return candidate_y;
	}
	std::pair<int, int> squareGetOnly(const std::vector<std::set<char>>& src, char ca, int square_id)
	{
		std::pair<int, int> co{-1,-1};
		for (int x = 3 * (square_id % 3); x < 3 * (square_id % 3) + 3; ++x)
			for (int y = 3 * (square_id / 3); y < 3 * (square_id / 3) + 3; ++y)
				if (src[c(x, y)].contains(ca))
				{
					if (co == std::pair<int, int>{-1, -1})
						co = { x,y };
					else
						return { -1,-1 };
				}
		return co;
	}
	int horizontalGetOnly(const std::vector<std::set<char>>& src, char ca, int y)
	{
		int candidate_x = -1;
		for (int x = 0; x < 9; ++x)
			if (src[c(x, y)].contains(ca))
			{
				if (candidate_x == -1)
					candidate_x = x;
				else
					return -1;
			}
		return candidate_x;
	}
	int verticalGetOnly(const std::vector<std::set<char>>& src, char ca, int x)
	{
		int candidate_y = -1;
		for (int y = 0; y < 9; ++y)
			if (src[c(x, y)].contains(ca))
			{
				if (candidate_y == -1)
					candidate_y = y;
				else
					return -1;
			}
		return candidate_y;
	}
public:
	Game()
	{
		candidates.resize(81);
		for (int i = 0; i < 81; ++i)
			candidates[i] = std::set<char>{ '_' };
	}
	Game(const Game&) = default;
	Game& operator=(const Game&) = default;
	std::set<char>& operator() (int x, int y)
	{
		return candidates[c(x, y)];
	}
	std::string get_printable()
	{
		std::string result;
		result.reserve(178);
		for (int y = 0; y < 9; ++y)
		{
			if (y % 3 == 0)
				result += std::string(12, '=') + '\n';
			for (int x = 0; x < 9; ++x)
				result += ((candidates[c(x, y)].size() == 1) ? ((x % 3 == 0) ? (std::string("|") + (*candidates[c(x, y)].begin())) : std::string(1, *candidates[c(x, y)].cbegin())) : ((x % 3 == 0) ? "|*" : "*"));
			result += '\n';
		}
		result += std::string(12, '=') + '\n';
		return result;
	}
	bool isSolved()
	{
		for (int i = 0; i < 81; ++i)
			if (candidates[i].size() != 1)
				return false;
		return true;
	}
	bool solveStep()
	{
		std::vector<std::set<char>> src(candidates);

		for (int x = 0; x < 9; ++x)
			for (int y = 0; y < 9; ++y)
				if (src[c(x, y)].size() == 1)
				{
					exclude_candidate_horizontal(*src[c(x, y)].cbegin(), y, x, x + 1);
					exclude_candidate_vertical(*src[c(x, y)].cbegin(), x, y, y + 1);
					exclude_square(*src[c(x, y)].cbegin(), getSquareId(x, y), x, y);
				}

		for (int square_id = 0; square_id < 9; ++square_id)
			for (char ca = '1'; ca <= '9'; ++ca)
			{
				int x = squareGetCandidateX(src, ca, square_id), y = squareGetCandidateY(src, ca, square_id);
				if (x != -1)
					exclude_candidate_vertical(ca, x, 3 * (square_id / 3), 3 * (square_id / 3) + 3);
				if (y != -1)
					exclude_candidate_horizontal(ca, y, 3 * (square_id % 3), 3 * (square_id % 3) + 3);
			}

		for (int square_id = 0; square_id < 9; ++square_id)
			for (char ca = '1'; ca <= '9'; ++ca)
			{
				std::pair<int, int> co = squareGetOnly(src, ca, square_id);
				if (co != std::pair<int, int>{-1, -1})
					candidates[c(co.first, co.second)] = { ca };
			}
		for (int y = 0; y < 9; ++y)
			for (char ca = '1'; ca <= '9'; ++ca)
			{
				int x = horizontalGetOnly(src, ca, y);
				if (x != -1)
					candidates[c(x, y)] = { ca };
			}
		for (int x = 0; x < 9; ++x)
			for (char ca = '1'; ca <= '9'; ++ca)
			{
				int y = verticalGetOnly(src, ca, x);
				if (y != -1)
					candidates[c(x, y)] = { ca };
			}

		return src != candidates;
	}
};

int countSolutionPoints(Game game, int x, int y, char ca)
{
	int globalCountOfCandidates = 0;
	for (int x = 0; x < 9; ++x)
		for (int y = 0; y < 9; ++y)
			globalCountOfCandidates += game(x, y).size();
	game(x, y) = {ca};
	
	while (game.solveStep()) {};
	
	for (int x = 0; x < 9; ++x)
		for (int y = 0; y < 9; ++y)
			globalCountOfCandidates -= game(x, y).size();
	return globalCountOfCandidates;
}

double countAverageSolutionPoints(Game game, int x, int y)
{
	double points = 0;	
	for (char ca : game(x, y))
		points += countSolutionPoints(game, x, y, ca);
	return points / game(x, y).size();
}

std::pair<int, int> getBestRequiredPosition(Game game)
{
	std::pair<int, int> best_pos;
	double best_count = -1;
	for (int x = 0; x < 9; ++x)
		for (int y = 0; y < 9; ++y)
			if (game(x, y).size() > 1)
			{
				double count = countAverageSolutionPoints(game, x, y);
				if (count > best_count)
				{
					best_count = count;
					best_pos.first = x;
					best_pos.second = y;
				}
			}
	return best_pos;
}

int main()
{
	Game game;
	std::cout << game.get_printable();
	for (int y = 0; y < 9; ++y)
		for (int x = 0; x < 9; ++x)
		{
		again:
			char ch = std::cin.get();
			if (ch == '*')
				game(x, y) = { '1','2','3','4','5','6','7','8','9' };
			else if (std::isdigit(ch))
				game(x, y) = { ch };
			else
				goto again;
			std::system("cls");
			std::cout << game.get_printable();
		}
	while (!game.isSolved())
	{
		if (!game.solveStep())
		{
			std::pair<int, int> best_pos = getBestRequiredPosition(game);
			std::cout << "Enter value of (" << best_pos.first << ", " << best_pos.second << "): " << std::flush;
			char val;
			game(best_pos.first, best_pos.second) = { static_cast<char>(std::cin.get()) };
			std::cin.ignore(1);
		}
		std::system("cls");
		std::cout << game.get_printable() << std::flush;
		std::cin.get();
	}
	std::cout << game.get_printable();
	return 0;
}