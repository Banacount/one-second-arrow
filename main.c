#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct { 
    bool isExist;
    int directionState; //Up - 0, Down - 1, Right - 2, Left - 3.
    Rectangle rect;
} Arrow;

//Global variables
#define MAIN_GAME 0 
#define GAME_OVER 1 
//Pages
int PageSelector = 0;

//Functions
Texture2D getTextureFrImg(char * file_path);
void displayArrows(Arrow *arrow_list, int arrow_count, Texture2D * texture, Rectangle arrow_source, float spacing, bool *right_flow);
void displayLives(int * lives, float spacing);

int main ()
{
    const int Scrwidth = 1280, Scrheight = 720;
    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(Scrwidth, Scrheight, "One Second Arrow");
    InitAudioDevice();

    int fps = 60;
    SetTargetFPS(fps);

    //Images
    Texture2D arrowTextures[4];
    arrowTextures[0] = getTextureFrImg("assets/images/arrow_up.png");
    arrowTextures[1] = getTextureFrImg("assets/images/arrow_down.png");
    arrowTextures[2] = getTextureFrImg("assets/images/arrow_right.png");
    arrowTextures[3] = getTextureFrImg("assets/images/arrow_left.png");
    Texture2D arrow_flow = getTextureFrImg("assets/images/arrow_flow.png");

    //Sfxs
    Sound scoreSfx = LoadSound("./assets/sfx/score_hit.wav");
    Sound hitSfx = LoadSound("./assets/sfx/hit.wav");
    Sound mistakeSfx = LoadSound("./assets/sfx/mistake.wav");

    //Arrows default value
    Vector2 ArrowsOrig = {0,0};
    Rectangle ArrowsSourceRec = {0, 0, 181, 181}; 
    Rectangle ArrowsRec = {0, 0, 96, 96}; float arrowScale = 96.00;

    //Fonts
    Font mainFont = LoadFontEx("assets/p2p-regular.ttf", 264, 0, 250);

    //Game variables
    int total_score = 0; bool scored = true;
    int arrow_hit = -1, lives = 5;
    bool rightFlow = true; int flowNum = 0;
    Arrow arrows[11]; int arrowCount = 0;
    //Rotation animation variables for flow-arrow
    float flowArrowRotation = 0.00; float rotateSpeed = 4.00;
    //Hit delay mechanic variables
    float hitDelay = 3.5; float captHitDelay = 0.00;
    bool resetHitDelay = true;
    float bgOpacChange = 0.00;

    while (!WindowShouldClose())
    {
        switch(PageSelector)
        {
            case MAIN_GAME:
            //Game Logics
            if(IsKeyPressed(KEY_UP)) arrow_hit = 0;
            else if(IsKeyPressed(KEY_DOWN)) arrow_hit = 1;
            else if(IsKeyPressed(KEY_RIGHT)) arrow_hit = 2;
            else if(IsKeyPressed(KEY_LEFT)) arrow_hit = 3;
            //Controls the flow of arrows
            if(arrow_hit != -1){
                bool isRight = false;
                if(arrows[flowNum].directionState == arrow_hit){
                    arrows[flowNum].isExist = false;
                    flowNum++;
                    isRight = true;
                    captHitDelay = GetTime();
                    //Triggers scored if flowNum and arrowCount is equal
                    if(flowNum >= arrowCount){
                        flowNum = 0;
                        total_score++;
                            rightFlow = !rightFlow;
                        PlaySound(scoreSfx);
                        scored = true;
                    } else {
                        PlaySound(hitSfx);
                    }
                }
                if(!isRight){
                    PlaySound(mistakeSfx);
                    lives--;
                }
                resetHitDelay = false;
                arrow_hit = -1;
            }
            //Delay logics here so users will have a hard time frfr hehe
            bgOpacChange = (GetTime()-captHitDelay) / hitDelay;
            if(resetHitDelay) captHitDelay = GetTime();
            if(GetTime()-captHitDelay >= hitDelay || lives <= 0){
                PageSelector = GAME_OVER;
            }
            //Animations right here folks haha
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
            //Scale animation on intro of arrows
            if(ArrowsRec.height < arrowScale){
                float timeTo = 60 * 0.3;//Sec
                float speed = (arrowScale / timeTo) + 1;
                ArrowsRec.height += speed;
                for(int a = 0; a < arrowCount; a++) arrows[a].rect = ArrowsRec;
                //printf("Time: %.2f\n", GetTime());
            }
            //Flow arrow rotate animation
            if(rightFlow && flowArrowRotation > 0){
                flowArrowRotation -= rotateSpeed;
            } else if(!rightFlow && flowArrowRotation < 180) {
                flowArrowRotation += rotateSpeed;
            }
            break;

            case GAME_OVER:
            break;

            default: break;
        }

        BeginDrawing();
        //Universal or Global UI
        ClearBackground(WHITE);

        switch(PageSelector)
        {
            //MAIN_GAME MENU
            case MAIN_GAME:
            DrawRectangleRec((Rectangle){0, 0, GetScreenWidth(),GetScreenHeight()}, (Color){255, 57, 24, (int)(255*bgOpacChange)});
            displayArrows(arrows, arrowCount, 
                          arrowTextures, ArrowsSourceRec, 10, &rightFlow);
            //Flow arrow
            float scale = 0.4;
            Vector2 srcSize = {541, 111};
            Vector2 displaySize = {srcSize.x*scale, srcSize.y*scale};
            DrawTexturePro(arrow_flow, 
                  (Rectangle){0, 0, srcSize.x, srcSize.y}, 
                    (Rectangle){GetScreenWidth()/2.0f, GetScreenHeight()*0.84, displaySize.x, displaySize.y},
                  (Vector2){(displaySize.x)/2, (displaySize.y)/2}, flowArrowRotation, WHITE);

            //Main UI
            DrawTextEx(mainFont, TextFormat("Score: %d", total_score),
                       (Vector2){20, 20}, 25, 0, BLACK);
            displayLives(&lives, 10);
            break;

            //GAME_OVER MENU
            case GAME_OVER: ;
            float fontSize = GetScreenWidth()*0.05;
            Vector2 textSize = MeasureTextEx(mainFont, "Game Over!", fontSize, 0);

            DrawTextEx(mainFont, "Game Over!", 
                       (Vector2){(GetScreenWidth()-textSize.x)/2, ((GetScreenHeight()-textSize.y)/2)-100},
                       fontSize, 0, BLACK);

            fontSize = GetScreenWidth()*0.02;
            textSize = MeasureTextEx(mainFont, "Your score is %d.            [Esc to exit]", fontSize, 0);
            DrawTextEx(mainFont, TextFormat("Your score is %d.            [Esc to exit]", total_score), 
                       (Vector2){(GetScreenWidth()-textSize.x)/2, ((GetScreenHeight()*0.6))},
                       fontSize, 0, BLACK);
            break;

            default: break;
        }

        EndDrawing();
    }

    //End program and unload loeaded shitters
    for(int i = 0; i < 4; i++) UnloadTexture(arrowTextures[i]);
    CloseAudioDevice();
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

void displayArrows (Arrow *arrow_list, int arrow_count, Texture2D *textures, Rectangle arrow_source, float spacing, bool *right_flow)
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
        if(!*right_flow) break;
        if(!arrow_list[i].isExist) continue;
        Rectangle arrowRect = arrow_list[i].rect;
        //Centered x and y calculation
        arrowRect.x = startX;
        arrowRect.y = (GetScreenHeight() - arrowRect.height) / 2.0f;

        if(arrow_list[i].directionState < 0 || arrow_list[i].directionState > 3) arrow_list[i].directionState = 0;
        DrawTexturePro(textures[arrow_list[i].directionState], arrow_source, arrowRect,(Vector2){0, 0}, 0, WHITE);
        startX += arrowRect.width + spacing;
    }

    //Reverse the arrows hehe
    for(int i = arrow_count-1; i >= 0; i--){
        if(*right_flow) break;
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

void displayLives (int * lives, float spacing)
{
    float stackX= 0.00, live_size = GetScreenWidth()*0.05;
    for(int i = 0; i < *lives; i++){
        Rectangle life_rect = {(GetScreenWidth()-(live_size+spacing))-(stackX), 
                               (GetScreenHeight()-(live_size+spacing)), live_size, live_size};
        DrawRectangleRec(life_rect, RED);
        stackX += live_size+spacing;
    }
}
