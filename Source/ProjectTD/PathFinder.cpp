// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFinder.h"
#include "BuildingMap.h"
#include"DrawDebugHelpers.h"
#include"MonsterSpawn.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
APathFinder::APathFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void APathFinder::BeginPlay()
{
	Super::BeginPlay();
	AActor * ams=UGameplayStatics::GetActorOfClass(this,AMonsterSpawn::StaticClass());
	MonsterWalkLocation=Cast<AMonsterSpawn>(ams)->MonsterWalkBlockLocation;
	z=MonsterWalkLocation[0].Z;
	// for(int i=0;i<MonsterWalkLocation.Num();i++)
	// {
	// 	UE_LOG(LogTemp,Warning,TEXT("array index=%d x=%f y=%f,z=%f"),i,MonsterWalkLocation[i].X,MonsterWalkLocation[i].Y,MonsterWalkLocation[i].Z)
	// }
	
	
	
}

// Called every frame
void APathFinder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APathFinder::Init(int rStart, int cStart, int rEnd, int cEnd)
{
	//寻路初始化赋值并将起点添加进OpenSet
	RowStart=rStart;
	ColStart=cStart;
	RowEnd=rEnd;
	ColEnd=cEnd;
	
	OpenSet.Empty();
	CloseSet.Empty();

	FPoint Start;
	Start.X=cStart;
	Start.Y=rStart;
	OpenSet.Add(Start);
	
	PathPoint.Empty();
	PathRes.Empty();
	bFindPathFinish=false;

	//读取地图生成器的信息
	AActor *buildingmap=UGameplayStatics::GetActorOfClass(this,ABuildingMap::StaticClass());
	
	if(buildingmap)
	{
		MapInfoArray=Cast<ABuildingMap>(buildingmap)->MapInfoArray;
		MapRow=Cast<ABuildingMap>(buildingmap)->MapRow;
		MapCol=Cast<ABuildingMap>(buildingmap)->MapCol;
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Can't find the BuildingMap"))
	}
}

void APathFinder::FindPath(int rStart, int cStart, int rEnd, int cEnd)
{
	//寻找最短路
	Init(rStart, cStart, rEnd, cEnd);
	// UE_LOG(LogTemp,Warning,TEXT("FindPathStart%d %d %d %d"),rStart,cStart,rEnd,cEnd)
	while(!bFindPathFinish)
	{
		FindPathStep(rStart,cStart,rEnd,cEnd);
	}
	
}
// 红色点：ProcessPoint阶段，将该点放入OpenSet。
// 蓝色点：FindPathStep阶段，对SelectPointInOpenList筛选出的点进行分析。
// 绿色点：BuildPath阶段，最终路径。
void APathFinder::FindPathStep(int rStart, int cStart, int rEnd, int cEnd)
{
	int index = SelectPointInOpenList();
	//判断OpenList中还有没有待计算的点
	if (index < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No path found, algorithm failed!!!"));
		bFindPathFinish=true;
		return;
	}

	FPoint p = OpenSet[index];
	
	// UE_LOG(LogTemp,Warning,TEXT("Draw %d %d"),p.Y,p.X)
	// 蓝色点：FindPathStep阶段，对SelectPointInOpenList筛选出的点进行分析。
	DrawDebugPoint(GetWorld(), FVector(p.X * 100, p.Y * 100, 100), 20, FColor::Blue, false, 5);

	if (IsEndPoint(p)) // 如果是终点，算法结束，构建路径
		{
		BuildPath(p);
		bFindPathFinish=true;
		return;
		}

	// 将p移出OpenSet，移入CloseSet
	OpenSet.RemoveAt(index);
	CloseSet.Add(p);

	// Process all neighbors
	int y = p.Y;
	int x = p.X;
	//寻找上下左右的非Open点放入Openlist中
	ProcessPoint(y - 1, x, CloseSet.Num() - 1);
	ProcessPoint(y, x + 1, CloseSet.Num() - 1);
	ProcessPoint(y, x - 1, CloseSet.Num() - 1);
	ProcessPoint(y + 1, x, CloseSet.Num() - 1);
	// UE_LOG(LogTemp,Warning,TEXT("====="))
}

void APathFinder::BuildPath(FPoint p)
{
	PathRes.Empty();
	FPoint point = p;
	PathRes.Add(p);

	while (true)
	{
		if (IsStartPoint(point)) break; // 如果是起点，终止
		else
		{
			point = CloseSet[point.ParentIndex];
			if (!IsStartPoint(point))
			{
				PathRes.Add(point);
			}
		}
	}
	PathPoint.Add(FVector(GetActorLocation().X,GetActorLocation().Y,100));
	for(int i=PathRes.Num()-1;i>=0;i--)
	{
		FPoint item=PathRes[i];
		// UE_LOG(LogTemp,Warning,TEXT("%d-%d"),item.Y,item.X);
		PathPoint.Add(FVector(item.X * 100,item.Y * 100,100));
		// 绿色点：BuildPath阶段，最终路径。
		DrawDebugPoint(GetWorld(), FVector(item.X * 100, item.Y * 100, 100), 20, FColor::Green, false, 5);
	}
}


float APathFinder::Cost(FPoint p)
{
	//此点总开销
	return DistantOfStart(p)+DistantOfEnd(p);
}


bool APathFinder::IsInPointList(FPoint p, TArray<FPoint> pointArray)
{
	for (FPoint point : pointArray)
	{
		if (point.X == p.X && point.Y == p.Y) return true;
	}
	return false;
}

bool APathFinder::IsInOpenList(FPoint p)
{
	return IsInPointList(p, OpenSet);
}

bool APathFinder::IsInCloseList(FPoint p)
{
	return IsInPointList(p, CloseSet);
}

void APathFinder::ProcessPoint(int y, int x, int parentIndex)
{
	FPoint p;
	p.Y = y;
	p.X = x;
	// UE_LOG(LogTemp,Warning,TEXT("proadd row=%d col=%d"),p.Y,p.X)
	if (!IsValidPoint(y, x) && !IsEndPoint(p))
	{
		// UE_LOG(LogTemp,Warning,TEXT("a"))
		return; // Do nothing for invalid point
	}
		

	if (IsInCloseList(p))
	{
		// UE_LOG(LogTemp,Warning,TEXT("b"))
		return; // Do nothing for visited point
	}
	if (!IsInOpenList(p))
	{
		
		p.ParentIndex = parentIndex;
		p.Cost = Cost(p);
		OpenSet.Add(p);
		// 红色点：ProcessPoint阶段，将该点放入OpenSet。
		DrawDebugPoint(GetWorld(), FVector(p.X * 100, p.Y * 100, 100), 20, FColor::Red, false,5);
	}
}

float APathFinder::DistantOfStart(FPoint p)
{
	return abs(p.Y-RowStart)+abs(p.X-ColStart);
}

float APathFinder::DistantOfEnd(FPoint p)
{
	return abs(p.Y-RowEnd)+abs(p.X-ColEnd);
}

int APathFinder::SelectPointInOpenList()
{
	int index = -1;
	int minCost = 99999;
	for (int i = 0; i < OpenSet.Num(); i++)
	{
		int cost = Cost(OpenSet[i]);
		if (cost < minCost)
		{
			minCost = cost;
			index = i;
		}
	}

	return index;
}

int APathFinder::GetBlockIndex(FVector l)
{
	
	for(int i=0;i<MonsterWalkLocation.Num();i++)
	{
		if(l.Equals(MonsterWalkLocation[i]))
			return i;
	}
	UE_LOG(LogTemp,Error,TEXT("APathFinder::GetBlockIndex return -1"))
	UE_LOG(LogTemp,Error,TEXT("Vector=%f %f %f"),l.X,l.Y,l.Z)
	return -1;
}

bool APathFinder::IsStartPoint(FPoint p)
{
	return p.Y==RowStart&&p.X==ColStart;
}

bool APathFinder::IsEndPoint(FPoint p)
{
	return p.Y==RowEnd&&p.X==ColEnd;
}

bool APathFinder::IsValidPoint(int y, int x)
{
	//此处需要需要修改条件，先测试算法先
	if (x < 0 || x >= MapCol || y < 0 || y >= MapRow)
		return false;
	else if (!(MapInfoArray[y * MapCol + x].Equals(TEXT("p"))||MapInfoArray[y * MapCol + x].Equals(TEXT("r"))))
		return false;
	else
		return true;
}

