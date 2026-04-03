#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharaB.generated.h"

UENUM(BlueprintType)
enum class ECharaBState : uint8
{
	Idle,
	Chase,
	Attack
};

UCLASS()
class ICANEXAMENT2_API ACharaB : public ACharacter
{
	GENERATED_BODY()

public:
	ACharaB();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category="AI")
	float SightDistance = 2000.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float AttackDistance = 150.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float WalkSpeed = 420.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float LoseSightTime = 1.0f;

	UPROPERTY(EditAnywhere, Category="Animation")
	UAnimMontage* KickMontage = nullptr;

	UPROPERTY(VisibleAnywhere, Category="AI")
	ECharaBState State = ECharaBState::Idle;

	UPROPERTY()
	AActor* TargetActor = nullptr;

	float LostSightTimer = 0.f;
	bool bCanAttack = true;

	bool CanSeePlayer() const;
	void UpdateState(float DeltaTime);
	void EnterIdle();
	void EnterChase();
	void EnterAttack();
	void FinishAttack();
};