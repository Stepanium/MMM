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
    CELL_VISITED = 0x10,
    CELL_START = 0x20,
    CELL_FINISH = 0x40
};

struct maze
{
    int m_nMazeWidth;
    int m_nMazeHeight;
    int *m_nMaze;
    olc::Pixel floorColor;
    olc::Pixel wallColor;

    olc::Pixel startColor;
    olc::Pixel finishColor;
    int start_x, start_y;
    int finish_x, finish_y;

    void GenerateMaze(int m_nMazeWidth, int m_nMazeHeight)
    {
        this->m_nMazeWidth = m_nMazeWidth;
        this->m_nMazeHeight = m_nMazeWidth;

        start_x = (int)((float)m_nMazeWidth * 0.5f);
        start_y = m_nMazeHeight - 1;
        finish_x = start_x;
        finish_y = 0;

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
            if (m_stack.top().first == start_x && m_stack.top().second == start_y)
                m_nMaze[offset(0, 0)] |= CELL_START;
            else if (m_stack.top().first == finish_x && m_stack.top().second == finish_y)
                m_nMaze[offset(0, 0)] |= CELL_FINISH;

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

    float counter = 0.0f;
    float freezeTime = 1.0f; // in seconds

    olc::Pixel color;
    // olc::Pixel antiColor;

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

struct camera
{
    vec2d origin;
    vec2d center;
    float zoom;

    float smooth = 0.95f; // how smooth is the movement?
    vec2d *target = nullptr;

    int nHalfScrW;
    int nHalfScrH;

    void Construct(vec2d *target, int nScreenWidth, int nScreenHeight, float zoom, float smooth = 0.95f)
    {
        this->target = target;
        this->center = *target;
        nHalfScrW = nScreenWidth * 0.5f;
        nHalfScrH = nScreenHeight * 0.5f;
        this->origin = {center.x - nHalfScrW / zoom, center.y - nHalfScrH / zoom};
        this->zoom = zoom;
        this->smooth = smooth;
    }

    void Update()
    {
        vec2d diff = *target - center;
        center += diff * smooth;
        this->origin = {center.x - nHalfScrW / zoom, center.y - nHalfScrH / zoom};
    }

    void Update(float zoom)
    {
        if (zoom >= 0.0f)
            this->zoom = zoom;
        vec2d diff = *target - center;
        center += diff * smooth;
        this->origin = {center.x - nHalfScrW / zoom, center.y - nHalfScrH / zoom};
    }

    vec2d Project(vec2d p)
    {
        vec2d p_projected = p;
        p_projected -= origin;
        p_projected *= zoom;
        return p_projected;
    }
};

class MMM : public olc::PixelGameEngine
{
private:
    maze m_maze;
    int m_nMazeWidth;
    int m_nMazeHeight;

    int m_nPathWidth;
    int m_nTileWidth;
    int m_nWallWidth;

    player p_player;

    bool bLight = true;
    bool bExplore = true; // is gameplay right now in the explore mode or int the remember and find the exit mode?
    bool bFreeze = false;

    camera c_camera;

    void DrawMaze(maze m_maze, player p_player, bool bLight, camera c_camera)
    {
        for (int x = 0; x < m_maze.m_nMazeWidth; x++)
        {
            for (int y = 0; y < m_maze.m_nMazeHeight; y++)
            {
                olc::Pixel color = (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_START) ? m_maze.startColor : (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_FINISH) ? m_maze.finishColor
                                                                                                                                                                                : m_maze.floorColor;
                int x_transformed = m_nWallWidth + x * m_nTileWidth;
                int y_transformed = m_nWallWidth + y * m_nTileWidth;
                vec2d center = {(float)x_transformed + 0.5f * m_nTileWidth, (float)y_transformed + 0.5f * m_nTileWidth};
                if (bLight || x_transformed > ScreenWidth() || y_transformed > ScreenHeight() || (p_player.pos - center).GetLengthSqared() < p_player.visionRadius * p_player.visionRadius)
                {
                    vec2d topLeft_projected = c_camera.Project({ (float)x_transformed, (float)y_transformed });
                    int newPathW = (int)((float)m_nPathWidth * c_camera.zoom);
                    FillRect(topLeft_projected.x, topLeft_projected.y, newPathW, newPathW, color);

                    // Draw passageways between cells
                    for (int p = 0; p < newPathW; p++)
                    {
                        for (int k = 0; k < newPathW; k++)
                        {
                            if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_SOUTH)
                                Draw(topLeft_projected.x + p, topLeft_projected.y + newPathW + k, color); // Draw South Passage

                            if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_EAST)
                                Draw(topLeft_projected.x + newPathW + k, topLeft_projected.y + p, color); // Draw East Passage
                        }
                    }
                }
            }
        }
    }

    void DrawPlayer(player p, camera c_camera)
    {
        vec2d p_pos_projected = c_camera.Project(p.pos);
        float p_r_projected = p.radius * c_camera.zoom;
        if (p_pos_projected.x >= 0 && p_pos_projected.x < ScreenWidth() && 
            p_pos_projected.y >= 0 && p_pos_projected.y < ScreenHeight())
            FillCircle((int)p_pos_projected.x, (int)p_pos_projected.y, (int)p_r_projected, p.color);
    }

public:
    MMM()
    {
        sAppName = "Memory Maze Man!";
    }

protected:
    bool OnUserCreate() override
    {
        m_nMazeWidth = 18;
        m_nMazeHeight = 18;
        m_maze.GenerateMaze(m_nMazeWidth, m_nMazeHeight);
        m_nPathWidth = 30;
        m_nWallWidth = 2;
        m_nTileWidth = m_nPathWidth + m_nWallWidth;
        m_maze.wallColor = olc::Pixel(10, 10, 10);
        m_maze.floorColor = olc::Pixel(240, 160, 0);
        m_maze.wallColor = olc::Pixel(10, 10, 10);

        m_maze.startColor = olc::GREEN;
        m_maze.finishColor = olc::RED;

        p_player.pos = {((float)m_maze.start_x + 0.5f) * m_nTileWidth, ((float)m_maze.start_y + 0.25f) * m_nTileWidth};
        p_player.radius = 2.0f;
        p_player.speed = 160.0f;
        p_player.visionRadius = 2.0f * m_nTileWidth;
        p_player.color = m_maze.wallColor;
        // p_player.antiColor = olc::Pixel(255 - p_player.color.r, 255 - p_player.color.g, 255 - p_player.color.b);

        c_camera.Construct(&p_player.pos, ScreenWidth(), ScreenHeight(), 2.0f, 0.5f);
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        ///// INPUT/////
        // translation input
        vec2d dir = {0, 0};
        if (GetKey(olc::Key::W).bHeld || GetKey(olc::Key::UP).bHeld)
            dir.y -= 1.0f;
        if (GetKey(olc::Key::S).bHeld || GetKey(olc::Key::DOWN).bHeld)
            dir.y += 1.0f;
        if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::LEFT).bHeld)
            dir.x -= 1.0f;
        if (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::RIGHT).bHeld)
            dir.x += 1.0f;
        // ready to start

        if (GetKey(olc::Key::R).bPressed && !bFreeze)
            bExplore = false;
        ///////////////

        //// CALCULATION ////

        if (!bExplore)
        {
            if (bLight)
            {
                bLight = false;
                p_player.pos = {((float)m_maze.start_x + 0.5f) * m_nTileWidth, ((float)m_maze.start_y + 0.5f) * m_nTileWidth};
            }
        }

        // player movement and collision resolution
        if (bFreeze)
            p_player.counter += fElapsedTime;
        else
            p_player.Move(dir, m_maze, m_nTileWidth, m_nPathWidth, m_nWallWidth, fElapsedTime);

        if (p_player.counter >= p_player.freezeTime)
        {
            p_player.counter = 0.0f;
            bFreeze = false;
        }

        if ((int)(p_player.pos.x / (float)m_nTileWidth) == m_maze.finish_x &&
            (int)(p_player.pos.y / (float)m_nTileWidth) == m_maze.finish_y)
        {
            bLight = true;
            bExplore = true;
            m_maze.GenerateMaze(m_nMazeWidth, m_nMazeHeight);
            p_player.pos = {((float)m_maze.start_x + 0.5f) * m_nTileWidth, ((float)m_maze.start_y + 0.5f) * m_nTileWidth};
            bFreeze = true;
        }

        c_camera.Update();
        ///////////////

        ////DRAWING////
        Clear(m_maze.wallColor);

        // draw maze
        DrawMaze(m_maze, p_player, bLight, c_camera);
        // draw player
        DrawPlayer(p_player, c_camera);
        ////////////////

        return true;
    }
};

int main()
{
    MMM demo;
    if (demo.Construct(578, 578, 1, 1, false))
        demo.Start();

    return 0;
}