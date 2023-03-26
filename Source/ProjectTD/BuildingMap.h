// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <array>

#include "CoreMinimal.h"
#include "Block.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "BuildingMap.generated.h"
//用于读取csv的地图信息并生成地图格子
//所有的地图信息存储于此

UCLASS()
class PROJECTTD_API ABuildingMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingMap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MapRow=8;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 MapCol=10;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString FileName;//="l1.csv";
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly);
	TArray<FString>MapInfoArray;


	//设置单个的地图信息
	UFUNCTION(BlueprintCallable, CallInEditor)
	void SetMapInfo(int row,int col,FString s);
	//更新所有格子显示的文字以及颜色
	UFUNCTION(BlueprintCallable, CallInEditor)
	void UpdateAllTextRender();
	//更新单个格子显示的文字以及颜色
	UFUNCTION(BlueprintCallable, CallInEditor)
	void UpdateOneTextRender(int row,int col);
	//创建地图
	UFUNCTION(BlueprintCallable, CallInEditor)
	void CreateMap();
	//读取csv信息并存储到MapInfoArray
	virtual bool ReadCsv();
	//清除所有的block
	UFUNCTION(BlueprintCallable, CallInEditor)
	void ClearBlock();

	UPROPERTY(VisibleAnywhere);
	TArray<ABlock*>BlockList;
	USceneComponent* MyScent;
	
	

};
