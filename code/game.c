
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include "utils.h"
#include "thread.h"

typedef enum 
{
	
	PLAYER_IDLE,
	PLAYER_RUN,
	PLAYER_ATTACK

}PlayerState;

enum TextureID
{
	TEXTURE_IDLE,
	TEXTURE_RUN,
	TEXTURE_ATTACK,
	TEXTURE_COUNT

};

enum AnimationID
{
	ANIMATION_IDLE,
	ANIMATION_RUN,
	ANIMATION_COUNT

};

typedef struct BoundingReigonImg 
{
	int Minx;
	int Miny;
	int MaxX;
	int MaxY;
	int Width;
	int Height;

}BoundingReigonImg;

typedef struct AnimationState
{
	
	int NumOfFrames;
	int currentFrame;
	float AnimationFrameSpeed;
	float TimeAccumulator;
	
}AnimationState;

struct Arena
{
	void *base;
	size_t used;

};


int TileMap[19][25] =
{
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

enum TileTexture
{
	RoadCementBegin,
	RoadCementBeginTop,
	RoadCementMiddle,
	RoadCementTop,
	
};

enum GameState
{
	Game,
	Editor,
	TexturePacking,
	Count
		
};

typedef struct BoundingReigonGroup
{
	BoundingReigonImg *area;
	int count;

}BoundingReigonGroup;

Texture2D CreateTextureFromImage(const char* ImgFile)
{
	
	Image img = {};
	img = LoadImage(ImgFile);
	Texture2D Texture = LoadTextureFromImage(img);
	UnloadImage(img);
	
	return Texture;

}

inline int RoundToInt(float value) 
{

    return (int)(value + 0.5f);

}

void UpdateFrameIndexAndCurrentIndex(AnimationState *State)
{
	
			if(State->TimeAccumulator >= State->AnimationFrameSpeed)
			{

				State->TimeAccumulator = 0;
				State->currentFrame = (State->currentFrame + 1) % State->NumOfFrames;
				//return State.currentFrame;
				
			}
		 

}



void drawPLayer(Texture2D Texture,int FrameWidth,int FrameHeight,float Xpos,float Ypos,AnimationState State,bool flip)
{
if (flip==1)
{

	DrawTexturePro(Texture,(Rectangle){FrameWidth * State.currentFrame, 0, -FrameWidth, FrameHeight},(Rectangle){RoundToInt(Xpos) - (FrameWidth/2) ,RoundToInt(Ypos) - (FrameHeight/2), FrameWidth, FrameHeight},(Vector2){0, 0},0.0f,WHITE);

}
else
{

	DrawTexturePro(Texture,(Rectangle){ FrameWidth * State.currentFrame, 0, FrameWidth, FrameHeight},(Rectangle){RoundToInt(Xpos) - (FrameWidth/2) ,RoundToInt(Ypos) - (FrameHeight/2), FrameWidth, FrameHeight},(Vector2){0, 0},0.0f,WHITE);

}
}


void CalculateNonAlphaBoxInsideImage(char *ImagePath ,int FrameWidth,int FrameHeight ,BoundingReigonGroup *group,void **pointer)
{
	// only use if image has a bunch of padding 

	Image Myimage = LoadImage(ImagePath);
	Color *pixels = LoadImageColors(Myimage);

	int NumofFramesinsideImage = Myimage.width / FrameWidth;

	group->area = (BoundingReigonImg*)*pointer;
	u64 used = sizeof(BoundingReigonImg) * NumofFramesinsideImage;
	*pointer = (u8*)(*pointer) + used;

	group->count = NumofFramesinsideImage;
	
		for(int i = 0; i < NumofFramesinsideImage; i++)
		{
			int maxX = 0;
			int maxY = 0;
			int minX = FrameWidth;
			int minY = FrameHeight;
			
			for (int y = 0; y < 80; y++)
			{
				for (int x = 0; x < 120; x++)
				{
					Color c = pixels[(y*Myimage.width) + ((i*FrameWidth))+ x];
					
					if(c.a > 0)
					{

						if(x < minX)
						{ 
							minX = x;  
						}
						if (y < minY)
						{
							minY = y;
						}
						if (x > maxX)
						{
							maxX = x;
						}
						if (y > maxY)
						{
							maxY = y;
						}

					}
				}
			}
			
			BoundingReigonImg *box = group->area;
			box[i].Minx = minX;
			box[i].MaxX = maxX;
			box[i].Miny = minY;
			box[i].MaxY = maxY;
			box[i].Width =  maxX - minX + 1;
			box[i].Height = maxY - minY + 1;

			

		}		
			UnloadImageColors(pixels);			
			UnloadImage(Myimage);
}


// void PlayerAnimateWithBoundingBox(BoundingReigonGroup *group , PlayerState AnimeType,int Xpos,int Ypos , int FrameWidth,int FrameHeight , Texture2D *Tex , AnimationState *animations)
// {

// 		switch(AnimeType)
// 			{
				
// 				case PLAYER_IDLE: 
// 				BoundingReigonImg *boxIdle = group[0].area;
// 				DrawPlayerBoundingBox(Xpos,Ypos,boxIdle,animations[ANIMATION_IDLE],false);
// 				drawPLayer(Tex[TEXTURE_IDLE],FrameWidth,FrameHeight,Xpos,Ypos,animations[ANIMATION_IDLE],false);

// 				break;
				
// 				case PLAYER_RUNRIGHT:
// 				//DrawTexturePro(TextureRun ,(Rectangle){(frameWidth * StateRunning.currentFrame) ,0  ,frameWidth ,frameHeight } ,(Rectangle){Xpos,Ypos,120,80}, (Vector2){0,0} , 0.0f, WHITE);
// 				BoundingReigonImg *boxrun = group[1].area;
// 				DrawPlayerBoundingBox(Xpos,Ypos,boxrun,animations[ANIMATION_RUN],false);
// 				drawPLayer(Tex[TEXTURE_RUN],FrameWidth,FrameHeight,Xpos,Ypos,animations[ANIMATION_RUN],false);
				
// 				break;
				
// 				case PLAYER_RUNLEFT:
// 				BoundingReigonImg *boxrunRight = group[1].area;
// 				DrawPlayerBoundingBox(Xpos,Ypos,boxrunRight,animations[ANIMATION_RUN],true);
// 				drawPLayer(Tex[TEXTURE_RUN],FrameWidth,FrameHeight,Xpos,Ypos,animations[ANIMATION_RUN],true);
// 				//DrawTexturePro(TextureRun ,(Rectangle){(frameWidth*StateRunning.currentFrame), 0, -frameWidth ,frameHeight} ,(Rectangle){Xpos-60,Ypos-40,120,80}, (Vector2){0,0} , 0.0f, WHITE);

// 				// TODO i dont think this is right for player Run right think about creating a newspritesheet for right will make it easier or try something else like also a new bounding box for a flipped image this is a bit cmoplicated for me 

// 				break;
// 			}
				
// }



// void PlayerAnimate(PlayerState AnimeType,int Xpos,int Ypos , int FrameWidth,int FrameHeight , Texture2D *Tex , AnimationState *animations)
// {

// 		switch(AnimeType)
// 			{
				
// 				case PLAYER_IDLE: 
// 				drawPLayer(Tex[TEXTURE_IDLE],FrameWidth,FrameHeight,Xpos,Ypos,animations[ANIMATION_IDLE],false);
// 				break;
				
// 				case PLAYER_RUNRIGHT:
// 				drawPLayer(Tex[TEXTURE_RUN],FrameWidth,FrameHeight,Xpos,Ypos,animations[ANIMATION_RUN],false);
// 				break;
				
// 				case PLAYER_RUNLEFT:
// 				drawPLayer(Tex[TEXTURE_RUN],FrameWidth,FrameHeight,Xpos,Ypos,animations[ANIMATION_RUN],true);
// 				break;

// 			}
				
// }


void Animate(int AnimIndex,int Xpos,int Ypos , int FrameWidth,int FrameHeight , Texture2D *Tex , AnimationState *animations,bool isflipped)
{

				
	drawPLayer(Tex[AnimIndex],FrameWidth,FrameHeight,Xpos,Ypos,animations[AnimIndex],isflipped);
							
}














int main(void)
{
	
    InitWindow(800 , 600, "2D Engine");
    SetTargetFPS(60);
			
	const char* Run  = "../Assets/Cool Knight(120x80)/Colour1/NoOutline/120x80_PNGSheets/_Run.png";
	const char* Idle = "../Assets/Cool Knight(120x80)/Colour1/NoOutline/120x80_PNGSheets/_Idle.png";
	const char* Attack = "../Assets/Cool Knight(120x80)/Colour1/NoOutline/120x80_PNGSheets/_Attack.png";
	
	const char* Tileset2 = "../Assets/Final/Tiles.png";
	const char* BackGround = "../Assets/Final/Background_0.png";

	u64 PermanentStorageSize = Megabytes(64);  
	void *MemoryBlock = MemAlloc(PermanentStorageSize);

	u64 TemporaryStorageSize = Gigabytes(2);

	void *base = MemoryBlock;
	size_t used = 0;


	BoundingReigonGroup *group = (BoundingReigonGroup*)MemoryBlock;
	used = sizeof(BoundingReigonGroup) * 3;
	void *pointer = (u8*)base + used;
	
	//BoundingReigonGroup group[3]; 
	
	CalculateNonAlphaBoxInsideImage(Idle,120,80,&group[0],&pointer);
	CalculateNonAlphaBoxInsideImage(Run,120,80,&group[1],&pointer);
	CalculateNonAlphaBoxInsideImage(Attack,120,80,&group[2],&pointer);

	Texture2D PlayerTextures[TEXTURE_COUNT];

 	PlayerTextures[TEXTURE_RUN] = CreateTextureFromImage(Run);
	PlayerTextures[TEXTURE_IDLE] = CreateTextureFromImage(Idle);
	PlayerTextures[TEXTURE_ATTACK]  = CreateTextureFromImage(Attack);



	Texture2D Tiles2 = CreateTextureFromImage(Tileset2);
	Texture2D BackgroundTexture = CreateTextureFromImage(BackGround);
	
	int frameWidth = 120; //TextureRun.width/NumOfFrames;
	int frameHeight = 80; // TextureRun.height;

	// AnimationState StateIdle = {};
	// StateIdle.NumOfFrames = 10;
	// StateIdle.AnimationFrameSpeed = 0.07;
	
	// AnimationState StateRunning = {};
	// StateRunning.NumOfFrames= 10;
	// StateRunning.AnimationFrameSpeed=0.07;

	bool isflipped = false;
	AnimationState Animations[ANIMATION_COUNT] = {};
	Animations[ANIMATION_IDLE].NumOfFrames = 10;
	Animations[ANIMATION_IDLE].AnimationFrameSpeed = 0.07;
	
	Animations[ANIMATION_RUN].NumOfFrames = 10;
	Animations[ANIMATION_RUN].AnimationFrameSpeed= 0.07;

	float FrameDeltaTime2 = 0.0;
	float FrameDeltaTime = 0.0;
	float scrollingX = 0;

	

    while (!WindowShouldClose()) {


		static float Xpos = 0;
		static float Ypos = 0;

		float newposX = Xpos;
		float newPosY = Ypos;


		static enum GameState GameMode = Game;

		PlayerState AnimeState = PLAYER_IDLE;
		

		if(IsKeyPressed(KEY_SPACE))
		{
			
			GameMode = (GameMode+1) % Count;
			
		}
		
		
		if(IsKeyDown(KEY_RIGHT))
		{
			if(GameMode == Editor)
			{
				scrollingX += 1.0f;
			}
			
			newposX += 2.2f;
			AnimeState = PLAYER_RUN;
			isflipped = false;
		}
		
		else if(IsKeyDown(KEY_LEFT))
		{
			if(GameMode == Editor)
			{
				//scrollingX -= 1.0f;
			}
			else
			{
				AnimeState = PLAYER_RUN;
				newposX -= 2.2f;
				isflipped = true;
			}
		}
		
		
		if(IsKeyDown(KEY_UP))
		{
			
			newPosY -= 1.0f;
			
		}
		
		if(IsKeyDown(KEY_DOWN))
		{
			
			newPosY += 1.0f;
			
		}
		
        // Draw		
        BeginDrawing();
		ClearBackground(RAYWHITE);
		

		switch(GameMode)
		{
			
		case Game:

			//DrawTexturePro(BackgroundTexture, (Rectangle){0,0,BackgroundTexture.width,BackgroundTexture.height},(Rectangle){0,0,800,600}, (Vector2){0,0}, 0, RED);
			int Playerheight = 32;//*0.60;
			int PlayerWidth = 32; 
			DrawRectangle(RoundToInt(Xpos - (32/2)), RoundToInt(Ypos - (32/2)), 32, 32, BLACK);

			int NumOfTilesWidth = 800/32;
			int NumOfTilesHeight = 600/32;
			int NumOfTilesInTextureRow = Tiles2.width / 32;
			//NumOfTilesHeight+=1;
			
			for(int j=0; j<NumOfTilesHeight; j++)
			{
				for(int i=0; i<NumOfTilesWidth; i++)
				{
					
					int value = TileMap[j][i];
					
					int y = ( value /  NumOfTilesInTextureRow); // iD WILL Start from zero but not the tiles
					
					
					int x = (value - (y * NumOfTilesInTextureRow));
 
					x *= 32;
					y *= 32;

					Rectangle SourceRectFromID = {x , y , 32 ,32};
					Rectangle DestFromMap = {(i*32),j*32 , 32,32};  

					DrawTexturePro(Tiles2 ,SourceRectFromID,DestFromMap,(Vector2){0,0},0,WHITE);

		
					
				}
				
			}

			Animations[ANIMATION_IDLE].TimeAccumulator += GetFrameTime();
			Animations[ANIMATION_RUN].TimeAccumulator += GetFrameTime();

			UpdateFrameIndexAndCurrentIndex(&Animations[ANIMATION_IDLE]);
			UpdateFrameIndexAndCurrentIndex(&Animations[ANIMATION_RUN]);
			
			
			int PlayerTileXPos = (newposX) / 32;
			int playerTileYpos =  (newPosY) / 32;

			//int PlayerBottomPixel = RoundToInt(Ypos) - (frameHeight / 2) + group[0].area[Animations[ANIMATION_IDLE].currentFrame].Miny + group[0].area[Animations[ANIMATION_IDLE].currentFrame].Height;
			//int PlayerTileYpos = PlayerBottomPixel / 32;

			int top = ((newPosY - (Playerheight * 0.5)) - 1)/32;
			int bottom = (newPosY + (Playerheight * 0.5))/32;
			int left = (newposX - (PlayerWidth * 0.5))/32;
			int right = (newposX + (PlayerWidth * 0.5) - 1)/32;

			if (TileMap[top][left] != 1 && TileMap[top][right] != 1 && TileMap[bottom][left] != 1 && TileMap[bottom][right] != 1) 
			{
				
				Xpos = newposX;
				Ypos = newPosY;	

			}

		
			Animate(AnimeState,Xpos, Ypos , 120 , 80 , PlayerTextures , Animations,isflipped);

			
			
#if 0
			switch(AnimeState)
			{
				
				case PLAYER_IDLE: 
				BoundingReigonImg *boxIdle = group[0].area;
				DrawPlayerBoundingBox(Xpos,Ypos,boxIdle,StateIdle,false);
				drawPLayer(TextureIdle,frameWidth,frameHeight,Xpos,Ypos,StateIdle,false);

				break;
				
				case PLAYER_RUNRIGHT:
				//DrawTexturePro(TextureRun ,(Rectangle){(frameWidth * StateRunning.currentFrame) ,0  ,frameWidth ,frameHeight } ,(Rectangle){Xpos,Ypos,120,80}, (Vector2){0,0} , 0.0f, WHITE);
				BoundingReigonImg *boxrun = group[1].area;
				DrawPlayerBoundingBox(Xpos,Ypos,boxrun,StateRunning,false);
				drawPLayer(TextureRun,frameWidth,frameHeight,Xpos,Ypos,StateRunning,false);
				
				break;
				
				case PLAYER_RUNLEFT:
				BoundingReigonImg *boxrunRight = group[1].area;
				DrawPlayerBoundingBox(Xpos,Ypos,boxrunRight,StateRunning,true);
				drawPLayer(TextureRun,frameWidth,frameHeight,Xpos,Ypos,StateRunning,true);
				//DrawTexturePro(TextureRun ,(Rectangle){(frameWidth*StateRunning.currentFrame), 0, -frameWidth ,frameHeight} ,(Rectangle){Xpos-60,Ypos-40,120,80}, (Vector2){0,0} , 0.0f, WHITE);

				// TODO i dont think this is right for player Run right think about creating a newspritesheet for right will make it easier or try something else like also a new bounding box for a flipped image this is a bit cmoplicated for me 

				break;
			}	
#endif			


			break;


			case Editor:
			
			ClearBackground(WHITE);
			
			int ScreenWidth = GetScreenWidth();
			int ScreenHeight = GetScreenHeight();
			float splitLine = ScreenWidth * 0.30;

			int AdjustedReigon = splitLine;
			
			if(splitLine > Tiles2.width)
			{
				
				int reigon = splitLine - Tiles2.width;
				AdjustedReigon = splitLine - reigon; 
				
			}
	
			int numoftiles = AdjustedReigon / 32;
			int visiblewidth = numoftiles * 32;
			
			int newscroll = visiblewidth + scrollingX;


			if(newscroll > Tiles2.width)
			{
				
				scrollingX = 0;
				
			}


			Rectangle PickerArea = {0,0,visiblewidth ,Tiles2.height};
			Rectangle TilesSrc = {scrollingX , 0 , visiblewidth , Tiles2.height};
			
			DrawTexturePro(Tiles2 ,TilesSrc  , PickerArea , (Vector2){0,0}  ,0.0f, WHITE);


			int numGridRows = splitLine/32;
			int numGridHeight = ScreenHeight/32;
			
			// Grid for PickerArea
			for(int i=0; i<numGridRows; i++)
			{
				
				DrawLineV((Vector2){i*32 ,0},(Vector2){i*32,ScreenHeight},LIGHTGRAY);
				
			}
			
			for(int i=0; i<numGridHeight; i++)
			{
				
				DrawLineV((Vector2){0 ,i*32},(Vector2){numoftiles*32,i*32},LIGHTGRAY);
				
			}


			float LevelAreaWidth  = ScreenWidth - visiblewidth;
			int LevelAreaCountWidthSections = LevelAreaWidth/32;
			int LevelAreaCountHeightSections = ScreenHeight / 32;


			for(int i = 0; i < (LevelAreaCountWidthSections ); i++)
			{	
				
				DrawLineV((Vector2){visiblewidth + (i*32), 0}   ,  (Vector2){visiblewidth+(i*32),ScreenHeight} , LIGHTGRAY);

			}
			for(int i = 0; i < LevelAreaCountHeightSections + 1; i++)
			{

				DrawLineV((Vector2){visiblewidth , (i*32)}   ,  (Vector2){ScreenWidth ,i*32} , LIGHTGRAY);

			}

			DrawLineV((Vector2){visiblewidth,0} ,(Vector2){visiblewidth ,ScreenHeight},RED);
			


			Vector2 Tilepicker = GetMousePosition();

			int NumOfTilesInRow = Tiles2.width / 32;

			int Id;
			int posintexx;
			int posinttexy;
			bool tilepicked;
			bool tilechanged = false;


		if(Tilepicker.x < visiblewidth)
		{
			
			if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
			{

				int mosuetexx = (scrollingX + GetMousePosition().x) / 32;
				int mousetexy = GetMousePosition().y / 32;

				Id = mosuetexx + (mousetexy * NumOfTilesInRow);

				posintexx =   mosuetexx * 32;
				posinttexy  =  mousetexy * 32;


				tilepicked = true;
				tilechanged = true;

			}
		}
			Rectangle SourceRect = {posintexx , posinttexy , 32 , 32};
			Rectangle DestRect   = {RoundToInt(GetMousePosition().x / 32 ) * 32 ,RoundToInt(GetMousePosition().y / 32 ) * 32 ,32,32};

			static int TilemapStartpos = 0;

			if(IsKeyPressed(KEY_ENTER))
			{

				TilemapStartpos++;


			}
			if(IsKeyPressed(KEY_BACKSPACE) && TilemapStartpos > 0)
			{

				TilemapStartpos--;


			}
			
			DrawTexturePro(Tiles2 ,SourceRect , DestRect, (Vector2){0,0} , 0 , WHITE);




			if(tilepicked == true)
			{
				if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && tilechanged == false)
				{
	
					Rectangle placeRect = {};

					placeRect.x = RoundToInt(GetMousePosition().x / 32 ) * 32;
					placeRect.y = RoundToInt(GetMousePosition().y / 32 ) * 32;

					placeRect.width = 32;
					placeRect.height = 32;

					int WorldXPos = RoundToInt((GetMousePosition().x - visiblewidth)  / 32);
					int WorldYpos = RoundToInt(GetMousePosition().y / 32);

					TileMap[WorldYpos][WorldXPos + TilemapStartpos] = Id;
							 		
					
				}
			
			}
						
			for(int j=0; j<NumOfTilesHeight; j++)
			{
				for(int i=TilemapStartpos; i<NumOfTilesWidth; i++)
				{ 
					int offset = i-TilemapStartpos;

					int value = TileMap[j][i];
					
					int y = ( value /  NumOfTilesInRow); // iD WILL Start from zero but not the tiles
					
					
					int x = (value - (y * NumOfTilesInRow));
 
					x *= 32;
					y *= 32;

					Rectangle SourceRectFromID = {x , y , 32 ,32};
					Rectangle DestFromMap = {visiblewidth + (offset*32),j*32 , 32,32};  

					DrawTexturePro(Tiles2 ,SourceRectFromID,DestFromMap,(Vector2){0,0},0,WHITE);

					
					
				}
				
			}

			break;



		case TexturePacking: 

		//Learning texture packing (expermentation)
 
		// typedef struct  
		// {
		// 	Rectangle rect;
		// 	Rectangle *next;

		// }RectList;

		
		// void * freelist = MemAlloc(sizeof(RectList)*15);
		// RectList *List = (RectList*)freelist;
		// static bool boxIsEmpty = true;	
		// Rectangle box = {ScreenWidth/2, ScreenHeight/2,200,200};
		// List->rect = box;
		// List->next = NULL;
		// Rectangle RectGoInBox[10] = {};
		// SetRandomSeed(103);
		// DrawRectangle(box.x,box.y,box.width,box.height,RED);
		// DrawRectangle(box.x-(box.width/2),(box.y - box.height/2),box.width,box.height,GREEN);

		// for (int i = 0; i < 10; i++)
		// {
		// 	RectGoInBox[i].width = GetRandomValue(0,150);
		// 	RectGoInBox[i].height = GetRandomValue(0,150);
		// }
		
		
		// for (int i = 0; i < 10; i++)
		// {
		// 	if(boxIsEmpty == true)
		// 	{
		// 	RectGoInBox[i].x = 0;
		// 	RectGoInBox[i].y = 0;

		// 	boxIsEmpty = false;

			
		// 	int box1Height = RectGoInBox[i].height;
		// 	int box1Width = box.width - RectGoInBox[i].width;
			
		// 	int box2Height = box.height - RectGoInBox[i].height;
		// 	int box2width = box.width;


		// 	// allocate memory for list

		// 	}
			
		// 	else
		// 	{

		// 	}


		// }


    }
		
		DrawFPS(10, 10);
		
		
		EndDrawing();
		
    }
	
	UnloadTexture(PlayerTextures[TEXTURE_RUN]);
	UnloadTexture(PlayerTextures[TEXTURE_IDLE]);
	UnloadTexture(PlayerTextures[TEXTURE_ATTACK]);
    // Cleanup
    CloseWindow();
	
    return 0;
}