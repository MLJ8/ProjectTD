// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "PathFinder.generated.h"

USTRUCT()
struct FPoint
{
	GENERATED_BODY()

	int X = 0;
	int Y = 0;
	float Cost = 0;
	int ParentIndex = -1;
};
//根据起始点和终点寻找最短路
UCLASS()
class PROJECTTD_API APathFinder : public AActor
{
	GENERATED_BODY()
private:
	//寻路点显示时间
	const int32 drawtime=5;
protected:
	int32 MapRow=8;
	int32 MapCol=10;
	float z;
	
	
public:
	
	bool bFindPathFinish;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FString> MapInfoArray;
	
	
	//返回起点到终点的路径
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FVector> PathPoint;

	//点记录集
	TArray<FPoint> PathRes;
	TArray<FPoint> OpenSet;
	TArray<FPoint> CloseSet;
	int32 RowStart,ColStart,RowEnd,ColEnd;

	//敌人行走路线点集
	UPROPERTY(VisibleAnywhere)
	TArray<FVector>MonsterWalkLocation;
	// Sets default values for this actor's properties
	APathFinder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	//初始化数值
	virtual void Init(int rStart,int cStart,int rEnd,int cEnd);
	//查找路径
	UFUNCTION(BlueprintCallable)
	virtual void FindPath(int rStart,int cStart,int rEnd,int cEnd);
	virtual void FindPathStep(int rStart,int cStart,int rEnd,int cEnd);
	//画出路径并存储
	virtual void BuildPath(FPoint p);
	//当前格子寻路最终代价
	virtual float Cost(FPoint p);
	
	//对一个点进行判断处理，非openlist的点加入openlist中
	virtual void ProcessPoint(int y, int x, int parentIndex);
	//距离起点距离
	virtual float DistantOfStart(FPoint p);
	//距离终点距离
	virtual float DistantOfEnd(FPoint p);
	//从OpenSet中选择一个总花费最少的点
	virtual int SelectPointInOpenList();
	//根据位置找怪物行走路线的BlockIndex
	int GetBlockIndex(FVector l);

	bool IsStartPoint(FPoint p);
	bool IsEndPoint(FPoint p);
	bool IsValidPoint(int x, int y);
	bool IsInPointList(FPoint p, TArray<FPoint> pointArray);
	bool IsInOpenList(FPoint p); 
	bool IsInCloseList(FPoint p);

	// r 怪物行走道路 
	// s 禁止放置 一般是锁定格子放风景什么的
	// t 防御塔 
	// g 管道（绿色）这颜色有点怪

};
