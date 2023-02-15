// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingMap.h"

#include <cwctype>

#include"Editor.h"
#include "LevelEditorViewport.h"
#include"Block.h"
#include "EditorModeManager.h"
#include "Misc/FileHelper.h"

// Sets default values
ABuildingMap::ABuildingMap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	MyScent=CreateDefaultSubobject<USceneComponent>(TEXT("MyScent"));
	RootComponent=MyScent;
}

// Called when the game starts or when spawned
void ABuildingMap::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuildingMap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuildingMap::ReadCsv()
{
	//获得文件路径
	FString path=FPaths::ProjectDir()+"Level/"+FileName;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
	{
		UE_LOG(LogTemp, Error, TEXT("%s not exist"), *path);
	}
	FString tmp;
	//将文件内容写入tmp中，
	FFileHelper::LoadFileToString(tmp,*(path));

	//遍历tmp，并处理换行符和回车符，用‘,'分割每个数据
	FString NewString;
	for(TCHAR q:tmp)
	{
		if(std::iswspace(q))
		{
			if(q=='\n')
				continue;
			q=',';
			NewString+=',';
		}else
			NewString+=q;
	}
	//这种写法尾部会多一个,到时TArray多一个空元素
	NewString.ParseIntoArray(MapInfoArray,TEXT(","),false);


	// FString pc;
	// for(int i=0;i<MapInfoArray.Num();i++)
	// {
	// 	pc+=MapInfoArray[i];
	// 	if(i%MapCol==MapCol-1)
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("%s"), *pc);
	// 		pc.Empty();
	// 	}
	// }
	
}

void ABuildingMap::ClearBlock()
{
	for(int i=0;i<BlockList.Num();i++)
	{
		if(IsValid(BlockList[i]))
			BlockList[i]->Destroy();
	}
	BlockList.Empty();
	
}

void ABuildingMap::SetMapInfo(int row, int col, FString s)
{
	// UE_LOG(LogTemp,Warning,TEXT("test %d %d %s"),row,col,*s)
	MapInfoArray[row*MapCol+col]=s;
	UpdateOneTextRender(row,col);
}

void ABuildingMap::UpdateAllTextRender()
{
	for(int y=0;y<MapRow;y++)
	{
		for(int x=0;x<MapCol;x++)
		{
			BlockList[y*MapCol+x]->SetTextRenderText(MapInfoArray[y*MapCol+x]);
		}
	}
}

void ABuildingMap::UpdateOneTextRender(int row, int col)
{
	BlockList[row*MapCol+col]->SetTextRenderText(MapInfoArray[row*MapCol+col]);
}

//地图坐标轴为x轴正方向为列递增，y轴正方向为行递增
void ABuildingMap::CreateMap()
{
	ReadCsv();
	ClearBlock();
	if(GEditor)
	{
		//获得关卡，此种方法不适用于流关卡
		UWorld* world=GEditor->GetLevelViewportClients()[0]->GetWorld();
		ULevel* level=world->GetLevel(0);
		//行为y 列为x
		for(int y=0;y<MapRow;y++)
		{
			for(int x=0;x<MapCol;x++)
			{
				//创建地图基底方块并改名修改变换
				FVector location=FVector(x*100.0,y*100.0,0);
				FTransform transform(FRotator::ZeroRotator, location, FVector::OneVector);
				AActor* block = GEditor->AddActor(level, ABlock::StaticClass(), transform);
				block->SetActorLabel(FString::Printf(TEXT("%d-%d"), y, x));
				//显示基底对应的关卡数据 日后隐藏
				ABlock *tmp=Cast<ABlock>(block);
				BlockList.Add(tmp);
				UpdateOneTextRender(y,x);
				
				block->RegisterAllComponents();
				block->RerunConstructionScripts();
			}
		}
		GEditor->EditorUpdateComponents();
		world->UpdateWorldComponents(true, false);
		GLevelEditorModeTools().MapChangeNotify();
	}
	
}

