// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

#include "UObject/ConstructorHelpers.h"
// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MyStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("blockMesh"));
	SetRootComponent(MyStaticMesh);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	if (meshAsset.Succeeded())
		MyStaticMesh->SetStaticMesh(meshAsset.Object);
	SetTextRenderDefault();
}

void ABlock::SetTextRenderText(FString s)
{
	if(s.Equals("s"))
		Text->SetTextRenderColor(FColor(255,0,0));
	else if(s.Equals("r"))
		Text->SetTextRenderColor(FColor(255,255,0));
	else if(s.Equals("t"))
		Text->SetTextRenderColor(FColor(0,0,255));
	else if(s.Equals("p"))
		Text->SetTextRenderColor(FColor(0,255,0));
	else if(s.Equals("0"))
		Text->SetTextRenderColor(FColor(255,255,255));
	Text->SetText(s);
}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlock::SetTextRenderDefault()
{
	//Text临时显示方块信息
	Text=CreateDefaultSubobject<UTextRenderComponent>(TEXT("tr"));
	Text->SetupAttachment(RootComponent);
	Text->SetRelativeTransform(FTransform(FRotator(90,90,0),FVector(0,0,60),FVector(2,2,2)));
	Text->SetText(TEXT("test"));
	Text->SetVerticalAlignment(EVRTA_TextCenter);
	Text->SetHorizontalAlignment(EHTA_Center);
}

