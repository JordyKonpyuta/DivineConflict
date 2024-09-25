// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnumsList.h"
#include "InteractInterface.h"
#include "Unit.generated.h"

class ABase;
class AGrid;
class UStaticMeshComponent;
class ACustomPlayerController;

UCLASS()
class DIVINECONFLICT_API AUnit : public APawn , public IInteractInterface
{
	GENERATED_BODY()

	


public:
	// Sets default values for this pawn's properties
	AUnit();

	virtual bool Interact_Implementation(ACustomPlayerController* PlayerController) override;

	UPROPERTY( EditAnywhere, BlueprintReadOnly ,Category = "Unit")
	AGrid* Grid;

	UFUNCTION()
	void interaction(ACustomPlayerController *PlayerController);

	UFUNCTION(BlueprintCallable)
	void SetGrid(AGrid* NewGrid);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY( VisibleAnywhere, BlueprintReadOnly, Category = "Unit", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* UnitMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unit")
	EPlayer PlayerOwner = EPlayer::P_Neutral;
	
	UPROPERTY()
	ACustomPlayerController* PlayerControllerRef;
	
	void SetGrid();

	

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;
	
	UPROPERTY()
	UTexture2D* UnitIcon;
	
	// Units stats
	UPROPERTY()
	int Attack = 0;
	
	UPROPERTY()
	int Defense = 0;
	
	UPROPERTY()
	int CurrentHealth = 0;
	
	UPROPERTY()
	int MaxHealth = 0;
	
	UPROPERTY()
	int MovementCost = 0;
	
	UPROPERTY()
	int AttackCost = 0;
	
	UPROPERTY()
	int SpellCost = 0;

	UPROPERTY()
	bool IsSelected = false;

	UPROPERTY()
	bool IsHell = false;

	UPROPERTY()
	int TotalDamagesInflicted = 0;

	UPROPERTY()
	FString Name = "";

	UPROPERTY()
	int PM = 3;

	UPROPERTY()
	FIntPoint IndexPosition = FIntPoint(-999, -999);

	UPROPERTY()
	TArray<FIntPoint> Path;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION( BlueprintCallable)
	void Move(TArray<FIntPoint> PathToFollow);

	UFUNCTION(BlueprintCallable)
	void AttackUnit(AUnit* UnitToAttack);

	UFUNCTION(BlueprintCallable)
	void AttackBase(ABase* BaseToAttack);
	

	

	// Getter for units stats
	UFUNCTION(BlueprintCallable)
	int GetAttack();

	UFUNCTION(BlueprintCallable)
	int GetDefense();

	UFUNCTION(BlueprintCallable)
	int GetCurrentHealth();

	UFUNCTION(BlueprintCallable)
	int GetMaxHealth();

	UFUNCTION(BlueprintCallable)
	int GetMovementCost();

	UFUNCTION(BlueprintCallable)
	int GetAttackCost();

	UFUNCTION(BlueprintCallable)
	int GetSpellCost();

	UFUNCTION(BlueprintCallable)
	bool GetIsSelected();

	UFUNCTION(BlueprintCallable)
	bool GetIsHell();

	UFUNCTION(BlueprintCallable)
	int GetTotalDamageInflicted();

	UFUNCTION(BlueprintCallable)
	FString GetName();

	UFUNCTION(BlueprintCallable)
	int GetPM();

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetUnitIcon();

	UFUNCTION()
	FIntPoint GetIndexPosition();

	UFUNCTION()
	EPlayer GetPlayerOwner();

	// Setter for units stats
	UFUNCTION(BlueprintCallable)
	void SetAttack(int a);

	UFUNCTION(BlueprintCallable)
	void SetDefense(int d);

	UFUNCTION(BlueprintCallable)
	void SetCurrentHealth(int ch);

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int mh);

	UFUNCTION(BlueprintCallable)
	void SetMovementCost(int mc);

	UFUNCTION(BlueprintCallable)
	void SetAttackCost(int ac);

	UFUNCTION(BlueprintCallable)
	void SetSpellCost(int sc);

	UFUNCTION(BlueprintCallable)
	void SetIsSelected(bool s);

	UFUNCTION(BlueprintCallable)
	void SetIsHell(bool h);

	UFUNCTION(BlueprintCallable)
	void SetTotalDamageInflicted(int tdi);

	UFUNCTION(BlueprintCallable)
	void SetName(FString n);

	UFUNCTION(BlueprintCallable)
	void SetPM(int p);

	UFUNCTION(BlueprintCallable)
	void SetUnitIcon(UTexture2D* i);

	UFUNCTION()
	void SetIndexPosition(FIntPoint ip);

	UFUNCTION()
	void SetPlayerOwner(EPlayer po);
	
};
