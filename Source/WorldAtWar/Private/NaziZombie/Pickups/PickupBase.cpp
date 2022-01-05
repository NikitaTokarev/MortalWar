

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
	PrimaryActorTick.bCanEverTick = false;	

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

		CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnOverlapBegin);

		if (RespawnPoints.Num() != 0 && bChangeLocationOnBegin)
		{
			int32 RandomIndex = FMath::RandRange(0, RespawnPoints.Num() - 1);
			const AActor* RandomPoint = RespawnPoints[RandomIndex];
			checkf(RandomPoint, TEXT("Respawn Point Is Not Valid"));

			SetActorLocation(RandomPoint->GetActorLocation());			
		}		
	}
	
	
}



bool APickupBase::CouldBeTaken() const
{
	return !GetWorldTimerManager().IsTimerActive(RespawnTimerHandle);
}


void APickupBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		const auto Player = Cast<ANaziZombieCharacter>(OtherActor);
		if (Player && OtherComp->GetClass() != Player->GetInteractableCompClass() && CanTakePickup(Player))
		{			
			Multi_PickupWasTaken();
			GivePickupTo(Player);
		}		
	}
}

//void APickupBase::NotifyActorBeginOverlap(AActor* OtherActor)
//{
//	
//}

bool APickupBase::CanTakePickup(ANaziZombieCharacter* Player)
{
	return true;
}

void APickupBase::GivePickupTo(ANaziZombieCharacter* Player)
{	

}





bool APickupBase::Multi_PickupWasTaken_Validate()
{
	return true;
}



void APickupBase::Multi_PickupWasTaken_Implementation()
{
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	if (GetRootComponent())
	{
		GetRootComponent()->SetVisibility(false, true);		
	}

	if (HasAuthority())
	{
		if (bIsRespawnables)
		{
			GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase::GetNextRespawnLocation, RespawnTime);
		}
		else
		{
			Destroy();
		}
	}
	

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupTakenSound, GetActorLocation());
}



void APickupBase::GetNextRespawnLocation()
{
	if (RespawnPoints.Num() != 0)
	{
		const FVector NewLocation = RespawnPoints[FMath::RandRange(0, RespawnPoints.Num() - 1)]->GetActorLocation();
		SetActorLocation(NewLocation);
	}	

	Multi_Respawn();
}



bool APickupBase::Multi_Respawn_Validate()
{
	return true;
}

void APickupBase::Multi_Respawn_Implementation()
{		
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