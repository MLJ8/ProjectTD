// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class PROJECTTD_API ABlock : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlock();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MyStaticMesh;
	UPROPERTY(VisibleAnywhere)
	UTextRenderComponent* Text;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//设置信息并更新
	virtual void SetTextRenderText(FString s);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//已废弃
	void SetTextRenderDefault();

};
