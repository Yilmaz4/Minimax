#include <iostream>
#include <vector>
#include <windows.h>
#include <map>
#include <algorithm>
#include <string>
#include <sstream>
#include <format>
#include <chrono>
#include <thread>

constexpr auto full_block       = L"\u2588";
constexpr auto upper_half_block = L"\u2580";
constexpr auto lower_half_block = L"\u2584";

namespace Algorithms {
    std::vector<std::vector<int>> rotate(std::vector<std::vector<int>>& p) {
        std::vector<std::vector<int>> temp(p[0].size(), std::vector<int>(p.size()));

        int count = 0;
        int count2 = 0;

        for (count = 0; count < p.size(); count++) {
            for (count2 = 0; count2 < p[0].size(); count2++) {
                temp[count2][temp[0].size() - count - 1] = p[count][count2];
            }
        }
        return temp;
    }
}
class Board {
private:
    std::vector<std::vector<int>> map = {};
public:
    Board() {
        this->map = {
            { 0, 0, 0 },
            { 0, 0, 0 },
            { 0, 0, 0 },
        };
    }
    Board(std::vector<std::vector<int>> board) {
        this->map = board;
    }

    void make_move(int player, int x, int y) {
        this->map[x - 1][y - 1] = player;
    }
    void make_move(int player, int index) {
        this->map[floor(index / this->map.size()) - (this->map.size() + 1)][floor(index / this->map.size())] = player;
    }
    void set_board(Board board) {
        this->map = board.get_board();
    }
    std::vector<std::vector<int>> get_board() {
        return this->map;
    }
};

static class Minimax {
public:
    Board board;
    Minimax() {
        SetConsoleOutputCP(CP_UTF8);
        setvbuf(stdout, nullptr, _IOFBF, 1000);
        std::wcout.sync_with_stdio(false);
        std::wcout.imbue(std::locale("en_US.utf8"));

        SetCursor(LoadCursorW(NULL, IDC_HAND));
        while (this->is_game_over(board) == 0) {
            this->render_board(board, { 0, 8 });
            std::wcout << "Your move: ";
        take_input:
            std::wstring input;
            std::wcin >> input;
            
            if (input[1] != '-' || input.size() != 3) {
                this->render_board(board, { 0, 8 });
                std::wcout << L"Wrong format! Your move: ";
                goto take_input;
            }
            board.make_move(1, input[0] - '0', input[2] - '0');
            this->render_board(board, { 0, 8 });
            this->minimax(board, 2);
        }
        
        this->pause();
    }
protected:
    static inline void pause() {
        std::wcout << L"Press any key to continue . . . ";

        HANDLE hstdin;
        DWORD  mode;
        hstdin = GetStdHandle(STD_INPUT_HANDLE);
        GetConsoleMode(hstdin, &mode);
        SetConsoleMode(hstdin, ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);

        unsigned int ch = 10;
        while (ch == 10) {
            ch = std::wcin.get();
        }
        SetConsoleMode(hstdin, mode);
    }
private:
    void render_board(Board board, COORD current) {
        COORD  position = { 0, 0 };
        HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(output, position);
        std::wcout << std::wstring(1000, L' ') << std::endl;
        SetConsoleCursorPosition(output, position);

        std::wcout << L"    1   2   3\n  ┌───┬───┬───┐" << std::endl;
        std::map<int, std::wstring> char_map = {
            { 0, L" " },
            { 1, L"X" },
            { 2, L"O" },
        };
        int i = 0;
        for (std::vector<int> const& row : board.get_board()) {
            i++;
            std::wcout << std::format(L"{} │ {} │ {} │ {} │", i, char_map[row[0]], char_map[row[1]], char_map[row[2]]) << std::endl;
            auto v = board.get_board();
            std::wcout << (i == v.size() ? L"  └───┴───┴───┘" : L"  ├───┼───┼───┤") << std::endl;
        }
        SetConsoleCursorPosition(output, current);
    }
    bool is_matrix_empty(Board board) {
        for (int x = 0; x < board.get_board().size(); x++) {
            for (int y = 0; y < board.get_board()[0].size(); y++) {
                if (board.get_board()[x][y] != 0) return false;
            }
        }
        return true;
    }
    int is_game_over(Board board) {
        if (this->is_matrix_empty(board)) {
            return 0;
        }
        for (int i = 0; i < 2; i++) {
            auto map = board.get_board();
            for (std::vector<int> const& row : map) {
                if (std::adjacent_find(row.begin(), row.end(), std::not_equal_to<>()) == row.end() && row[0] != 0) {
                    std::cout << "game over" << std::endl;
                    return row[0];
                }
            }
            this->render_board(board, { 0, 8 });
            std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
            this->render_board(Algorithms::rotate(map), { 0, 8 });
            std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
        }
        for (int i = 0; i < 2; i++) {
            std::vector<int> diagonals = {};
            for (int j = (i == 0 ? 0 : 2); i == 0 ? j < 2 : j > 0; i == 0 ? j++ : j--) {
                auto map = board.get_board();
                diagonals.push_back(map[j][j]);
            }
            if (std::adjacent_find(diagonals.begin(), diagonals.end(), std::not_equal_to<>()) == diagonals.end() && diagonals[0] != 0) {
                return diagonals[0];
            }
        }
        return 0;
    }
    int minimax(Board board, int turn) {
        std::vector<std::vector<int>> empty_places = {};
        for (int x = 0; x < board.get_board().size(); x++) {
            for (int y = 0; y < board.get_board()[0].size(); y++) {
                if (board.get_board()[x][y] == 0) {
                    empty_places.push_back(*new std::vector<int>{ x, y });
                }
            }
        }
        std::vector<Board> scenerios = {};
        for (auto const& place : empty_places) {
            auto new_board = board.get_board();
            new_board[place[0]][place[1]] = turn;
            if (this->is_game_over(new_board) != 0) {
                return turn == 2 ? 1 : -1;
            }
            scenerios.push_back(*new Board(new_board));
        }
        std::vector<int> results;
        for (auto const& board : scenerios) {
            this->render_board(board, { 0, 8 });
            std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
            results.push_back(this->minimax(board, turn == 1 ? 2 : 1));
        }
        if (turn == 1) {
            return *std::min_element(results.begin(), results.end());
        }
        return *std::max_element(results.begin(), results.end());
    }
    void do_move(Board& board) {
        std::vector<std::vector<int>> empty_places = {};
        for (int x = 0; x < board.get_board().size(); x++) {
            for (int y = 0; y < board.get_board()[0].size(); y++) {
                empty_places.push_back(*new std::vector<int>{ x, y });
            }
        }
        std::vector<Board> scenerios = {};
        for (auto const& place : empty_places) {
            auto new_board = board.get_board();
            new_board[place[0]][place[1]] = 2;
            scenerios.push_back(*new Board(new_board));
        }
        std::map<std::vector<std::vector<int>>, int> results;
        for (auto& board : scenerios) {
            results[board.get_board()] = (this->minimax(board, 2));
        }
        int highest = 0;
        Board best;
        for (auto const& [vector, result] : results) {
            if (result > highest) {
                highest = result;
                best = Board(vector);
            }
        }
        board.set_board(board);
    }
};

int main(int argc, char* argv[], char* options[]) {
    Minimax minimax;
}