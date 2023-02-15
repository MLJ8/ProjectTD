// Fill out your copyright notice in the Description page of Project Settings.


#include "PathFinderBase.h"
#include "BuildingMap.h"
#include"DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UPathFinderBase::UPathFinderBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
	
}


// Called when the game starts
void UPathFinderBase::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UPathFinderBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPathFinderBase::Init(int rStart, int cStart, int rEnd, int cEnd)
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

void UPathFinderBase::FindPath(int rStart, int cStart, int rEnd, int cEnd)
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
void UPathFinderBase::FindPathStep(int rStart, int cStart, int rEnd, int cEnd)
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
	DrawDebugPoint(GetWorld(), FVector(p.X * 100, p.Y * 100, 70), 20, FColor::Blue, false, 100);

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
}

void UPathFinderBase::BuildPath(FPoint p)
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
	for(int i=PathRes.Num()-1;i>=0;i--)
	{
		FPoint item=PathRes[i];
		// UE_LOG(LogTemp,Warning,TEXT("%d-%d"),item.Y,item.X);
		PathPoint.Add(FVector(item.X * 100,item.Y * 100,70));
		DrawDebugPoint(GetWorld(), FVector(item.X * 100, item.Y * 100, 70), 20, FColor::Green, false, 100);
	}
}


float UPathFinderBase::Cost(FPoint p)
{
	//此点总开销
	return DistantOfStart(p)+DistantOfEnd(p);
}


bool UPathFinderBase::IsInPointList(FPoint p, TArray<FPoint> pointArray)
{
	for (FPoint point : pointArray)
	{
		if (point.X == p.X && point.Y == p.Y) return true;
	}
	return false;
}

bool UPathFinderBase::IsInOpenList(FPoint p)
{
	return IsInPointList(p, OpenSet);
}

bool UPathFinderBase::IsInCloseList(FPoint p)
{
	return IsInPointList(p, CloseSet);
}

void UPathFinderBase::ProcessPoint(int y, int x, int parentIndex)
{
	FPoint p;
	p.Y = y;
	p.X = x;
	
	if (!IsValidPoint(y, x) && !IsEndPoint(p))
		return; // Do nothing for invalid point

	if (IsInCloseList(p)) return; // Do nothing for visited point
	if (!IsInOpenList(p))
	{
		p.ParentIndex = parentIndex;
		p.Cost = Cost(p);
		OpenSet.Add(p);
		DrawDebugPoint(GetWorld(), FVector(p.X * 100, p.Y * 100, 70), 20, FColor::Red, false,100);
	}
}

float UPathFinderBase::DistantOfStart(FPoint p)
{
	return abs(p.X-RowStart)+abs(p.Y-ColStart);
}

float UPathFinderBase::DistantOfEnd(FPoint p)
{
	return abs(p.X-RowEnd)+abs(p.Y-ColEnd);
}

int UPathFinderBase::SelectPointInOpenList()
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

bool UPathFinderBase::IsStartPoint(FPoint p)
{
	return p.Y==RowStart&&p.X==ColStart;
}

bool UPathFinderBase::IsEndPoint(FPoint p)
{
	return p.Y==RowEnd&&p.X==ColEnd;
}

bool UPathFinderBase::IsValidPoint(int y, int x)
{
	//此处需要需要修改条件，先测试算法先
	if (x < 0 || x >= MapCol || y < 0 || y >= MapRow)
		return false;
	else if (!MapInfoArray[y * MapCol + x].Equals(MapInfoArray[RowStart*MapCol+ColStart]))
		return false;
	else
		return true;
}

