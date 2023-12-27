// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicTrigger.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "DreamBotsCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"

// Sets default values
ACinematicTrigger::ACinematicTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TriggerRoot = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	TriggerRoot->SetupAttachment(RootComponent);

	CollisionMesh = CreateDefaultSubobject<UBoxComponent>(FName("CollisionMesh"));
	CollisionMesh->SetupAttachment(TriggerRoot);

	CollisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ACinematicTrigger::OnOverlapBegin);
}


// Called when the game starts or when spawned
void ACinematicTrigger::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ACinematicTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACinematicTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADreamBotsCharacter* character = Cast<ADreamBotsCharacter>(OtherActor);
	if (LevelSequencer && OtherActor == character)
	{
		Character = character;

		if (CinematicHasBeenPlayed)
			return;

		if (Character->GetDisableTick())
			return;

		ULevelSequencePlayer* LevelSequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), LevelSequencer, FMovieSceneSequencePlaybackSettings(),Cinematic);
		if (LevelSequencePlayer)
		{
			CinematicHasBeenPlayed = true;

			LevelSequencePlayer->OnFinished.AddDynamic(this, &ACinematicTrigger::OnCinematicFinished);

			LevelSequencePlayer->Play();

			// Disable character movement
			if (character && character->GetController())
			{
				character->SetCanPlayerMove(false);
			}
			if (IsEndSequence)
			{
				character->GetMesh()->SetVisibility(false,true);
				character->SetActorTickEnabled(false);

				// set the customize settings of the new character to the character you have
				TArray<AActor*> PlayerActors;
				UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Player"), PlayerActors);

				for (ALevelSequenceActor* LevelSequenceActor : TActorRange<ALevelSequenceActor>(GetWorld()))
				{

					for (AActor* PlayerActor : PlayerActors)
					{
						ADreamBotsCharacter* player = Cast<ADreamBotsCharacter>(PlayerActor);
						if(player)
						{
							
							player->ChangeCharacterType(character->GetCharType(), false, true);
						}
					}
				}


			}
		}
	}
}


void ACinematicTrigger::OnCinematicFinished()
{
	if (Character)
	{
		// Re-enable character movement
		if (Character && Character->GetController())
		{
			Character->SetCanPlayerMove(true);
			Character->CheckForCameraZone();
		}
		if (IsEndSequence)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "StartScreen");
		}
	}
}