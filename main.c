#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct { 
    bool isExist;
    int directionState; //Up - 0, Down - 1, Right - 2, Left - 3.
    Rectangle rect;
} Arrow;

//Global variables

//Functions
Texture2D getTextureFrImg(char * file_path);
void displayArrows(Arrow *arrow_list, int arrow_count, Texture2D * texture, Rectangle arrow_source, float spacing);

int main ()
{
    const int Scrwidth = 1280, Scrheight = 720;
    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(Scrwidth, Scrheight, "One Second Arrow");

    int fps = 60;
    SetTargetFPS(fps);

    //Images
    Texture2D arrowTextures[4];
    arrowTextures[0] = getTextureFrImg("assets/images/arrow_up.png");
    arrowTextures[1] = getTextureFrImg("assets/images/arrow_down.png");
    arrowTextures[2] = getTextureFrImg("assets/images/arrow_right.png");
    arrowTextures[3] = getTextureFrImg("assets/images/arrow_left.png");

    //Arrows default value
    Vector2 ArrowsOrig = {0,0};
    Rectangle ArrowsSourceRec = {0, 0, 181, 181}; 
    Rectangle ArrowsRec = {0, 0, 96, 96}; float arrowScale = 96.00;

    //Fonts
    Font mainFont = LoadFontEx("assets/sekuya.ttf", 64, 0, 250);

    //Game variables
    int total_score = 0; bool scored = true;
    int arrow_hit = -1;
    bool rightFlow = true; int flowNum = 0;
    Arrow arrows[11]; int arrowCount = 0;
    int test1 = 0.0;
    while (!WindowShouldClose())
    {
        //Game Logics
        if(IsKeyPressed(KEY_UP)) arrow_hit = 0;
        else if(IsKeyPressed(KEY_DOWN)) arrow_hit = 1;
        else if(IsKeyPressed(KEY_RIGHT)) arrow_hit = 2;
        else if(IsKeyPressed(KEY_LEFT)) arrow_hit = 3;

        //Controls the flow of arrows
        if(arrow_hit != -1){
            if(arrows[flowNum].directionState == arrow_hit){
                arrows[flowNum].isExist = false;
                flowNum++;
                //Triggers scored if flowNum and arrowCount is equal
                if(flowNum >= arrowCount){
                    flowNum = 0;
                    total_score++;
                    scored = true;
                }
            }
            arrow_hit = -1;
        }

        //Generate arrows after flow done
        if(scored){
            arrowCount = 0;
            ArrowsRec.height = 2;
            //Spawn some arrows yey
            int spawnRate = (int)(total_score/10) + 1;
            if(spawnRate >= 8) spawnRate = 8;
            for(int s = 0; s < spawnRate; s++){
                arrows[arrowCount].isExist = true;
                arrows[arrowCount].directionState = GetRandomValue(0, 3);
                arrows[arrowCount].rect = ArrowsRec;
                arrowCount++;
            }
            scored = false;
        }

        //Animation lol
        if(ArrowsRec.height < arrowScale){
            float timeTo = 60 * 0.3;//Sec
            float speed = (arrowScale / timeTo) + 1;
            ArrowsRec.height += speed;
            for(int a = 0; a < arrowCount; a++) arrows[a].rect = ArrowsRec;
            //printf("Time: %.2f\n", GetTime());
        }

        BeginDrawing();

            ClearBackground(WHITE);
            DrawTextEx(mainFont, TextFormat("Score: %d", total_score), 
                       (Vector2){20, 20}, 25, 0, BLACK);
            displayArrows(arrows, arrowCount, 
                          arrowTextures, ArrowsSourceRec, 10);
//            DrawTexturePro(arrowUp, ArrowsSourceRec, 
//                           (Rectangle){(GetScreenWidth() - ArrowsRec.width) / 2.0f,
//                           (GetScreenHeight() - ArrowsRec.height) / 2.0f, 
//                           ArrowsRec.width, 
//                           ArrowsRec.height}, 
//                           (Vector2){0, 0}, 0, WHITE);

        EndDrawing();
    }

    for(int i = 0; i < 4; i++) UnloadTexture(arrowTextures[i]);
    CloseWindow();
    return 0;
}

//Function definitions
Texture2D getTextureFrImg (char * file_path)
{
    Image img = LoadImage(file_path);
    Texture2D texture = LoadTextureFromImage(img);
    UnloadImage(img);
    return texture;
}

void displayArrows (Arrow *arrow_list, int arrow_count, Texture2D *textures, Rectangle arrow_source, float spacing)
{
    int allCount = 0; float allX = 0;
    for(int i = 0; i < arrow_count; i++){
        if(!arrow_list[i].isExist) continue;
        allX += arrow_list[i].rect.width;
        allCount++;
    }

    if(allCount > 1) allX += (allCount - 1) * spacing;
    float startX = (GetScreenWidth() - allX) / 2.0f;
    for(int i = 0; i < arrow_count; i++){
        if(!arrow_list[i].isExist) continue;
        Rectangle arrowRect = arrow_list[i].rect;
        //Centered x and y calculation
        arrowRect.x = startX;
        arrowRect.y = (GetScreenHeight() - arrowRect.height) / 2.0f;

        if(arrow_list[i].directionState < 0 || arrow_list[i].directionState > 3) arrow_list[i].directionState = 0;
        DrawTexturePro(textures[arrow_list[i].directionState], arrow_source, arrowRect,(Vector2){0, 0}, 0, WHITE);
        startX += arrowRect.width + spacing;
    }
}
