// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLevelScriptActor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PROGETTO/Components/DayNightCycleComponent.h"

AMyLevelScriptActor::AMyLevelScriptActor()
{
    // Aggiungi il componente DayNightCycleComponent
    DayNightCycleComponent = CreateDefaultSubobject<UDayNightCycleComponent>(TEXT("DayNightCycleComponent"));

	// Crea il componente audio per la musica
	MusicAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicAudioComponent"));
}

void AMyLevelScriptActor::BeginPlay()
{
    Super::BeginPlay();

    if (!MusicAudioComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("MusicAudioComponent non è stato creato correttamente!"));
    }

    if (DayNightCycleComponent)
    {

        UE_LOG(LogTemp, Warning, TEXT("Componente DayNightCycle trovato!"));

        // Ascolta gli eventi di inizio giorno e inizio notte
        DayNightCycleComponent->OnDayStart.AddDynamic(this, &AMyLevelScriptActor::OnDayStartHandler);
        DayNightCycleComponent->OnNightStart.AddDynamic(this, &AMyLevelScriptActor::OnNightStartHandler);
    }

    else
    {
        UE_LOG(LogTemp, Error, TEXT("Componente DayNightCycle non trovato!"));
    }
    
}

void AMyLevelScriptActor::OnNightStartHandler()
{
    
    if (MusicAudioComponent && NightMusicCue)
    {
        MusicAudioComponent->SetSound(NightMusicCue);
        MusicAudioComponent->Play();
    }
}

void AMyLevelScriptActor::OnDayStartHandler()
{
    
    if (MusicAudioComponent && NightMusicCue)
    {
        MusicAudioComponent->Stop();
    }
}
