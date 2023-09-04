#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"

#include <stack>
#include "utiliities.h"

using namespace std;

// g++ -o main main.cpp -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -lpulse -lpulse-simple

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

class MMM : public olc::PixelGameEngine
{
private:
    maze m_maze;
    int m_nMazeWidth;
    int m_nMazeHeight;

    int m_nPathWidth;
    int m_nTileWidth;
    int m_nWallWidth;

    olc::Sprite *sprFloor[3];
    olc::Decal *decFloor[3];

    olc::Sprite *sprGameBG;
    olc::Decal *decGameBG;
    vec2d bg_game_scale;

    olc::Sprite *sprStart;
    olc::Decal *decStart;
    olc::Sprite *sprFinish;
    olc::Decal *decFinish;

    player p_player;
    olc::Sprite *sprPlayer;
    olc::Decal *decPlayer;

    camera c_camera;
    float zoomNull;   // inspect mode
    float zoomSearch; // search mode

    olc::Sprite *sprFading;
    olc::Decal *decFading;
    float lightScaleNormal;
    float lightScaleSmall;

    // Sound Specific
    olc::sound::WaveEngine engine;

    olc::sound::Wave bg_music_memorize;
    olc::sound::Wave bg_music_search;

    bool bMenu;
    bool bMemorize; // is gameplay right now in the memorize mode or nt the find and find the exit mode?
    bool bRemember;

    bool bLight;
    bool bFreeze;
    bool bTransitionFromMenu;
    bool bTransitionToLevel;
    bool bTransitionFromLevel;
    float transCounter;
    bool bFinished;
    bool bText;

    float textSize;
    float fade;
    float t_title_x = 75 * ScreenWidth() / 640;
    float t_title_y = 236 * ScreenHeight() / 480;

    olc::Sprite *sprMenuBG;
    olc::Decal *decMenuBG;
    vec2d bg_menu_scale;

    olc::Sprite *sprInput;
    olc::Decal *decInput;
    vec2d bg_input_scale;
    float inputCounter;

    float newZoom;
    vec2d lookTarget;

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

                vec2d topLeft_projected = c_camera.Project({(float)x_transformed, (float)y_transformed});
                float newPathW = (float)m_nPathWidth * c_camera.zoom;
                float newWallW = (float)m_nWallWidth * c_camera.zoom;
                float newTileW = newPathW + newWallW;

                vec2d scale = {newPathW / decFloor[0]->sprite->width, newPathW / decFloor[0]->sprite->height};
                vec2d scale_wall = {newWallW / decFloor[0]->sprite->width, newWallW / decFloor[0]->sprite->height};
                olc::Decal *decal = (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_START) ? decFloor[1] : (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_FINISH) ? decFloor[2]
                                                                                                                                                                           : decFloor[0];

                float r_vision2 = p_player.visionRadius * p_player.visionRadius;
                float distance2 = (p_player.pos - center).GetLengthSqared();

                if (topLeft_projected.x > -newTileW && topLeft_projected.y > -newTileW &&
                    topLeft_projected.x < ScreenWidth() && topLeft_projected.y < ScreenHeight() &&
                    (bLight || (distance2 < r_vision2)))
                {
                    DrawDecal({topLeft_projected.x, topLeft_projected.y}, decal, {scale.x, scale.y});
                    // FillRect(topLeft_projected.x, topLeft_projected.y, newPathW, newPathW, color);

                    if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_SOUTH)
                        DrawDecal({topLeft_projected.x, topLeft_projected.y + newPathW}, decal, {scale.x, scale_wall.y});
                    if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_EAST)
                        DrawDecal({topLeft_projected.x + newPathW, topLeft_projected.y}, decal, {scale_wall.x, scale.y});

                    if (decal == decFloor[1])
                    {
                        vec2d scaleStart = {newPathW * 3.0f / decStart->sprite->width, newPathW * 3.0f / decStart->sprite->height};
                        DrawDecal({topLeft_projected.x - newPathW * 1.5f, topLeft_projected.y + newPathW * 2.0f}, decStart, {scaleStart.x, scaleStart.y});
                    }
                    else if (decal == decFloor[2])
                    {
                        vec2d scaleFinish = {newPathW * 3.0f / decFinish->sprite->width, newPathW * 3.0f / decFinish->sprite->height};
                        DrawDecal({topLeft_projected.x - newPathW * 1.5f, topLeft_projected.y - newPathW * 2.0f}, decFinish, {scaleFinish.x, scaleFinish.y});
                    }

                    /*// Draw passageways between cells
                    for (float p = 0.0f; p < newPathW; p++)
                    {
                        for (float k = 0.0f; k < newPathW; k++)
                        {
                            if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_SOUTH)
                                Draw(topLeft_projected.x + p, topLeft_projected.y + newPathW + k, color); // Draw South Passage

                            if (m_maze.m_nMaze[y * m_maze.m_nMazeWidth + x] & CELL_PATH_EAST)
                                Draw(topLeft_projected.x + newPathW + k, topLeft_projected.y + p, color); // Draw East Passage
                        }
                    }*/
                }
            }
        }
    }

    void DrawPlayer(player p, camera c_camera, olc::Decal *decFading, vec2d spriteScale)
    {

        vec2d p_draw_pos = p.pos;
        /*
                // player pos in maze space m_nMazeWidth x m_nMazeHeight
                vec2d p_maze_space = { p.pos.x / m_nTileWidth, p.pos.y / m_nTileWidth };

                // closest corner of the cell
                vec2d closestCorner = { roundf(p_maze_space.x), roundf(p_maze_space.y) };
                if (m_maze[closestCorner.y * m_nMazeWidth + closestCorner.x] & CELL_PATH_EAST)

                vec2d offset = (closestCorner - p_maze_space) * m_nTileWidth;

                int sign = (offset.x) ? 1 : -1;
                if (abs(offset.x) < p.radius + m_nWallWidth * 0.5f)
                {
                    offset.x = -sign * (p.radius + m_nWallWidth * 0.5f);
                    p_draw_pos.x = closestCorner.x * m_nTileWidth + offset.x;
                }

                sign = (offset.y) ? 1 : -1;
                if (abs(offset.y) < p.radius + m_nWallWidth * 0.5f)
                {
                    offset.y = -sign * (p.radius + m_nWallWidth * 0.5f);
                    p_draw_pos.y = closestCorner.y * m_nTileWidth + offset.y;
                }*/

        vec2d p_pos_projected = c_camera.Project(p_draw_pos);
        float p_r_projected = p.radius * c_camera.zoom;
        vec2d scale = {p_r_projected * 2.0f / decPlayer->sprite->width, p_r_projected * 2.0f / decPlayer->sprite->height};
        if (p_pos_projected.x >= 0 && p_pos_projected.x < ScreenWidth() &&
            p_pos_projected.y >= 0 && p_pos_projected.y < ScreenHeight())
        {
            // FillCircle(p_pos_projected.x, p_pos_projected.y, p_r_projected, p.color);
            // DrawCircle(p_pos_projected.x, p_pos_projected.y, p_r_projected, olc::WHITE);
            DrawDecal({p_pos_projected.x - p_r_projected, p_pos_projected.y - p_r_projected}, decPlayer, {scale.x, scale.y});
        }

        if (!bLight)
        {
            // Width and hight of the sprite relative in screen space
            float darkW = spriteScale.x * (float)ScreenWidth();
            float darkH = spriteScale.y * (float)ScreenHeight();

            // top left of sprite in screen space
            vec2d origin = {c_camera.center.x - darkW * 0.5f, c_camera.center.y - darkH * 0.5f};

            // top left of sprite in camera space
            vec2d origin_projected = c_camera.Project(origin);

            // sprite width and height in camera space
            vec2d scale = {darkW / decFading->sprite->width * c_camera.zoom, darkH / decFading->sprite->height * c_camera.zoom};

            DrawDecal({origin_projected.x, origin_projected.y}, decFading, {scale.x, scale.y});
        }
    }

public:
    MMM()
    {
        sAppName = "Memory Maze Man!";
    }

protected:
    bool OnUserCreate() override
    {
        m_nMazeWidth = 9;
        m_nMazeHeight = 9;
        m_maze.GenerateMaze(m_nMazeWidth, m_nMazeHeight);

        m_nPathWidth = 30;
        m_nWallWidth = 2;
        m_nTileWidth = m_nPathWidth + m_nWallWidth;

        m_maze.wallColor = olc::Pixel(10, 10, 10);
        sprFloor[0] = new olc::Sprite("MMM_floor_v0.png"); // regular tile
        decFloor[0] = new olc::Decal(sprFloor[0]);
        sprFloor[1] = new olc::Sprite("MMM_floor_start_finish_v0.png"); // start tile
        decFloor[1] = new olc::Decal(sprFloor[1]);
        sprFloor[2] = new olc::Sprite("MMM_floor_start_finish_v0.png"); // finish tile
        decFloor[2] = new olc::Decal(sprFloor[2]);

        sprGameBG = new olc::Sprite("MMM_bg_v0.1.png");
        decGameBG = new olc::Decal(sprGameBG);
        bg_game_scale.x = (float)ScreenWidth() / sprGameBG->width;
        bg_game_scale.y = (float)ScreenHeight() / sprGameBG->height;

        sprStart = new olc::Sprite("MMM_start_v0.png");
        decStart = new olc::Decal(sprStart);
        sprFinish = new olc::Sprite("MMM_finish_v0.png");
        decFinish = new olc::Decal(sprFinish);

        m_maze.floorColor = olc::Pixel(100, 20, 100);
        m_maze.startColor = olc::WHITE;
        m_maze.finishColor = olc::WHITE;

        p_player.pos = {((float)m_maze.start_x + 0.5f) * m_nTileWidth, ((float)m_maze.start_y + 0.25f) * m_nTileWidth};
        p_player.radius = 2.0f;
        p_player.speed = 160.0f;
        sprPlayer = new olc::Sprite("MMM_player_v0.1.png");
        decPlayer = new olc::Decal(sprPlayer);
        p_player.color = m_maze.wallColor;

        zoomNull = 1.0f;
        zoomSearch = 2.0f;
        c_camera.Construct(&p_player.pos, ScreenWidth(), ScreenHeight(), zoomNull, 0.5f);
        newZoom = zoomNull;
        lookTarget = {(float)m_nMazeWidth * 0.5f * m_nTileWidth, (float)m_nMazeHeight * 0.5f * m_nTileWidth};

        sprFading = new olc::Sprite("MMM_darken_v0.1.png");
        decFading = new olc::Decal(sprFading);
        lightScaleNormal = 0.5f;
        lightScaleSmall = 0.7f;
        p_player.visionRadius = (float)ScreenWidth() / zoomSearch * 0.75f;

        bg_music_search.LoadAudioWaveform("MMM_OST_v0.wav");
        engine.InitialiseAudio(44100, 2);
        engine.SetOutputVolume(0.8f);

        engine.PlayWaveform(&bg_music_search, true);

        bMenu = true;
        bMemorize = false;
        bRemember = false;

        bTransitionFromMenu = false;
        bTransitionToLevel = false;
        bTransitionFromLevel = false;
        bText = false;
        bLight = true;
        bFreeze = false;

        textSize = 2.0f;
        fade = 1.0f;

        sprMenuBG = new olc::Sprite("MMM_bg_menu_v0.png");
        decMenuBG = new olc::Decal(sprMenuBG);
        bg_menu_scale.x = (float)ScreenWidth() / sprMenuBG->width;
        bg_menu_scale.y = (float)ScreenHeight() / sprMenuBG->height;

        sprInput = new olc::Sprite("MMM_text_v0.png");
        decInput = new olc::Decal(sprInput);
        bg_input_scale.x = (float)ScreenWidth() / sprInput->width;
        bg_input_scale.y = (float)ScreenHeight() / sprInput->height;
        inputCounter = 7.0f;
        transCounter = 1.0f;

        bFinished = false;
        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        //// CALCULATION ////
        ///// INPUT/////

        vec2d dir = {0, 0};

        // newZoom = (float)ScreenWidth() / ((m_nMazeWidth + 2) * m_nTileWidth);
        // translation input
        if (GetKey(olc::Key::W).bHeld || GetKey(olc::Key::UP).bHeld)
            dir.y -= 1.0f;
        if (GetKey(olc::Key::S).bHeld || GetKey(olc::Key::DOWN).bHeld)
            dir.y += 1.0f;
        if (GetKey(olc::Key::A).bHeld || GetKey(olc::Key::LEFT).bHeld)
            dir.x -= 1.0f;
        if (GetKey(olc::Key::D).bHeld || GetKey(olc::Key::RIGHT).bHeld)
            dir.x += 1.0f;

        if (bTransitionFromMenu)
        {
            fade -= 1.0f * fElapsedTime;
            if (fade <= 0.0f)
            {
                bMenu = false;
                bTransitionFromMenu = false;
                bText = true;
                bTransitionToLevel = true;
            }
        }
        else if (bTransitionToLevel)
        {
            transCounter -= 1.0f * fElapsedTime;
            cout << transCounter << endl;
            if (transCounter <= 0.0f)
            {
                transCounter = 1.0f;
                bTransitionToLevel = false;
                bMemorize = true;
                newZoom = (float)ScreenWidth() / ((m_nMazeWidth + 2) * m_nTileWidth);
                lookTarget = {m_nMazeWidth * 0.5f * m_nTileWidth, m_nMazeHeight * 0.5f * m_nTileWidth};
                c_camera.target = &lookTarget;
            }
        }
        else if (bText)
        {
            inputCounter -= 1.0f * fElapsedTime;
            if (inputCounter <= 0.0f || bRemember)
                bText = false;
        }
        else if (bTransitionFromLevel)
        {
            transCounter -= 1.0f * fElapsedTime;
            cout << transCounter << endl;
            if (transCounter <= 0.0f)
            {
                transCounter = 1.0f;
                bTransitionFromLevel = false;
                bMemorize = true;
            }
        }

        if (!bFinished)
        {
            if (bMenu)
            {
                newZoom = zoomNull;
                if (GetKey(olc::Key::SPACE).bPressed)
                    bTransitionFromMenu = true;
            }
            else if (bMemorize)
            {
                if (GetKey(olc::Key::EQUALS).bHeld)
                {
                    newZoom += 1.0f * fElapsedTime;
                }
                if (GetKey(olc::Key::MINUS).bHeld)
                    newZoom -= 1.0f * fElapsedTime;

                if (newZoom <= 0.1f)
                    newZoom = 0.1f;

                if (GetKey(olc::Key::R).bPressed && !bFreeze)
                {
                    bMemorize = false;
                    bRemember = true;
                    newZoom = zoomSearch;
                    c_camera.target = &p_player.pos;
                }

                lookTarget += dir * p_player.speed * fElapsedTime;
            }
            else
            {
                newZoom = zoomSearch;

                // ready to start

                if (bLight)
                {
                    bLight = false;
                    p_player.pos = {((float)m_maze.start_x + 0.5f) * m_nTileWidth, ((float)m_maze.start_y + 0.5f) * m_nTileWidth};
                }

                if ((int)(p_player.pos.x / (float)m_nTileWidth) == m_maze.finish_x &&
                    (int)(p_player.pos.y / (float)m_nTileWidth) == m_maze.finish_y)
                {
                    bLight = true;
                    // bMemorize = true;
                    bRemember = false;
                    bTransitionToLevel = true;
                    m_nMazeWidth += 2;
                    m_nMazeHeight += 2;
                    if (m_nMazeHeight >= 16.0f) bFinished = true;
                    m_maze.GenerateMaze(m_nMazeWidth, m_nMazeHeight);
                    p_player.pos = {((float)m_maze.start_x + 0.5f) * m_nTileWidth, ((float)m_maze.start_y + 0.5f) * m_nTileWidth};
                    bFreeze = true;

                    newZoom = (float)ScreenWidth() / ((m_nMazeWidth + 2) * m_nTileWidth);
                    cout << c_camera.zoom << endl;
                    lookTarget = {m_nMazeWidth * 0.5f * m_nTileWidth, m_nMazeHeight * 0.5f * m_nTileWidth};
                    c_camera.target = &lookTarget;
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
            }
        }
        cout << m_nMazeWidth << endl;
        bFinished = (m_nMazeWidth >= 17);

        c_camera.Update(newZoom);

        ///////////////

        ////DRAWING////
        if (bFinished)
        {
            DrawDecal({0, 0}, decGameBG, {bg_game_scale.x, bg_game_scale.y});
            DrawStringDecal({(float)ScreenWidth() * 0.1f, (float)ScreenHeight() * 0.45f}, "Thank you for playing", olc::WHITE, {textSize, textSize});
            DrawStringDecal({(float)ScreenWidth() * 0.1f, (float)ScreenHeight() * 0.55f}, "MEMORY MAZE MAN!", olc::WHITE, {textSize, textSize});
        } 
        else if (bMenu)
        {
            DrawDecal({0, 0}, decMenuBG, {bg_menu_scale.x, bg_menu_scale.y});
            olc::Pixel textColor = olc::Pixel(255.0f * fade, 255.0f * fade, 255.0f * fade);
            DrawDecal({0, 0}, decMenuBG, {bg_menu_scale.x, bg_menu_scale.y}, textColor);
            // DrawString({(int32_t)t_title_x, (int32_t)t_title_y}, "MEMORY MAZE MAN!", textColor, textSize);

            DrawStringDecal({(float)ScreenWidth() * 0.1f, (float)ScreenHeight() * 0.9f}, "press SPACE to continue", textColor, {textSize, textSize});
        }
        else if (bTransitionFromMenu)
        {
            DrawDecal({0, 0}, decGameBG, {bg_game_scale.x, bg_game_scale.y});
        }
        else if (bTransitionToLevel)
        {
            DrawDecal({0, 0}, decGameBG, {bg_game_scale.x, bg_game_scale.y});
            switch (m_nMazeWidth)
            {
            case 9:
                DrawStringDecal({(float)ScreenWidth() * 0.45f, (float)ScreenHeight() * 0.45f}, "9X9", olc::WHITE, {textSize, textSize});
                break;
            case 11:
                DrawStringDecal({(float)ScreenWidth() * 0.45f, (float)ScreenHeight() * 0.45f}, "11X11", olc::WHITE, {textSize, textSize});
                break;
            case 13:
                DrawStringDecal({(float)ScreenWidth() * 0.45f, (float)ScreenHeight() * 0.45f}, "13X13", olc::WHITE, {textSize, textSize});
                break;
            case 15:
                DrawStringDecal({(float)ScreenWidth() * 0.45f, (float)ScreenHeight() * 0.45f}, "FINAL BOSS", olc::WHITE, {textSize, textSize});
                break;

            default:
                break;
            }
        }
        else if (bMemorize)
        {
            // Clear(m_maze.wallColor);
            DrawDecal({0, 0}, decGameBG, {bg_game_scale.x, bg_game_scale.y});

            // draw maze
            DrawMaze(m_maze, p_player, true, c_camera);
            // draw player
            // DrawPlayer(p_player, c_camera, decFading, {lightScaleNormal, lightScaleNormal});
            if (bText)
            {
                vec2d scaleInput = {(float)ScreenWidth() / decInput->sprite->width * 0.8f, (float)ScreenHeight() / decInput->sprite->height * 0.8f};
                DrawDecal({(float)ScreenWidth() * 0.1f, (float)ScreenHeight() * 0.1f}, decInput, {scaleInput.x, scaleInput.y});
            }
            else
            {
                DrawStringDecal({(float)ScreenWidth() * 0.1f, (float)ScreenHeight() * 0.9f}, "press R to start", olc::WHITE, {textSize, textSize});
            }
        }
        else if (bRemember)
        {
            // Clear(m_maze.wallColor);
            DrawDecal({0, 0}, decGameBG, {bg_game_scale.x, bg_game_scale.y});

            // draw maze
            DrawMaze(m_maze, p_player, false, c_camera);
            // draw player

            DrawPlayer(p_player, c_camera, decFading, {lightScaleSmall, lightScaleSmall});
        }
        ////////////////

        return true;
    }
};

int main()
{
    // Seed random number generator
    srand(clock());

    MMM demo;
    if (demo.Construct(578, 578, 1, 1, false))
        demo.Start();

    return 0;
}