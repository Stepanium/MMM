#define OLC_PGE_APPLICATION

#include <stack>

#include "olcPixelGameEngine.h"

using namespace std;

// g++ -o main main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17

enum
{
    CELL_PATH_NORTH = 0x01,
    CELL_PATH_EAST = 0x02,
    CELL_PATH_SOUTH = 0x04,
    CELL_PATH_WEST = 0x08,
    CELL_VISITED = 0x10
};

struct maze
{
    int m_nMazeWidth;
    int m_nMazeHeight;
    int *m_nMaze;
    olc::Pixel floorColor;
    olc::Pixel wallColor;

    void GenerateMaze(int m_nMazeWidth, int m_nMazeHeight)
    {

        this->m_nMazeWidth = m_nMazeWidth;
        this->m_nMazeHeight = m_nMazeWidth;
        m_nMaze = new int[m_nMazeWidth * m_nMazeHeight];

        memset(m_nMaze, 0x00, m_nMazeWidth * m_nMazeHeight * sizeof(int));

        stack<pair<int, int>> m_stack;
        m_stack.push(make_pair(0, 0));

        m_nMaze[0] = CELL_VISITED;
        int m_nVisitedCells = 1;

        // Do maze algorithm

        auto offset = [&](int x, int y)
        {
            return (m_stack.top().second + y) * m_nMazeWidth + m_stack.top().first + x;
        };

        while (m_nVisitedCells < m_nMazeHeight * m_nMazeWidth)
        {

            // Create a set of the unvisited neighbours
            vector<int> neighbours;

            // North neighbour
            if (m_stack.top().second > 0 && (m_nMaze[offset(0, -1)] & CELL_VISITED) == 0)
                neighbours.push_back(0);

            // East neighbour
            if (m_stack.top().first < m_nMazeWidth - 1 && (m_nMaze[offset(1, 0)] & CELL_VISITED) == 0)
                neighbours.push_back(1);

            // South neighbour
            if (m_stack.top().second < m_nMazeHeight - 1 && (m_nMaze[offset(0, 1)] & CELL_VISITED) == 0)
                neighbours.push_back(2);

            // West neighbour
            if (m_stack.top().first > 0 && (m_nMaze[offset(-1, 0)] & CELL_VISITED) == 0)
                neighbours.push_back(3);

            // Are there any neighbours available?
            if (!neighbours.empty())
            {
                int next_cell_dir = neighbours[rand() % neighbours.size()];

                switch (next_cell_dir)
                {
                    // North
                case 0:
                    m_nMaze[offset(0, 0)] |= CELL_PATH_NORTH;
                    m_nMaze[offset(0, -1)] |= CELL_PATH_SOUTH | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + -1)));
                    break;
                    // East
                case 1:
                    m_nMaze[offset(0, 0)] |= CELL_PATH_EAST;
                    m_nMaze[offset(+1, 0)] |= CELL_PATH_WEST | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + 1), (m_stack.top().second + 0)));
                    break;
                    // South
                case 2:
                    m_nMaze[offset(0, 0)] |= CELL_PATH_SOUTH;
                    m_nMaze[offset(0, +1)] |= CELL_PATH_NORTH | CELL_VISITED;
                    m_stack.push(make_pair((m_stack.top().first + 0), (m_stack.top().second + 1)));
                    break;
                    // West
                case 3:
                    m_nMaze[offset(0, 0)] |= CELL_PATH_WEST;
                    m_nMaze[offset(-1, 0)] |= CELL_PATH_EAST | CELL_VISITED;
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
    }
};

struct vec2d
{
    float x = 0;
    float y = 0;

    vec2d operator+(const vec2d &rhs)
    {
        return {x + rhs.x, y + rhs.y};
    }

    vec2d operator-(const vec2d &rhs)
    {
        return {x - rhs.x, y - rhs.y};
    }

    void operator+=(const vec2d &rhs)
    {
        x += rhs.x;
        y += rhs.y;
    }

    void operator-=(const vec2d &rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
    }

    vec2d operator*(const float &rhs)
    {
        return {x * rhs, y * rhs};
    }

    vec2d operator/(const float &rhs)
    {
        return {x / rhs, y / rhs};
    }

    void operator*=(const float &rhs)
    {
        x *= rhs;
        y *= rhs;
    }

    void operator/=(const float &rhs)
    {
        x /= rhs;
        y /= rhs;
    }

    float GetLengthSqared()
    {
        return x * x + y * y;
    }

    float GetLength()
    {
        float l2 = GetLengthSqared();
        float out = sqrtf(l2);
        return out;
    }

    void Normalize()
    {
        float l = GetLength();
        if (l > 0.0f)
        {
            float reverseL = 1.0f / l;
            x *= reverseL;
            y *= reverseL;
        }
    }

    float DotProduct(const vec2d &rhs)
    {
        return x * rhs.x + y * rhs.y;
    }

    float CrossProduct(const vec2d &rhs)
    {
        return x * rhs.y - y * rhs.x;
    }
};

struct player
{
    vec2d pos = {0, 0};
    vec2d dir = {0, 0};
    float radius = 1.0f;
    float visionRadius = 70.0f;
    float speed = 16.0f;

    olc::Pixel color;
    olc::Pixel antiColor;

    void Move(vec2d direction, maze maze, int m_nTileWidth, int m_nPathWidth, int m_nWallWidth, float fElapsedTime)
    {
        if (direction.x == 0.0f && direction.y == 0.0f)
            return;

        direction.Normalize();
        vec2d move = direction * speed * fElapsedTime;
        vec2d nextPos = pos + move;

        float halfW = 0.5f * m_nWallWidth;
        bool fixX = false, fixY = false;

        if (nextPos.x < halfW)
        {
            nextPos.x = halfW;
            fixX = true;
        }
        else if (nextPos.x > maze.m_nMazeWidth * m_nTileWidth - halfW)
        {
            nextPos.x = maze.m_nMazeWidth * m_nTileWidth - halfW;
            fixX = true;
        }

        if (nextPos.y < halfW)
        {
            nextPos.y = halfW;
            fixY = true;
        }
        else if (nextPos.y > maze.m_nMazeHeight * m_nTileWidth - halfW)
        {
            nextPos.y = maze.m_nMazeHeight * m_nTileWidth - halfW;
            fixY = true;
        }

        int m_pos_x = (int)(pos.x / m_nTileWidth);
        int m_pos_y = (int)(pos.y / m_nTileWidth);

        int m_next_x = (int)(nextPos.x / m_nTileWidth);
        int m_next_y = (int)(nextPos.y / m_nTileWidth);

        // COLLISION DETECTION AND RESOLUTION
        if (!fixY)
        {
            // NORTH
            if (m_pos_y > m_next_y)
            {
                int t = maze.m_nMaze[m_pos_y * maze.m_nMazeWidth + m_pos_x];
                if (!(t & CELL_PATH_NORTH))
                {
                    float bottomY = (float)(m_pos_y * m_nTileWidth) + halfW;
                    nextPos.y = bottomY;
                }
            }
            // SOUTH
            else if (m_pos_y < m_next_y)
            {
                int t = maze.m_nMaze[m_pos_y * maze.m_nMazeWidth + m_pos_x];
                if (!(t & CELL_PATH_SOUTH))
                {
                    float topY = (float)(m_next_y * m_nTileWidth) - halfW;
                    nextPos.y = topY;
                }
            }
        }

        if (!fixX)
        {
            // EAST
            if (m_pos_x < m_next_x)
            {
                int t = maze.m_nMaze[m_pos_y * maze.m_nMazeWidth + m_pos_x];
                if (!(t & CELL_PATH_EAST))
                {
                    float leftX = (float)(m_next_x * m_nTileWidth) - halfW;
                    nextPos.x = leftX;
                }
            }
            // WEST
            else if (m_pos_x > m_next_x)
            {
                int t = maze.m_nMaze[m_pos_y * maze.m_nMazeWidth + m_pos_x];
                if (!(t & CELL_PATH_WEST))
                {
                    float rightX = (float)(m_pos_x * m_nTileWidth) + halfW;
                    nextPos.x = rightX;
                }
            }
        }

        pos = nextPos;
    }
};

class MMM : public olc::PixelGameEngine
{
private:
    maze m_maze;
    int m_nPathWidth;
    int m_nTileWidth;
    int m_nWallWidth;
    player p;

    bool light = true;

public:
    MMM()
    {
        sAppName = "Memory Maze Man!";
    }

protected:
    bool OnUserCreate() override
    {
        m_maze.GenerateMaze(16, 16);
        m_nPathWidth = 32;
        m_nWallWidth = 2;
        m_nTileWidth = m_nPathWidth + m_nWallWidth;
        m_maze.wallColor = olc::Pixel(10, 10, 10);
        m_maze.floorColor = olc::Pixel(240, 160, 0);

        p.pos = {250.0f, 535.0f};
        p.radius = 2.0f;
        p.speed = 128.0f;
        p.visionRadius = 2.0f * m_nTileWidth;
        p.color = m_maze.wallColor;
        p.antiColor = olc::Pixel(255 - p.color.r, 255 - p.color.g, 255 - p.color.b);

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        // INPUT//
        // TRANSLATION//
        vec2d dir = {0, 0};
        if (GetKey(olc::Key::W).bHeld)
            dir.y -= 1.0f;
        if (GetKey(olc::Key::S).bHeld)
            dir.y += 1.0f;
        if (GetKey(olc::Key::A).bHeld)
            dir.x -= 1.0f;
        if (GetKey(olc::Key::D).bHeld)
            dir.x += 1.0f;
        //////

        p.Move(dir, m_maze, m_nTileWidth, m_nPathWidth, m_nWallWidth, fElapsedTime);

        // DRAWING//
        Clear(m_maze.wallColor);

        // DRAW MAZE
        for (int x = 0; x < m_maze.m_nMazeWidth; x++)
        {
            for (int y = 0; y < m_maze.m_nMazeHeight; y++)
            {
                int x_transformed = m_nWallWidth + x * m_nTileWidth;
                int y_transformed = m_nWallWidth + y * m_nTileWidth;
                vec2d center = {(float)x_transformed + 0.5f * m_nTileWidth, (float)y_transformed + 0.5f * m_nTileWidth};
                if (light || (p.pos - center).GetLengthSqared() < p.visionRadius * p.visionRadius)
                {
                    FillRect(x_transformed, y_transformed, m_nPathWidth, m_nPathWidth, m_maze.floorColor);

                    // Draw passageways between cells
                    for (int p = 0; p < m_nPathWidth; p++)
                    {
                        for (int k = 0; k < m_nWallWidth; k++)
                        {
                            if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_SOUTH)
                                Draw(x_transformed + p, y_transformed + m_nPathWidth + k, m_maze.floorColor); // Draw South Passage

                            if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_EAST)
                                Draw(x_transformed + m_nPathWidth + k, y_transformed + p, m_maze.floorColor); // Draw East Passage
                        }
                    }
                }
            }
        }

        // DRAW PLAYER
        FillCircle((int)p.pos.x, (int)p.pos.y, (int)p.radius, p.color);

        return true;
    }
};

int main()
{
    MMM demo;
    if (demo.Construct(600, 600, 1, 1))
        demo.Start();

    return 0;
}