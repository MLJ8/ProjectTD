// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "PathFinderBase.generated.h"

USTRUCT()
struct FPoint
{
	GENERATED_BODY()

	int X = 0;
	int Y = 0;
	float Cost = 0;
	int ParentIndex = -1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTTD_API UPathFinderBase : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPathFinderBase();

	// UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MapRow=8;
	// UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MapCol=10;
	bool bFindPathFinish;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FString> MapInfoArray;
	
	TArray<FPoint> PathRes;
	TArray<FPoint> OpenSet;
	TArray<FPoint> CloseSet;
	int RowStart,ColStart,RowEnd,ColEnd;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//初始化
	virtual void Init(int rStart,int cStart,int rEnd,int cEnd);
	
	//查找路径
	USplineComponent *SplinePath;
	TArray<FVector> PathPoint;
	UFUNCTION(BlueprintCallable)
	virtual void FindPath(int rStart,int cStart,int rEnd,int cEnd);
	virtual void FindPathStep(int rStart,int cStart,int rEnd,int cEnd);
	//画出路径
	virtual void BuildPath(FPoint p);
	// 最终代价
	virtual float Cost(FPoint p);
	
	//对一个点进行处理，非openlist的点加入openlist中
	virtual void ProcessPoint(int y, int x, int parentIndex);
	//距离起点距离
	virtual float DistantOfStart(FPoint p);
	//距离终点距离
	virtual float DistantOfEnd(FPoint p);
	//从OpenSet中选择一个总花费最少的点
	virtual int SelectPointInOpenList();

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
