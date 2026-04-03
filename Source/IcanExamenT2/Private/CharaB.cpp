#include "CharaB.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ACharaB::ACharaB()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ACharaB::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	EnterIdle();
}

void ACharaB::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateState(DeltaTime);
}

bool ACharaB::CanSeePlayer() const
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return false;
	
	const FVector Start = GetActorLocation() + FVector(0.f, 0.f, 50.f);
	const FVector End = Player->GetActorLocation() + FVector(0.f, 0.f, 50.f);

	if (FVector::Dist(Start, End) > SightDistance)
	{
		return false;
	}

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);


	
	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
		
	);

	GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, FString::Printf(TEXT("CanSeePlayer: %s"), bHit ? *Hit.GetActor()->GetName() : TEXT("None")));

	return bHit && Hit.GetActor() == Player;
}

void ACharaB::UpdateState(float DeltaTime)
{
	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	const bool bSeesPlayer = CanSeePlayer();
	const float Dist = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

	if (bSeesPlayer)
	{
		TargetActor = Player;
		LostSightTimer = 0.f;
	}
	else
	{
		LostSightTimer += DeltaTime;
		if (LostSightTimer >= LoseSightTime)
		{
			TargetActor = nullptr;
		}
	}

	switch (State)
	{
	case ECharaBState::Idle:
		if (TargetActor)
		{
			EnterChase();
		}
		break;

	case ECharaBState::Chase:
		if (!TargetActor)
		{
			EnterIdle();
			return;
		}

		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			AI->MoveToActor(TargetActor, 5.f);
		}

		if (Dist <= AttackDistance && bCanAttack)
		{
			EnterAttack();
		}
		break;

	case ECharaBState::Attack:
		break;
	}
}

void ACharaB::EnterIdle()
{
	State = ECharaBState::Idle;
	TargetActor = nullptr;

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}
}

void ACharaB::EnterChase()
{
	State = ECharaBState::Chase;
}

void ACharaB::EnterAttack()
{
	State = ECharaBState::Attack;
	bCanAttack = false;

	if (AAIController* AI = Cast<AAIController>(GetController()))
	{
		AI->StopMovement();
	}

	float Duration = 0.8f;
	if (KickMontage)
	{
		const float Played = PlayAnimMontage(KickMontage);
		if (Played > 0.f)
		{
			Duration = Played;
		}
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&ACharaB::FinishAttack,
		Duration,
		false
	);
}

void ACharaB::FinishAttack()
{
	bCanAttack = true;
	EnterIdle();
}