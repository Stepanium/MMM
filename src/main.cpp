#define OLC_PGE_APPLICATION

#include <stack>

#include "olcPixelGameEngine.h"

using namespace std;

class MMM : public olc::PixelGameEngine
{
private:
    int m_nMazeWidth;
    int m_nMazeHeight;
    int *m_maze;

    int m_nPathWidth;

    enum
    {
        CELL_PATH_NORTH = 0x01,
        CELL_PATH_EAST = 0x02,
        CELL_PATH_SOUTH = 0x04,
        CELL_PATH_WEST = 0x08,
        CELL_VISITED = 0x10
    };

    int m_nVisitedCells = 0;

    stack<pair<int, int>> m_stack;

public:
    MMM()
    {
        sAppName = "Memory Maze Man!";
    }

protected:
    bool OnUserCreate() override
    {
        m_nMazeWidth = 32;
        m_nMazeHeight = 32;
        m_maze = new int[m_nMazeWidth * m_nMazeHeight];

        memset(m_maze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

        m_stack.push(make_pair(0, 0));
        m_maze[0] = CELL_VISITED;
        m_nVisitedCells = 1;

        m_nPathWidth = 7;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Do maze algorithm

        auto offset = [&](int x, int y)
        {
            return (m_stack.top().second + y) * m_nMazeWidth + m_stack.top().first + x;
        };

        if (m_nVisitedCells < m_nMazeHeight * m_nMazeWidth)
        {

            // Create a set of the unvisited neighbours
            vector<int> neighbours;

            // North neighbour
            if (m_stack.top().second > 0 && (m_maze[offset(0, -1)] & CELL_VISITED) == 0)
                neighbours.push_back(0);

            // East neighbour
            if (m_stack.top().first < m_nMazeWidth - 1 && (m_maze[offset(1, 0)] & CELL_VISITED) == 0)
                neighbours.push_back(1);

            // South neighbour
            if (m_stack.top().second < m_nMazeHeight - 1 && (m_maze[offset(0, 1)] & CELL_VISITED) == 0)
                neighbours.push_back(2);

            // West neighbour
            if (m_stack.top().first > 0 && (m_maze[offset(-1, 0)] & CELL_VISITED) == 0)
                neighbours.push_back(3);

            // Are there any neighbours available?
            if (!neighbours.empty())
            {
                int next_cell_dir = neighbours[rand() % neighbours.size()];

                switch (next_cell_dir)
                {
                    // North
                case 0:
                    m_maze[offset(0, 0)] |= CELL_PATH_NORTH;
                    m_maze[offset(0, -1)] |= CELL_PATH_SOUTH | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + -1)));
                    break;
                    // East
                case 1:
                    m_maze[offset(0, 0)] |= CELL_PATH_EAST;
                    m_maze[offset(+1, 0)] |= CELL_PATH_WEST | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
                    break;
                    // South
                case 2:
                    m_maze[offset(0, 0)] |= CELL_PATH_SOUTH;
                    m_maze[offset(0, +1)] |= CELL_PATH_NORTH | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
                    break;
                    // West
                case 3:
                    m_maze[offset(0, 0)] |= CELL_PATH_WEST;
                    m_maze[offset(-1, 0)] |= CELL_PATH_EAST | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + -1), (m_stack.top().second + 0)));
                    break;
                }

                m_nVisitedCells++;
            }
            else
            {
                // No available neighbours so backtrack!
                m_stack.pop();
            }
        }

        // DRAWING//
        Clear(olc::BLACK);

        for (int x = 0; x < m_nMazeWidth; x++)
        {
            for (int y = 0; y < m_nMazeHeight; y++)
            {
                for (int px = 0; px < m_nPathWidth; px++)
                    for (int py = 0; py < m_nPathWidth; py++)
                        if (m_maze[x + y * m_nMazeWidth] & CELL_VISITED)
                        {
                            Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::WHITE);
                        }
                        else
                        {
                            Draw(x * (m_nPathWidth + 1) + px, y * (m_nPathWidth + 1) + py, olc::BLUE);
                        }

                // Draw passageways between cells
                for (int p = 0; p < m_nPathWidth; p++)
                {
                    if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_SOUTH)
                        Draw(x * (m_nPathWidth + 1) + p, y * (m_nPathWidth + 1) + m_nPathWidth); // Draw South Passage

                    if (m_maze[y * m_nMazeWidth + x] & CELL_PATH_EAST)
                        Draw(x * (m_nPathWidth + 1) + m_nPathWidth, y * (m_nPathWidth + 1) + p); // Draw East Passage
                }
            }
        }

        return true;
    }
};

int main()
{
    MMM demo;
    if (demo.Construct(256, 256, 2, 2))
        demo.Start();

    return 0;
}