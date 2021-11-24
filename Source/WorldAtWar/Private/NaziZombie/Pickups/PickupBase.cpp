


#include "NaziZombie/Pickups/PickupBase.h"
#include "Player/NaziZombieCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundWave.h"

// Sets default values
APickupBase::APickupBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(50.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);	
	SetRootComponent(CollisionComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("MeshComponent");
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(GetRootComponent());

	
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();	

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);

		if (RespawnPoints.Num() == 0) return;

		const FVector NewLocation = RespawnPoints[FMath::RandRange(0, RespawnPoints.Num() - 1)]->GetActorLocation();
		SetActorLocation(NewLocation);
	}

	GenerateRotationYaw();
	
}

void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalRotation(FRotator(0.0f, RotationYaw, 0.0f));
}


bool APickupBase::CouldBeTaken() const
{
	return !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle);
}


void APickupBase::NotifyActorBeginOverlap(AActor* OtherActor)
{
	
		const auto Player = Cast<ANaziZombieCharacter>(OtherActor);
		if (Player && CanTakePickup(Player))
		{
			PickupWasTaken();
			if (HasAuthority())
			{
				GivePickupTo(Player);
			}			
		}

		Super::NotifyActorBeginOverlap(OtherActor);
	
}

bool APickupBase::CanTakePickup(ANaziZombieCharacter* Player)
{
	return true;
}

void APickupBase::GivePickupTo(ANaziZombieCharacter* Player)
{	

}

void APickupBase::PickupWasTaken()
{
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	if (GetRootComponent())
	{
		GetRootComponent()->SetVisibility(false, true);		
	}

	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase::GetNextRespawnLocation, RespawnTime);
	}
	

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupTakenSound, GetActorLocation());
}



void APickupBase::GetNextRespawnLocation()
{
	if (RespawnPoints.Num() == 0) return;

	const FVector NewLocation = RespawnPoints[FMath::RandRange(0, RespawnPoints.Num() - 1)]->GetActorLocation();
	SetActorLocation(NewLocation);

	Multi_Respawn(NewLocation);
}



bool APickupBase::Multi_Respawn_Validate(const FVector& Location)
{
	return true;
}

void APickupBase::Multi_Respawn_Implementation(const FVector& Location)
{
	GenerateRotationYaw();	
	if (GetRootComponent())
	{
		GetRootComponent()->SetVisibility(true, true);		
	}

	FTimerHandle TempHandle;
	GetWorldTimerManager().SetTimer(TempHandle, this, &APickupBase::EnableCollision, 0.5f);
}

void APickupBase::EnableCollision()
{
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

void APickupBase::GenerateRotationYaw()
{
	const auto Direction = FMath::RandBool() ? 1.0f : -1.0f;
	RotationYaw = FMath::RandRange(1.0f, 2.0f) * Direction;
}